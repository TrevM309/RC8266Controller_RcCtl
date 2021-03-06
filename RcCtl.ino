// TrevM 14/05/2021
// Main file for ESP-M2 Remote Control Transmitter

#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include "common.h"
#include "debug.h"
#include "lcd.h"
#include "wifi.h"

enum
{
  HORIZ = 0,
  VERT,
};

#ifdef DUAL

#define NUMCH   4
#define NUMADC  5

#else // DUAL

#define NUMCH   2
#define NUMADC  3

#endif //DUAL

#define NUMAVG  20
#define MINDIF  5

#define LIPOMAX   4.2
#define LIPOSTORE 3.8
#define LIPOMIN   3.6

#define SIGLEN 40
#define SIGANG 28

#define MINADC 20
#define MAXADC (0x456 - 40)

unsigned long tlast = 0; // last time values shown (millis)
// 0=RH 1=RV 2=LH 3=LV
int16_t adc[NUMADC];     // average adc values
int32_t SumAdc[NUMADC];  // average sums
int8_t NumAdc = 0;       // how many readings in sum
int16_t ladc[NUMADC];    // last adc value sent
int16_t dadc[NUMADC];    // last adc value displayed

// adc limit values
int16_t Amin[NUMCH];
int16_t Amid[NUMCH];
int16_t Amax[NUMCH];
uint8_t seq = 0;
int8_t perc[NUMCH];
int8_t lperc[NUMCH];
float bat = 0.0;

Adafruit_ADS1015 ads;

// local funcs
void sendADCs();
void readADCs();
void showADCs();
void showBat(uint8_t x, uint8_t y, float Vmax, float Vstore, float Vmin, float Vnow);
void showSig(uint8_t x, uint8_t y, long rssi);

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
  // initialise everything
  for (x = 0; x < NUMADC; x++)
  {
    adc[x] = 0;
    SumAdc[x] = 0;
    ladc[x] = 0;
    dadc[x] = 0;
  }
  for (x = 0; x < NUMCH; x++)
  {
    // auto calibrate centre
    Amid[x] = ads.readADC_SingleEnded(x);
    Amin[x] = MINADC;
    Amax[x] = MAXADC;
    perc[x] = 0;
    lperc[x] = 100;
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
    if (x < 4)
    {
      // ADS1015
      SumAdc[x] += ads.readADC_SingleEnded(x);
    }
    else
    {
      SumAdc[x] += analogRead(A0);
    }
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
    int8_t tx = 0;

    for (x = 0; x < NUMADC; x++)
    {
      ladc[x] = adc[x];
    }

    // calc battery voltage
#ifdef DUAL
    bat = (1100 * adc[4] / 1023) / 100.0;
#else //DUAL
    bat = ads.computeVolts(adc[2]) * 2.0;
#endif //DUAL
    
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
    // inversions
    perc[0] = -perc[0];  // invert horiz
    for (x = 0; x < NUMCH; x++)
    {
      if (perc[x] != lperc[x])
      {
        tx++;
      }
    }
    if (tx != 0)
    {
      for (x = 0; x < NUMCH; x++)
      {
        lperc[x] = perc[x];
      }
      WifiSend(perc, NUMCH);
    }
  }
}

// Display ADCs
void showADCs()
{
  float dV = (float)DevVolts() / 100.0;
  long rssi = WifiDb();
  U8 x;

  for (x = 0; x < NUMCH; x++)
  {
    dbgPrintf("%d:%3x(%+4d) ",x,adc[x],perc[x]);
  }
  dbgPrintf("Bat:%0.2fV Dev:%0.2fV RSSI:%ddBm\n", bat, dV,rssi);
  switch(seq)
  {
    case 0:
      showSig(112,10,rssi);
      break;
    case 1:
      showBat(0,2,(LIPOMAX*2),(LIPOSTORE*2),(LIPOMIN*2),dV);
      break;
    case 2:
      showBat(0,42,LIPOMAX,LIPOSTORE,LIPOMIN,bat);
      break;
    default:
      break;
  }
  seq++;
  if (seq > 2)
  {
    seq = 0;
  }
}

void showBat(uint8_t x, uint8_t y, float Vmax, float Vstore, float Vmin, float Vnow)
{
  uint32_t pos;
  uint32_t vmx = (uint32_t)(Vmax * 100.0);
  uint32_t vst = (uint32_t)(Vstore * 100.0);
  uint32_t vmn = (uint32_t)(Vmin * 100.0);
  uint32_t vnw = (uint32_t)(Vnow * 100.0);

  if (vnw > vmx)
    vnw = vmx;
  // draw symbol
  LCD_DrawLine(x+5,y,x+100+5,y,LCD_WHITE);            // top line
  LCD_DrawLine(x+5,y+32,x+100+5,y+32,LCD_WHITE);      // bottom line
  LCD_DrawLine(x+100+5,y,x+100+5,y+32,LCD_WHITE);     // right line
  LCD_DrawLine(x+5,y,x+5,y+32,LCD_WHITE);             // left line
  LCD_DrawRectangle(x,y+16-5,x+5,y+16+5,LCD_WHITE);   // left pip
  // draw level
  pos = (vnw - vmn) * 100 / (vmx - vmn);
  //dbgPrintf("%d %d %d %d = %d\n",vmx,vst,vmn,vnw,pos);
  if (pos <= 100)
  {
    u16 col = LCD_RED;
    if (vnw >= (vmn+10))
    {
      col = LCD_YELLOW;
    }
    if (vnw > vst)
    {
      col = LCD_GREEN;
    }
    LCD_Fill(x+5+100-pos,y+1,x+5+99,y+31,col);
    LCD_Fill(x+6,y+1,x+6+100-pos,y+31,LCD_BLUE);
  }
  // Add Voltage
  //void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color,u8 size);
  LCD_ShowChar(x+40,y+8,'0' + (vnw / 100),1,LCD_BLACK,1);
  LCD_ShowChar(x+40+8,y+8,'v',1,LCD_BLACK,1);
  LCD_ShowChar(x+40+16,y+8,'0' + ((vnw % 100)/10),1,LCD_BLACK,1);
  
}

void showSig(uint8_t x, uint8_t y, long rssi)
{
  float rads;
  uint8_t sx = x + (SIGLEN/2);
  uint8_t sy = y + SIGLEN;
  uint8_t x1,y1,x2,y2;
  uint8_t slen;
  uint8_t a;

  // mind limits
  slen = 1;
  if (rssi >= -90)
    slen = SIGLEN / 4;
  if (rssi >= -80)
    slen = SIGLEN / 2;
  if (rssi >= -70)
    slen = SIGLEN * 3 / 4;
  if (rssi >= -30)
    slen = SIGLEN;
  // draw val
  for (a = 1; a < SIGANG; a++)
  {
    rads = a * 0.01745329252;
    x1 = (uint8_t)(sin(rads) * (float)(SIGLEN-1));
    y1 = (uint8_t)(cos(rads) * (float)(SIGLEN-1));
    x2 = (uint8_t)(sin(rads) * (float)slen);
    y2 = (uint8_t)(cos(rads) * (float)slen);
    LCD_DrawLine(sx-x1,sy-y1,sx,sy,LCD_BLUE);
    LCD_DrawLine(sx+x1,sy-y1,sx,sy,LCD_BLUE);
    LCD_DrawLine(sx-x2,sy-y2,sx,sy,LCD_WHITE);
    LCD_DrawLine(sx+x2,sy-y2,sx,sy,LCD_WHITE);
  }
  // draw symbol
  rads = SIGANG * 0.01745329252;
  x1 = (uint8_t)(sin(rads) * (float)SIGLEN);
  y1 = (uint8_t)(cos(rads) * (float)SIGLEN);
  //dbgPrintf("sx:%d sy:%d x1:%d y1:%d\n",sx,sy,x1,y1);
  // draw left & right axis
  LCD_DrawLine(sx-x1,sy-y1,sx,sy,LCD_WHITE);
  LCD_DrawLine(sx+x1,sy-y1,sx,sy,LCD_WHITE);
  LCD_DrawPoint(sx,y,LCD_WHITE);
  // draw arc
  for (a = 1; a < SIGANG; a++)
  {
    rads = a * 0.01745329252;
    x1 = (uint8_t)(sin(rads) * (float)SIGLEN);
    y1 = (uint8_t)(cos(rads) * (float)SIGLEN);
    LCD_DrawPoint(sx-x1,sy-y1,LCD_WHITE);
    LCD_DrawPoint(sx+x1,sy-y1,LCD_WHITE);
  }
  LCD_Printf(x+8,y+SIGLEN+5,LCD_WHITE,1,"%ld ",rssi);
}
