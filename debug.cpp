// debug.cpp
// TrevM 08/06/2018

#include "Arduino.h"
#include "debug.h"

void dbgInit(void)
{
  Serial.begin(115200,SERIAL_8N1);
  Serial.setDebugOutput(true);
  delay(100);
  TRACE(("\n\nTrace Started\n"));
}

void dbgPrintf(char* fmt, ...)
{
  va_list lst;
  char buf[256];
  int x;
  int len;

  va_start(lst, fmt);
  len = vsprintf(buf,fmt,lst);
  if (len > 0)
  {
    for (x = 0; x < len; x++)
    {
      if (buf[x] == '\n')
        Serial.println("");
      else
        Serial.print(buf[x]);
    }
  }
}

void dbgNL(void)
{
  Serial.write('\n');
}

void dbgHNib(u8 val)
{
  char chr;
  if (val >= 10)
    chr = (val-10)+'a';
  else
    chr = '0'+val;
  Serial.write(chr);
}

void dbgHex(u8 val)
{
  dbgHNib(val >> 4);  
  dbgHNib(val & 0xf);  
  Serial.write(',');
}

void dbgHex32(u32 val)
{
  int x;

  Serial1.write("0x");
  for(x = 0; x < 8; x++)
  {
    dbgHNib((val >> 28) & 0xf);
    val <<= 4;
  }
  Serial.write(',');
}
