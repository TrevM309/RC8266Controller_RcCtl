// debug.h
// TrevM 08/06/2018
// for debugging, sends degug messages over UART1
// at 921,600 baud, 8 data bits, no parity, 1 stop bit

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define TRACE(a) Serial.printf a
#define TRACED(a) Serial.print a
// requires double brackets on TRACE
// e.g. TRACE(("\nHello World"));

void dbgInit(void);
void dbgPrintf(char* fmt, ...);
void dbgNL(void);
void dbgHex(u8 val);
void dbgHex32(u32 val);

#endif //__DEBUG_H__
