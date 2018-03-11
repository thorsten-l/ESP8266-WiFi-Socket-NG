#include <App.hpp>
#include <DefaultAppConfig.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WifiHandler.hpp>
#include <RelayHandler.hpp>
#include "WebHandler.hpp"

WebHandler webHandler;

static AsyncWebServer server(80);

String jsonStatus()
{
  String message = "{\"state\":";
  message += ( relayHandler.isPowerOn() ) ? "1}\r\n" : "0}\r\n";
  return message;
}

void handlePageNotFound( AsyncWebServerRequest *request )
{
  request->send(404);
}

void handleSetupPage( AsyncWebServerRequest *request )
{
  if(!request->authenticate("admin", appcfg.admin_password))
  {
    return request->requestAuthentication();
  }

  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->printf( "<html><body><h1>" APP_NAME " (%s) - Setup</h1>", appcfg.ota_hostname );
  response->println("<h4>Version " APP_VERSION ", by " APP_AUTHOR " </h4>");

  response->println("<form action='/savecfg' method=POST>");

  // Security
  response->println("<h2>Security</h2>");
  response->printf("<p>Admin Password <input type='text' name='admin_password' size='34' maxlength='30' value='%s'></p>\n", appcfg.admin_password );

  // WiFi
  response->println("<h2>WiFi Network Scan</h2>");
  response->printf("<pre>%s</pre>\n", wifiHandler.getScannedNetworks() );

  response->println("<h2>WiFi</h2>");
  response->println("<p>Mode <select name='wifi_mode'>");
  response->printf("<option %s value='%d'>Access Point</option>\n", (appcfg.wifi_mode == WIFI_AP) ? "selected" : "", WIFI_AP );
  response->printf("<option %s value='%d'>Station</option>\n", (appcfg.wifi_mode == WIFI_STA) ? "selected" : "", WIFI_STA );
  response->println("</select></p>");
  response->printf("<p>SSID <input type='text' name='wifi_ssid' size='34' maxlength='30' value='%s'></p>\n", appcfg.wifi_ssid );
  response->printf("<p>Password <input type='text' name='wifi_password' size='68' maxlength='64' value='%s'></p>\n", appcfg.wifi_password );

  // OTA (Over The Air - firmware update)
  response->println("<h2>Over The Air - firmware update (OTA)</h2>");
  response->printf("<p>OTA Hostname <input type='text' name='ota_hostname' size='68' maxlength='64' value='%s'></p>\n", appcfg.ota_hostname );
  response->printf("<p>OTA Password <input type='text' name='ota_password' size='68' maxlength='64' value='%s'></p>\n", appcfg.ota_password );

  // OpenHAB
  response->println("<h2>OpenHAB</h2>");
  response->printf("<p>OpenHAB Callback Enabled <input type='checkbox' name='ohab_enabled' value='true' %s><p>", (appcfg.ohab_enabled) ? "checked" : "" );
  response->println("<p>OpenHAB Version <select name='ohab_version'>");
  response->printf("<option %s value='1'>1.8</option>", (appcfg.ohab_version == 1) ? "selected" : "" );
  response->printf("<option %s value='2'>&gt;=2.0</option>", (appcfg.ohab_version == 2) ? "selected" : "" );
  response->println("</select></p>");
  response->printf("<p>OpenHAB Item Name <input type='text' name='ohab_itemname' size='68' maxlength='64' value='%s'></p>", appcfg.ohab_itemname );
  response->printf("<p>OpenHAB Host <input type='text' name='ohab_host' size='68' maxlength='64' value='%s'></p>", appcfg.ohab_host );
  response->printf("<p>OpenHAB Port <input type='text' name='ohab_port' size='68' maxlength='64' value='%d'></p>", appcfg.ohab_port );
  response->printf("<p>OpenHAB Use Authentication <input type='checkbox' name='ohab_useauth' value='true' %s><p>", (appcfg.ohab_useauth) ? "checked" : "" );
  response->printf("<p>OpenHAB User <input type='text' name='ohab_user' size='68' maxlength='64' value='%s'>", appcfg.ohab_user);
  response->printf("<p>OpenHAB Password <input type='text' name='ohab_password' size='68' maxlength='64' value='%s'>", appcfg.ohab_password);

  // Alexa
  response->println("<h2>Alexa</h2>");
  response->printf("<p>Alexa Enabled <input type='checkbox' name='alexa_enabled' value='true' %s><p>", (appcfg.alexa_enabled) ? "checked" : "" );
  response->printf("<p>Alexa Devicename <input type='text' name='alexa_devicename' size='68' maxlength='64' value='%s'></p>", appcfg.alexa_devicename );

  // MQTT
  response->println("<h2>MQTT</h2>");
  response->printf("<p>MQTT Enabled <input type='checkbox' name='mqtt_enabled' value='true' %s><p>", (appcfg.mqtt_enabled) ? "checked" : "" );
  response->printf("<p>MQTT Client ID <input type='text' name='mqtt_clientid' size='68' maxlength='64' value='%s'></p>", appcfg.mqtt_clientid );
  response->printf("<p>MQTT Host <input type='text' name='mqtt_host' size='68' maxlength='64' value='%s'></p>", appcfg.mqtt_host );
  response->printf("<p>MQTT Port <input type='text' name='mqtt_port' size='68' maxlength='64' value='%d'></p>", appcfg.mqtt_port );
  response->printf("<p>MQTT Use Authentication <input type='checkbox' name='mqtt_useauth' value='true' %s><p>", (appcfg.mqtt_useauth) ? "checked" : "" );
  response->printf("<p>MQTT User <input type='text' name='mqtt_user' size='68' maxlength='64' value='%s'>", appcfg.mqtt_user );
  response->printf("<p>MQTT Password <input type='text' name='mqtt_password' size='68' maxlength='64' value='%s'>", appcfg.mqtt_password );
  response->printf("<p>MQTT In Topic <input type='text' name='mqtt_intopic' size='68' maxlength='64' value='%s'>", appcfg.mqtt_intopic );
  response->printf("<p>MQTT Out Topic <input type='text' name='mqtt_outtopic' size='68' maxlength='64' value='%s'>", appcfg.mqtt_outtopic );

  response->println("<p><input type='submit' value='Save Configuration'></p>");
  response->println("</form>");
  response->printf("</body></html>");
  request->send(response);
}

void paramChars( AsyncWebServerRequest *request, char *dest,
   const char* paramName, const char* defaultValue )
{
  const char *value = defaultValue;

  if(request->hasParam( paramName, true ))
  {
    AsyncWebParameter* p = request->getParam(paramName, true);
    value = p->value().c_str();
    if ( value == 0 || strlen( value ) == 0 )
    {
      value = defaultValue;
    }
  }

  strncpy( dest, value, 63 );
  dest[63] = 0;
}

int paramInt( AsyncWebServerRequest *request, const char* paramName, int defaultValue )
{
  int value = defaultValue;

  if(request->hasParam( paramName, true ))
  {
    AsyncWebParameter* p = request->getParam(paramName, true);
    const char *pv = p->value().c_str();
    if ( pv != 0 && strlen( pv ) > 0 )
    {
      value = atoi( pv );
    }
  }

  return value;
}

bool paramBool( AsyncWebServerRequest *request, const char* paramName )
{
  bool value = false;

  if(request->hasParam( paramName, true ))
  {
    AsyncWebParameter* p = request->getParam(paramName, true);
    const char *pv = p->value().c_str();
    if ( pv != 0 && strlen( pv ) > 0 )
    {
      value = strcmp( "true", pv ) == 0;
    }
  }
  return value;
}

void handleSavePage( AsyncWebServerRequest *request )
{
  if(!request->authenticate("admin", appcfg.admin_password))
  {
    return request->requestAuthentication();
  }

  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->println("<html><head><meta http-equiv='refresh' content='30; URL=/'></head>");
  response->printf( "<body><h1>" APP_NAME " (%s) - Save Configuration</h1>", appcfg.ota_hostname );
  response->println("<h4>Version " APP_VERSION ", by " APP_AUTHOR " </h4><pre>");

  int params = request->params();

  for(int i=0;i<params;i++)
  {
    AsyncWebParameter* p = request->getParam(i);
    if(p->isFile()){ //p->isPost() is also true
      response->printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    } else if(p->isPost()){
      response->printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else {
      response->printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }

  // Security
  paramChars( request, appcfg.admin_password, "admin_password", DEFAULT_ADMIN_PASSWORD );

  // WIFI
  appcfg.wifi_mode = paramInt( request, "wifi_mode", DEFAULT_WIFI_MODE);
  paramChars( request, appcfg.wifi_ssid, "wifi_ssid", DEFAULT_WIFI_SSID );
  paramChars( request, appcfg.wifi_password, "wifi_password", DEFAULT_WIFI_PASSWORD );

  // OTA
  paramChars( request, appcfg.ota_hostname, "ota_hostname", DEFAULT_OTA_HOSTNAME );
  paramChars( request, appcfg.ota_password, "ota_password", DEFAULT_OTA_PASSWORD );

  // OpenHAB
  appcfg.ohab_enabled = paramBool( request, "ohab_enabled" );
  appcfg.ohab_version = paramInt( request, "ohab_version", DEFAULT_OHAB_VERSION );
  paramChars( request, appcfg.ohab_itemname, "ohab_itemname", DEFAULT_OHAB_ITEMNAME );
  paramChars( request, appcfg.ohab_host, "ohab_host", DEFAULT_OHAB_HOST );
  appcfg.ohab_port = paramInt( request, "ohab_port", DEFAULT_OHAB_PORT );
  appcfg.ohab_useauth = paramBool( request, "ohab_useauth" );
  paramChars( request, appcfg.ohab_user, "ohab_user", DEFAULT_OHAB_USER );
  paramChars( request, appcfg.ohab_password, "ohab_password", DEFAULT_OHAB_PASSWORD );

  // Alexa
  appcfg.alexa_enabled = paramBool( request, "alexa_enabled" );
  paramChars( request, appcfg.alexa_devicename, "alexa_devicename", DEFAULT_ALEXA_DEVICENAME );

  // MQTT
  appcfg.mqtt_enabled = paramBool( request, "mqtt_enabled" );
  paramChars( request, appcfg.mqtt_clientid, "mqtt_clientid", DEFAULT_MQTT_CLIENTID );
  paramChars( request, appcfg.mqtt_host, "mqtt_host", DEFAULT_MQTT_HOST );
  appcfg.mqtt_port = paramInt( request, "mqtt_port", DEFAULT_MQTT_PORT );
  appcfg.mqtt_useauth = paramBool( request, "mqtt_useauth" );
  paramChars( request, appcfg.mqtt_user, "mqtt_user", DEFAULT_MQTT_USER );
  paramChars( request, appcfg.mqtt_password, "mqtt_password", DEFAULT_MQTT_PASSWORD );
  paramChars( request, appcfg.mqtt_intopic, "mqtt_intopic", DEFAULT_MQTT_INTOPIC );
  paramChars( request, appcfg.mqtt_outtopic, "mqtt_outtopic", DEFAULT_MQTT_OUTTOPIC );

  response->println("</pre>");
  response->println("<h2 style='color: red'>Restarting System</h2>");
  response->println("</body></html>");
  request->send(response);
  app.delayedSystemRestart();
}


void handleRootPage( AsyncWebServerRequest *request )
{
  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->println("<html><head><meta http-equiv='refresh' content='10; URL=/'></head>");
  response->printf( "<body><h1>" APP_NAME " (%s)</h1>", appcfg.ota_hostname );
  response->println("<h4>Version " APP_VERSION ", by " APP_AUTHOR " </h4>");
  response->printf( "<h4>Current State</h4>power is %s</h4>",
    ( relayHandler.isPowerOn() ) ? "on" : "off" );

  response->println("<h4>Actions</h4>");
  response->println("<a href='on'>on</a><br/>");
  response->println("<a href='off'>off</a><br/>");
  response->println("<a href='state'>state</a><br/>");
  response->println("<a href='setup'>setup</a><br/>");

  response->println("<h4>Information</h4>");
  response->println("Build date = " __DATE__ " " __TIME__ "<br/>");
  response->printf("</body></html>");
  request->send(response);
}

WebHandler::WebHandler()
{
  initialized = false;
}

void WebHandler::setup()
{
  LOG0("HTTP server setup...\n");

  server.on( "/", HTTP_GET, handleRootPage );
  server.on( "/setup", HTTP_GET, handleSetupPage );
  server.on( "/savecfg", HTTP_POST, handleSavePage );
  server.onNotFound( handlePageNotFound );

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    relayHandler.delayedOn();
    request->send(200, "application/json", "{\"state\":1}\r\n" );
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    relayHandler.delayedOff();
    request->send(200, "application/json", "{\"state\":0}\r\n" );
  });

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", jsonStatus() );
  });

  MDNS.addService("http", "tcp", 80);
  MDNS.addServiceTxt( "http", "tcp", "path", "/" );
  MDNS.addServiceTxt( "http", "tcp", "fw_name", APP_NAME );
  MDNS.addServiceTxt( "http", "tcp", "fw_version", APP_VERSION );

  server.begin();

  LOG0("HTTP server started\n");
  initialized = true;
}

void WebHandler::handle()
{
  if( ! initialized )
  {
    setup();
  }
}
