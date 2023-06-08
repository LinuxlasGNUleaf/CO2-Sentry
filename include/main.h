#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_CCS811.h>

// HARDWARE CONFIG
#define LED_G D0
#define LED_Y D3
#define LED_R D8

#define DHTPIN D4
#define DHTTYPE DHT22

// OBJECTS
DHT dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 ccs;
RTC_DS3231 rtc;

// SOFTWARE CONFIG

// times in seconds
unsigned long initial_poll = 10;
unsigned long poll_interval = 5;

// METHOD DECLARATIONS
void readDHT(float *temperature, float *humidity);
void readCCS811(uint16_t *co2, uint16_t *tvoc);
bool pollScheduled(uint32_t *now);

#endif