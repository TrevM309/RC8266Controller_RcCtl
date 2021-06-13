#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "debug.h"
#include "wifi.h"

#define APSSID "RcCtrl1"
#define APPSK  "thereisnospoon"
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

// Local functions
void handleRoot() ;

void WifiInit(void)
{
  dbgPrintf("\nConfiguring access point...\n");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  dbgPrintf("AP IP address: %d.%d.%d.%d\n",myIP[0],myIP[1],myIP[2],myIP[3]);
  //Serial.println(myIP);  
  server.on("/", handleRoot);
  server.begin();
  dbgPrintf("HTTP server started\n");
}

void WifiProcess()
{
  server.handleClient();
}

void handleRoot() 
{
  dbgPrintf("Root Accessed\n");
  server.send(200, "text/html", "<h1>You are connected</h1>");
}
