/*
** wifiManager.cpp
** 05 août 2026
** TRAN Olivier
*/


#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

class WifiManager
{
public:
    void begin();
    void loop();
    bool isConnected();
    String getIP();
    String getSSID();

private:
    Preferences prefs;
    WebServer server{80};
    DNSServer dns;

    bool configMode = false;
    void connectStored();
    void startPortal();
    void setupRoutes();
    void saveCredentials(String ssid, String password);
};

#endif