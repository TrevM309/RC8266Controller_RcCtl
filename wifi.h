/* RC Controller WiFi related code */

typedef unsigned char U8;

void WifiInit(void);
void WifiProcess(void);
void WifiSend(U8 h_perc, U8 v_perc);
uint16_t DevVolts(void);
