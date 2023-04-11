#include "main.h"

DHT dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 ccs;
RTC_DS3231 rtc;

unsigned long last_dht_read = 0;
unsigned long last_ccs811_read = 0;
unsigned long last_rtc_read = 0;

void setup()
{
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
  Serial.println(F("DHTxx test!"));

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

void loop()
{
  readDHT();
  readCCS811();
  printTime();
}

void readDHT()
{
  if (millis() - last_dht_read < DHT_INTERVAL)
    return;
  last_dht_read = millis();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
}

void readCCS811()
{
  if (millis() - last_ccs811_read < CCS811_INTERVAL)
    return;
  last_ccs811_read = millis();

  if (ccs.available())
  {
    if (!ccs.readData())
    {
      uint16_t co2 = ccs.geteCO2();
      uint16_t tvoc = ccs.getTVOC();
      char ret[40];
      sprintf(ret, "CO2: %4dppm, TVOC: %4dppm", co2, tvoc);
      Serial.println(ret);
    }
    else
    {
      Serial.println("ERROR!");
      while (1)
        ;
    }
  }
}

void printTime()
{
  if (millis() - last_rtc_read < RTC_INTERVAL)
    return;
  last_rtc_read = millis();

  DateTime now = rtc.now();
  
  // Day of the week
  Serial.print("Day of the week: ");
  Serial.print(now.dayOfTheWeek());
  Serial.println();

  Serial.print("Current Time: ");
  if (now.hour() < 10)
  {
    Serial.print("0");
    Serial.print(now.hour());
  }
  else
  {
    Serial.print(now.hour(), DEC);
  }
  Serial.print(':');
  if (now.minute() < 10)
  {
    Serial.print("0");
    Serial.print(now.minute());
  }
  else
  {
    Serial.print(now.minute(), DEC);
  }
  Serial.print(':');
  if (now.second() < 10)
  {
    Serial.print("0");
    Serial.print(now.second());
  }
  else
  {
    Serial.print(now.second(), DEC);
  }
  Serial.println();
}