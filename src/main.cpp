#include "main.h"

void setup()
{
  // LED SETUP
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, LOW);

  while (!Serial)
    ;
  delay(3000);
  Serial.begin(9600);

  if (!rtc.begin())
  {
    Serial.println("Failed to start RTC module! Please check your wiring.");
    while (1)
      ;
  }
  rtc.disable32K();
  Serial.println(rtc.lostPower());

  dht.begin();
  if (!ccs.begin())
  {
    Serial.println("Failed to start CCS module! Please check your wiring.");
    while (1)
      ;
  }
}

uint32_t last_poll = 0;
uint32_t current_time = 0;

// current values
float current_temperature = 0;
float current_humidity = 0;
uint16_t current_co2 = 0;
uint16_t current_tvoc = 0;

void loop()
{
  if (pollScheduled(&current_time))
  {
    // update values
    readCCS811(&current_co2, &current_tvoc);
    readDHT(&current_temperature, &current_humidity);
    
    char ret[60];
    sprintf(ret, "%d, %lf, %lf, %d, %d\n", current_time, current_temperature, current_humidity, current_co2, current_tvoc);
    Serial.print(ret);
  }
}

bool pollScheduled(uint32_t *now)
{
  *now = rtc.now().unixtime();
  uint32_t delta = *now - last_poll;

  if (last_poll == 0 && delta > initial_poll)
  {
    last_poll = *now;
    return true;
  }
  if (delta > poll_interval)
  {
    last_poll = *now;
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