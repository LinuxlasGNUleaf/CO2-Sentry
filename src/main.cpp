#include "main.h"

DHT dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 ccs;

unsigned long last_dht_read = 0;
unsigned long last_ccs811_read = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  
  dht.begin();

  if (!ccs.begin())
  {
    Serial.println("Failed to start sensor! Please check your wiring.");
    while (1)
      ;
  }
}

void loop() {
  readDHT();
  readCCS811();
}

void readDHT(){
  if (millis() - last_dht_read < DHT_INTERVAL)
    return;
  last_dht_read = millis();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
}

void readCCS811(){
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
      while (1);
    }
  }
}