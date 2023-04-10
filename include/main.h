#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_CCS811.h>

#define DHTPIN 2
#define DHTTYPE DHT22

#define DHT_INTERVAL 2500
#define CCS811_INTERVAL 1000

void readDHT();
void readCCS811();