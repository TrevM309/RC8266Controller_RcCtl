// TrevM 14/05/2021
// Main file for ESP-M2 Remote Control Transmitter

#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include "debug.h"

Adafruit_ADS1015 ads;

void setup() 
{
  dbgInit();
  Wire.begin(5,4);
  ads.begin();
  delay(1000);
}

void loop() 
{
  int16_t adc0,adc1,adc2;
  float bat;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  bat = ads.computeVolts(adc2) * 2.0;

  dbgPrintf("Horiz: %x Vert: %x Bat: %0.3fV\n", adc0, adc1, bat);
  delay(1000);
}
