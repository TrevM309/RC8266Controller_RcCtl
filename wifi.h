/* RC Controller WiFi related code */

typedef unsigned char U8;

void WifiInit(void);
void WifiProcess(void);
void WifiSend(int8_t *perc, U8 Num);
long WifiDb(void);
uint16_t DevVolts(void);
