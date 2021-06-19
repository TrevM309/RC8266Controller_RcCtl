// TrevM 14/05/2021
// Main file for ESP-M2 Remote Control Transmitter

#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include "debug.h"
#include "lcd.h"
#include "wifi.h"

Adafruit_ADS1015 ads;

enum
{
  HORIZ = 0,
  VERT,
};

// adc limit values
int16_t Amin[2] = { 10, 10 };
int16_t Amid[2] = { 0x193, 0x17f };
int16_t Amax[2] = { 0x456-10, 0x455-10 };
int8_t perc[2] = { 0, 0 };
float bat = 0.0;

unsigned long tlast = 0;
int16_t adc[3] = { 0, 0, 0 };
int16_t ladc[3] = { 0, 0, 0 };

// local funcs
void sendADCs();
void readADCs();
void showADCs();

void setup() 
{
  dbgInit();
  Wire.begin(5,4);
  ads.begin();
  Lcd_Init();
  LCD_Clear(LCD_BLUE);
  BACK_COLOR=LCD_BLUE;
  delay(100);
  WifiInit();
  delay(1000);
  // auto calibrate centre
  Amid[0] = ads.readADC_SingleEnded(0);
  Amid[1] = ads.readADC_SingleEnded(1);
}

void readADCs()
{
  int x = 0;

  for(x = 0; x < 3; x++)
  {
    adc[x] = ads.readADC_SingleEnded(x);
  }
  if (adc[0] < Amin[0])
  {
    Amin[0] = adc[0];
  }
  if (adc[0] > Amax[0])
  {
    Amax[0] = adc[0];
  }
  if (adc[1] < Amin[1])
  {
    Amin[1] = adc[1];
  }
  if (adc[1] > Amax[1])
  {
    Amax[1] = adc[1];
  }
}

void sendADCs()
{
  if ((adc[0] != ladc[0]) || (adc[1] != ladc[1]) || (adc[2] != ladc[2]))
  {
    int x = 0;

    for(x = 0; x < 3; x++)
    {
      ladc[x] = adc[x];
    }
    bat = ads.computeVolts(adc[2]) * 2.0;
    
    // turn adc values into percentage for H & V
    if (adc[0] < Amid[0])
    {
      perc[0] = (adc[0] - Amin[0]) * 100 / (Amid[0] - Amin[0]); 
      perc[0] = 100 - perc[0];
    }
    else
    {
      perc[0] = (adc[0] - Amid[0]) * -100 / (Amax[0] - Amid[0]); 
    }
    if (adc[1] < Amid[1])
    {
      perc[1] = (adc[1] - Amin[1]) * 100 / (Amid[1] - Amin[1]); 
      perc[1] = 100 - perc[1];
    }
    else
    {
      perc[1] = (adc[1] - Amid[1]) * -100 / (Amax[1] - Amid[1]); 
    }
    WifiSend(perc[HORIZ], perc[VERT]);
  }
}

void showADCs()
{
  LCD_Printf(0, 0, LCD_WHITE,1, "H:%x V:%x   ", adc[0], adc[1]);
  dbgPrintf("H:%3x(%+4d) V:%3x(%+4d) Bat:%0.3fV\n", adc[0], perc[0], adc[1], perc[1], bat);
  LCD_Printf(0, 16, LCD_WHITE,1, "H:%3d V:%3d   ", perc[0], perc[1]);
  LCD_Printf(0, 32, LCD_WHITE,1, "Bat:%0.3fV  ", bat);
}

void loop() 
{
  unsigned long tnow;

  WifiProcess();
  readADCs();
  sendADCs();
  tnow = millis();
  if ((tnow - tlast) > 1000)
  {
    tlast = tnow;
    showADCs();
  }
}
