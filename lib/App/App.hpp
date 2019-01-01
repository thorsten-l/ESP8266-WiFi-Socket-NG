#ifndef __APP_H__
#define __APP_H__

#include <Arduino.h>

#define LOG0( format ) Serial.printf( "(%ld) " format, millis())
#define LOG1( format, x) Serial.printf( "(%ld) " format, millis(), x )

#define APP_NAME "WiFi Socket NG"
#define APP_VERSION "1.2.0dev3"
#define APP_AUTHOR "Dr. Thorsten Ludewig <t.ludewig@gmail.com>"
#define APP_CONFIG_FILE "/config.bin"

#define WIFI_LED 4
#define POWER_BUTTON 14
#define RELAY_TRIGGER_OFF 5
#define RELAY_TRIGGER_ON 12

typedef struct appconfig
{
  char wifi_ssid[64];
  char wifi_password[64];
  int  wifi_mode;

  char ota_hostname[64];
  char ota_password[64];

  char admin_password[64];

  bool ohab_enabled;
  int  ohab_version;
  char ohab_host[64];
  int  ohab_port;
  bool ohab_useauth;
  char ohab_user[64];
  char ohab_password[64];
  char ohab_itemname[64];

  bool alexa_enabled;
  char alexa_devicename[64];

  bool mqtt_enabled;
  char mqtt_clientid[64];
  char mqtt_host[64];
  int  mqtt_port;
  bool mqtt_useauth;
  char mqtt_user[64];
  char mqtt_password[64];
  char mqtt_intopic[64];
  char mqtt_outtopic[64];

} AppConfig;

class App
{
private:
  bool initialized = false;
  bool doSystemRestart;
  time_t systemRestartTimestamp;

  void loadConfig();
  void restartSystem();

public:
  App();

  void setup();
  void writeConfig();
  void printConfig();
  void delayedSystemRestart();
  void handle();
};

extern App app;
extern AppConfig appcfg;

#endif
