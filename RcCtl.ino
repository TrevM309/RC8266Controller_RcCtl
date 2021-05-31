// TrevM 14/05/2021
// Main file for ESP-M2 Remote Control Transmitter

#ifdef __ADS
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#endif //__ADS
#include "debug.h"
#include "lcd.h"

#ifdef __ADS
Adafruit_ADS1015 ads;

// adc values
int16_t h_min = 0;
int16_t h_mid = 0x193;
int16_t h_max = 0x456;
//
int16_t v_min = 0;
int16_t v_mid = 0x17f;
int16_t v_max = 0x455;
#else //__ADS
int Count = 0;
#endif //__ADS

void setup() 
{
  dbgInit();
#ifdef __ADS
  Wire.begin(5,4);
  ads.begin();
#endif
  Lcd_Init();
  LCD_Clear(LCD_BLUE);
  BACK_COLOR=LCD_BLUE;
  delay(1000);
  // auto calibrate centre
#ifdef __ADS
  h_mid = ads.readADC_SingleEnded(0);
  v_mid = ads.readADC_SingleEnded(1);
#endif
}

unsigned long tlast = 0;

void loop() 
{
  unsigned long tnow;
#ifdef __ADS
  int16_t adc0=0,adc1=0,adc2=0;
  float bat;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  bat = ads.computeVolts(adc2) * 2.0;
  if (adc0 < h_min)
  {
    h_min = adc0;
  }
  if (adc0 > h_max)
  {
    h_max = adc0;
  }
  if (adc1 < v_min)
  {
    v_min = adc1;
  }
  if (adc1 > v_max)
  {
    v_max = adc1;
  }
#endif
  tnow = millis();
  if ((tnow - tlast) > 1000)
  {
#ifdef __ADS
    int8_t h_perc = 0;
    int8_t v_perc = 0;
    
    tlast = tnow;
    //dbgPrintf("H:%x V:%x Bat:%0.3fV\n", adc0, adc1, bat);
    LCD_Printf(0, 0, LCD_WHITE,1, "H:%x V:%x   ", adc0, adc1);
    
    // turn adc values into percentage for H & V
    if (adc0 < h_mid)
    {
      h_perc = (adc0 - h_min) * 100 / (h_mid - h_min); 
      h_perc = -100 + h_perc;
    }
    else
    {
      h_perc = (adc0 - h_mid) * 100 / (h_max - h_mid); 
    }
    if (adc1 < v_mid)
    {
      v_perc = (adc1 - v_min) * 100 / (v_mid - v_min); 
      v_perc = -100 + v_perc;
    }
    else
    {
      v_perc = (adc1 - v_mid) * 100 / (v_max - v_mid); 
    }
    dbgPrintf("H:%3x(%+3d) V:%3x(%+3d) Bat:%0.3fV\n", adc0, h_perc, adc1, v_perc, bat);
    LCD_Printf(0, 16, LCD_WHITE,1, "H:%3d V:%3d   ", h_perc, v_perc);
    
    LCD_Printf(0, 32, LCD_WHITE,1, "Bat:%0.3fV  ", bat);
#else //__ADS
    tlast = tnow;
    dbgPrintf("Count %d\n", Count);
    LCD_Printf(0, 16, LCD_WHITE,1, "Count %d ", Count);
    Count++;
    if (Count > 50)
    {
      Count = 0;
    }
    
#endif //__ADS
  }
}
