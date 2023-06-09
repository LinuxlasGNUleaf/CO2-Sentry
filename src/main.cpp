#include "main.h"

// time variables
DateTime last_rtc = DateTime((uint32_t)0);
DateTime current_rtc = DateTime((uint32_t)0);
unsigned long last_i2c_call = 0;

// led state variables
unsigned long last_led_change = 0;
bool led_state = false;
bool ss_pin_state = false;

void setup()
{
  WDT_FEED();
  // LED SETUP
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, LOW);

  Serial.begin(9600);
  Serial.println("Connecting to RTC...");
  ESP.wdtFeed();
  rtc.begin();
  Serial.println(rtc.lostPower());

  Serial.println("Connecting DHT...");
  ESP.wdtFeed();
  dht.begin();

  Serial.println("Connecting CCS...");
  ESP.wdtFeed();
  if (!ccs.begin())
  {
    Serial.println(F("Failed to start CCS module! Please check your wiring."));
    while (1)
      ;
  }

  Serial.println("Connecting SD...");
  ESP.wdtFeed();
  if (!sd.begin(D8, SD_SCK_MHZ(5)))
  {
    Serial.println("Error while initializing SD card!");
    while (1)
      ;
  }
}

// current values
float current_temperature = 0;
float current_humidity = 0;
uint16_t current_co2 = 0;
uint16_t current_tvoc = 0;

void loop()
{
  if (pollScheduled())
  {
    // update values
    ESP.wdtFeed();
    readCCS811(&current_co2, &current_tvoc);
    ESP.wdtFeed();
    readDHT(&current_temperature, &current_humidity);

    char ret[60];
    sprintf(ret, "%d, %lf, %lf, %d, %d", current_rtc.unixtime(), current_temperature, current_humidity, current_co2, current_tvoc);
    writeLineToSDCard(ret);
  }
  handleLEDs();
  checkForIncoming();
}

bool pollScheduled()
{
  if (millis() - last_i2c_call < i2c_cooldown * 1000)
  {
    return false;
  }
  last_i2c_call = millis();
  current_rtc = rtc.now();

  if (current_rtc.unixtime() - last_rtc.unixtime() > poll_interval)
  {
    last_rtc = current_rtc;
    return true;
  }
  return false;
}

void readDHT(float *temperature, float *humidity)
{
  *temperature = dht.readTemperature();
  *humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(*temperature) || isnan(*humidity))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

void readCCS811(uint16_t *co2, uint16_t *tvoc)
{
  if (!ccs.available())
  {
    Serial.println(F("Failed to read from CCS sensor!"));
    return;
  }
  if (!ccs.readData())
  {
    *co2 = ccs.geteCO2();
    *tvoc = ccs.getTVOC();
  }
}

void handleLEDs()
{
  if ((led_state == true && millis() - last_led_change < led_on_interval) || (led_state == false && millis() - last_led_change < led_off_interval))
    return;
  last_led_change = millis();
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, LOW);
  led_state = !led_state;
  ss_pin_state = false;
  if (led_state == false)
    return;
  if (current_co2 < good_co2_threshold)
    analogWrite(LED_G, 10);
  else if (current_co2 > good_co2_threshold && (current_co2 < medium_co2_threshold))
  {
    analogWrite(LED_Y, 10);
    ss_pin_state = true;
  }
  else
    analogWrite(LED_R, 10);
}

void writeLineToSDCard(char *line)
{  
  ESP.wdtDisable();
  digitalWrite(LED_Y, !ss_pin_state);
  char fname[30];
  sprintf(fname, "%04d-%02d-%02d.csv", current_rtc.year(), current_rtc.month(), current_rtc.day());
  Serial.println(fname);
  ofstream sdout(fname, FILE_WRITE);
  if (!sdout)
  {
    Serial.println("Failed to open file.");
    while (1)
      ;
  }
  sdout << line << endl;
  sdout.close();
  Serial.println(line);
  digitalWrite(LED_Y, ss_pin_state);
  ESP.wdtEnable((uint32_t) 1000);
}

void checkForIncoming()
{
  Serial.setTimeout(10);
  char cmd;
  if (Serial.readBytes(&cmd, 1) != 1)
  {
    return;
  }
  switch (cmd)
  {
  case PING:
    Serial.println(PING);
    break;
  case TSTAMP_PUSH:
  {
    String ts_buf = Serial.readStringUntil('\n');
    uint32_t ts = 0;
    if (!sscanf(ts_buf.c_str(), "%u", &ts))
    {
      Serial.printf("TS read failed: %d\n", ts);
      return;
    }
    DateTime dt = DateTime(ts);
    rtc.adjust(dt);
    Serial.printf("Time adjusted to: %04d-%02d-%02d-%02d:%02d:%02d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    break;
  }
  default:
  {
    break;
  }
  }
}