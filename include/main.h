#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_CCS811.h>

#define LED_G D0
#define LED_Y D3
#define LED_R D8

#define DHTPIN D4
#define DHTTYPE DHT11

#define DHT_INTERVAL 2500
#define CCS811_INTERVAL 1000
#define RTC_INTERVAL 2000

void readDHT();
void readCCS811();
void printTime();