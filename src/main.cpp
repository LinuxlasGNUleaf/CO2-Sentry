#include "main.h"

// time variables
DateTime current_rtc = DateTime((uint32_t)0);
uint32_t last_rtc_ts = 0;
unsigned long last_i2c_call = 0;

// led state variables
unsigned long last_led_change = 0;
bool led_state = false;
bool y_pin_state = false;

// sd buffer
uint8_t sd_buffer_i = 0;
DataPoint sd_buffer[sd_buffer_size];

DataPoint current_data;

void setup()
{
  PAUSE_WDT;

  Serial.begin(9600);

  Serial.print("Connecting SD...");
  if (!sd.begin(D8, SD_SCK_MHZ(5)))
  {
    Serial.println("failed.\nCheck wiring and presence of SD card.");
    delay(500);
    ESP.restart();
  }
  Serial.println("done.");

  Serial.println("Connecting to RTC...");
  if (!rtc.begin())
  {
    Serial.println("failed.\nPlease check your wiring.");
  }
  Serial.println("done.");
  if (rtc.lostPower())
    Serial.println("Chip lost power in the meantime! Please resync the RTC clock.");

  Serial.print("Connecting DHT...");
  dht.begin();
  Serial.println("done.");

  Serial.print("Connecting CCS...");
  if (!ccs.begin())
  {
    Serial.println(F("failed.\nPlease check your wiring."));
    delay(500);
    ESP.restart();
  }
  ccs.readData();
  Serial.println("done.");

  // LED SETUP
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, LOW);
  CONT_WDT;
}

void loop()
{
  if (pollScheduled())
  {
    // update values
    ESP.wdtFeed();
    readDHT(&current_data.temp, &current_data.hmdty);
    ESP.wdtFeed();
    readCCS811(&current_data.co2, &current_data.tvoc);
    current_data.ts = current_rtc.unixtime();

    // write to buffer and increment index
    memcpy(&sd_buffer[sd_buffer_i], &current_data, sizeof(DataPoint));
    sd_buffer_i++;

    if (sd_buffer_i == sd_buffer_size)
    {
      writeBufferToSD();
    }
    last_rtc_ts = current_rtc.unixtime();
  }
  handleLEDs();
  checkForIncoming();
}

bool pollScheduled()
{
  if (millis() - last_i2c_call < i2c_cooldown)
  {
    return false;
  }
  last_i2c_call = millis();
  current_rtc = rtc.now();

  if (current_rtc.unixtime() - last_rtc_ts > poll_interval)
  {
    last_rtc_ts = current_rtc.unixtime();
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
  y_pin_state = false;
  if (led_state == false)
    return;
  if (current_data.co2 < good_co2_threshold)
    analogWrite(LED_G, 10);
  else if ((current_data.co2 > good_co2_threshold) && (current_data.co2 < medium_co2_threshold))
  {
    analogWrite(LED_Y, 10);
    y_pin_state = true;
  }
  else
    analogWrite(LED_R, 10);
}

void writeBufferToSD()
{
  PAUSE_WDT;
  unsigned long start_warn_ts = millis();
  while (millis() - start_warn_ts < sd_buffer_warn_time)
  {
    digitalWrite(LED_Y, HIGH);
    delay(50);
    digitalWrite(LED_Y, LOW);
    delay(50);
  }
  digitalWrite(LED_Y, y_pin_state);

  char fname[30];
  sprintf(fname, FNAME_STRING, current_rtc.year(), current_rtc.month(), current_rtc.day());
  Serial.printf("Writing %d entries to %s...\n", sd_buffer_i, fname);
  File32 saveFile = sd.open(fname, FILE_WRITE);
  if (!saveFile)
  {
    Serial.println(F("Failed to open file."));
    while (1)
      ;
  }
  char linebuf[50];
  for (uint8_t i = 0; i < sd_buffer_i; i++)
  {
    sprintf(linebuf, DATA_STRING, sd_buffer[i].ts, sd_buffer[i].temp, sd_buffer[i].hmdty, sd_buffer[i].co2, sd_buffer[i].tvoc);
    saveFile.write(linebuf);
    saveFile.write('\n');
  }
  saveFile.close();
  sd_buffer_i = 0;
  CONT_WDT;
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
    Serial.print(PING);
    break;
  case TSTAMP_PUSH:
  {
    String ts_buf = Serial.readStringUntil('\n');
    uint32_t ts = 0;

    if (sscanf(ts_buf.c_str(), "%u", &ts) != 1)
    {
      Serial.printf("TS read failed: %d\n", ts);
      return;
    }
    DateTime dt = DateTime(ts);
    rtc.adjust(dt);
    Serial.printf("Time adjusted to: %04d-%02d-%02d-%02d:%02d:%02d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    break;
  }
  case RESET:
    ESP.restart();
    break;
  default:
  {
    break;
  }
  }
}