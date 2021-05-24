// TrevM 14/05/2021
// Main file for ESP-M2 Remote Control Transmitter

#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include "debug.h"
#include "lcd.h"

Adafruit_ADS1015 ads;

void setup() 
{
  dbgInit();
  pinMode(15,OUTPUT);
  pinMode(4,OUTPUT);
  Wire.begin(5,4);
  ads.begin();
  Lcd_Init();
  LCD_Clear(LCD_BLUE);
  BACK_COLOR=LCD_BLUE;
  delay(1000);
  //LCD_Printf(5,0,LCD_WHITE,3,"Hello");
}

int count = 0;
unsigned long tlast = 0;

void loop() 
{
  unsigned long tnow;
  int16_t adc0=0,adc1=0,adc2=0;
  float bat;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  bat = ads.computeVolts(adc2) * 2.0;

  tnow = millis();
  if ((tnow - tlast) > 500)
  {
    tlast = tnow;
    LCD_Printf(0, 0, LCD_WHITE,1, "count %d  ",count);
    dbgPrintf("H:%x V:%x Bat:%0.3fV\n", adc0, adc1, bat);
    LCD_Printf(0, 16, LCD_WHITE,1, "H:%x V:%x   ", adc0, adc1);
    LCD_Printf(0, 32, LCD_WHITE,1, "Bat:%0.3fV  ", bat);
    count++;
    if (count > 50)
    {
      count = 0;
    }
  }
}
