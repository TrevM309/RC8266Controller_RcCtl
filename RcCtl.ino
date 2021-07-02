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

#define NUMCH   2
#define NUMADC  3
#define NUMAVG  20
#define MINDIF  5

unsigned long tlast  = 0;               // last time values shown (millis)
int16_t adc[NUMADC]  = { 0, 0, 0 };     // average adc values
int32_t SumAdc[NUMADC] = { 0, 0, 0 };   // average sums
int8_t NumAdc = 0;                      // how many readings in sum
int16_t ladc[NUMADC] = { 0, 0, 0 };     // last adc value sent
int16_t dadc[NUMADC] = { 0, 0, 0 };     // last adc value displayed

// adc limit values
int16_t Amin[NUMCH] = { 10, 10 };
int16_t Amid[NUMCH] = { 0x193, 0x17f };
int16_t Amax[NUMCH] = { 0x456-10, 0x455-10 };
int8_t perc[NUMCH]  = { 0, 0 };
int8_t lperc[NUMCH] = { 100, 100 };
float bat           = 0.0;

// local funcs
void sendADCs();
void readADCs();
void showADCs();

// standard Arduino setup (initialise everything at power up)
void setup() 
{
  int8_t x;
  
  dbgInit();
  Wire.begin(5,4);
  ads.begin();
  Lcd_Init();
  LCD_Clear(LCD_BLUE);
  BACK_COLOR=LCD_BLUE;
  LCD_Printf(20, 16, LCD_WHITE, 2, "Moseley");
  //delay(100);
  WifiInit();
  delay(1000);
  LCD_Clear(LCD_BLUE);
  // auto calibrate centre
  for (x = 0; x < NUMCH; x++)
  {
    Amid[x] = ads.readADC_SingleEnded(x);
  }
}

// standard Arduino loop (Run continuously after setup)
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

// read ADCs and auto adjust min & max
void readADCs()
{
  int8_t x = 0;

  // add adcs to sums
  for(x = 0; x < NUMADC; x++)
  {
    SumAdc[x] += ads.readADC_SingleEnded(x);
  }
  NumAdc++;
  if (NumAdc >= NUMAVG)
  {
    NumAdc = 0;
    for(x = 0; x < NUMADC; x++)
    {
      adc[x] = SumAdc[x] / NUMAVG;
      SumAdc[x] = 0;
    }
    for (x = 0; x < NUMCH; x++)
    {
      if (adc[x] < Amin[x])
      {
        Amin[x] = adc[x];
      }
      if (adc[x] > Amax[x])
      {
        Amax[x] = adc[x];
      }
    }
  }
}

// calculate percentages and send to device
void sendADCs()
{
  int8_t x = 0;
  int8_t v = 0;
  uint16_t diff[3];

  for (x = 0; x < NUMADC; x++)
  {
    if (adc[x] > ladc[x])
      diff[x] = adc[x] - ladc[x];
    else
      diff[x] = ladc[x] - adc[x];
    if (diff[x] < MINDIF)
      diff[x] = 0;
    if (diff[x] > 0)
      v = 1;
  }
  if (v != 0)
  {
    int8_t x = 0;

    for (x = 0; x < NUMADC; x++)
    {
      ladc[x] = adc[x];
    }

    // calc battery voltage
    bat = ads.computeVolts(adc[2]) * 2.0;
    
    // turn adc values into percentage for H & V
    for (x = 0; x < NUMCH; x++)
    {
      if (adc[x] < Amid[x])
      {
        perc[x] = (adc[x] - Amin[x]) * 100 / (Amid[x] - Amin[x]); 
        perc[x] = 100 - perc[x];
      }
      else
      {
        perc[x] = (adc[x] - Amid[x]) * -100 / (Amax[x] - Amid[x]); 
      }
    }
    perc[0] = -perc[0];  // invert horiz
    if ((perc[HORIZ] != lperc[HORIZ]) || (perc[VERT] != lperc[VERT]))
    {
      lperc[HORIZ] = perc[HORIZ];
      lperc[VERT]  = perc[VERT];
      WifiSend(perc[HORIZ], perc[VERT]);
    }
  }
}

// Display ADCs
void showADCs()
{
  float dV = (float)DevVolts() / 100.0;
  dbgPrintf("H:%3x(%+4d) V:%3x(%+4d) Bat:%0.3fV\n", adc[0], perc[0], adc[1], perc[1], bat);
  LCD_Printf(0, 16, LCD_WHITE,1, "H:%3d V:%3d   ", perc[0], perc[1]);
  LCD_Printf(0, 32, LCD_WHITE,1, "Bat:%0.2fV  ", bat);
  LCD_Printf(0, 48, LCD_WHITE,1, "Dev:%0.2fV  ", dV);
}
