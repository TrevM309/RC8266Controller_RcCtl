// TrevM 14/05/2021
// Main file for ESP-M2 Remote Control Transmitter

#define __ADS

#ifdef __ADS
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#endif //__ADS
#include "debug.h"
#include "lcd.h"
#include "wifi.h"

#ifdef __ADS
Adafruit_ADS1015 ads;

// adc values
int16_t h_min = 10;
int16_t h_mid = 0x193;
int16_t h_max = 0x456-10;
//
int16_t v_min = 10;
int16_t v_mid = 0x17f;
int16_t v_max = 0x455-10;
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
  delay(100);
  WifiInit();
  delay(1000);
  // auto calibrate centre
#ifdef __ADS
  h_mid = ads.readADC_SingleEnded(0);
  v_mid = ads.readADC_SingleEnded(1);
#endif
}

unsigned long tlast = 0;
int16_t adc[3] = { 0, 0, 0 };
int16_t ladc[3] = { 0, 0, 0 };

void readADCs()
{
#ifdef __ADS
  int x = 0;

  for(x = 0; x < 3; x++)
  {
    adc[x] = ads.readADC_SingleEnded(x);
  }
  if (adc[0] < h_min)
  {
    h_min = adc[0];
  }
  if (adc[0] > h_max)
  {
    h_max = adc[0];
  }
  if (adc[1] < v_min)
  {
    v_min = adc[1];
  }
  if (adc[1] > v_max)
  {
    v_max = adc[1];
  }
#endif
}

void showADCs()
{
  if ((adc[0] != ladc[0]) || (adc[1] != ladc[1]) || (adc[2] != ladc[2]))
  {
    int8_t h_perc = 0;
    int8_t v_perc = 0;
    float bat;
    int x = 0;

    for(x = 0; x < 3; x++)
    {
      ladc[x] = adc[x];
    }
    bat = ads.computeVolts(adc[2]) * 2.0;
    LCD_Printf(0, 0, LCD_WHITE,1, "H:%x V:%x   ", adc[0], adc[1]);
    
    // turn adc values into percentage for H & V
    if (adc[0] < h_mid)
    {
      h_perc = (adc[0] - h_min) * 100 / (h_mid - h_min); 
      h_perc = 100 - h_perc;
    }
    else
    {
      h_perc = (adc[0] - h_mid) * -100 / (h_max - h_mid); 
    }
    if (adc[1] < v_mid)
    {
      v_perc = (adc[1] - v_min) * 100 / (v_mid - v_min); 
      v_perc = 100 - v_perc;
    }
    else
    {
      v_perc = (adc[1] - v_mid) * -100 / (v_max - v_mid); 
    }
    dbgPrintf("H:%3x(%+4d) V:%3x(%+4d) Bat:%0.3fV\n", adc[0], h_perc, adc[1], v_perc, bat);
    LCD_Printf(0, 16, LCD_WHITE,1, "H:%3d V:%3d   ", h_perc, v_perc);
      
    LCD_Printf(0, 32, LCD_WHITE,1, "Bat:%0.3fV  ", bat);
  }
}

void loop() 
{
  unsigned long tnow;

  WifiProcess();
  readADCs();
  tnow = millis();
  if ((tnow - tlast) > 1000)
  {
    tlast = tnow;
#ifdef __ADS
    showADCs();
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
