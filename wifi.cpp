#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "debug.h"
#include "wifi.h"

#define UDP_TX_PORT 4210
#define UDP_RX_PORT 4211
const char *ssid     = "RcCtrl3";
const char *password = "password1234567";

// UDP
WiFiUDP UDPTX;
WiFiUDP UDPRX;
IPAddress server(192,168,4,15);     // IP address of the AP
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

uint16_t Vbat = 0;

void WifiInit(void)
{
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Connected");
  Serial.print("LocalIP:"); 
  Serial.println(WiFi.localIP());
  Serial.println("MAC:" + WiFi.macAddress());
  Serial.print("Gateway:"); 
  Serial.println(WiFi.gatewayIP());
  Serial.print("AP MAC:"); 
  Serial.println(WiFi.BSSIDstr());

  // Begin UDP port
  UDPTX.begin(UDP_TX_PORT);
  Serial.print("Opening UDP port ");
  Serial.println(UDP_TX_PORT);
  // Begin UDP port
  UDPRX.begin(UDP_RX_PORT);
  Serial.print("Opening UDP port ");
  Serial.println(UDP_RX_PORT);
}

void WifiSend(U8 h_perc, U8 v_perc)
{
  // Send Packet
  UDPTX.beginPacket(server, UDP_TX_PORT);
  UDPTX.write(-h_perc);
  UDPTX.write(v_perc);
  UDPTX.endPacket();
}

uint16_t DevVolts(void)
{
  return Vbat;
}

long WifiDb(void)
{
  return WiFi.RSSI();
}

void WifiProcess()
{
  int len;

  // Receive packet
  len = UDPRX.parsePacket();
  if (len > 0)
  {
    UDPRX.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Vbat = (packetBuffer[0] << 8) | packetBuffer[1];
    dbgPrintf("Device Vbat:%d\n",Vbat);
  }
}
