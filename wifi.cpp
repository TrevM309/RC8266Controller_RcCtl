#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "debug.h"
#include "wifi.h"

const char *ssid     = "RcCtrl1";
const char *password = "password1234567";

// UDP
WiFiUDP UDP;
IPAddress server(192,168,4,15);     // IP address of the AP
#define UDP_PORT 4210

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
  UDP.begin(UDP_PORT);
  Serial.print("Opening UDP port ");
  Serial.println(UDP_PORT);
}

void WifiSend(U8 h_perc, U8 v_perc)
{
  // Send Packet
  UDP.beginPacket(server, UDP_PORT);
  UDP.write(h_perc);
  UDP.write(v_perc);
  UDP.endPacket();
  
  /*
  client.connect(server, 80);
  Serial.println("********************************");
  Serial.print("Byte sent to the AP: ");
  Serial.println(client.print("Anyo\r"));
  String answer = client.readStringUntil('\r');
  Serial.println("From the AP: " + answer);
  client.flush();
  client.stop();
  */
}


void WifiProcess()
{
}
