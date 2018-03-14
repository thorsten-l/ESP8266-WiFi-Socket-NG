#include <App.hpp>
#include <DefaultAppConfig.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WifiHandler.hpp>
#include <RelayHandler.hpp>
#include "WebHandler.hpp"
#include "pure-min-css-gz.h"
#include "layout-css-gz.h"
#include "template-html.h"

WebHandler webHandler;

static AsyncWebServer server(80);
static bool webPowerState;

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

void prLegend( AsyncResponseStream *response, const char *name )
{
  response->printf( "<legend>%s</legend>", name );
}

void prGroupLabel( AsyncResponseStream *response, int id, const char *label )
{
  response->printf(
    "<div class='pure-control-group'>"
      "<label for='pgid%d'>%s</label>", id, label );
}

void prTextGroup( AsyncResponseStream *response, int id, const char *label,
   const char *name, const char *value )
{
  prGroupLabel( response, id, label );
  response->printf(
      "<input id='pgid%d' type='text' name='%s' maxlength='64' value='%s'>"
    "</div>", id, name, value );
}

void prTextGroup( AsyncResponseStream *response, int id, const char *label,
   const char *name, int value )
{
  prGroupLabel( response, id, label );
  response->printf(
      "<input id='pgid%d' type='text' name='%s' maxlength='64' value='%d'>"
    "</div>", id, name, value );
}

void prCheckBoxGroup( AsyncResponseStream *response, int id, const char *label,
   const char *name, bool value )
{
  prGroupLabel( response, id, label );
  response->printf(
      "<input id='pgid%d' type='checkbox' name='%s' value='true' %s>"
    "</div>", id, name, value ? "checked" : "" );
}

void prSelectStart( AsyncResponseStream *response, int id, const char *label, const char *name )
{
  prGroupLabel( response, id, label );
  response->printf("<select id='pgid%d' name='%s'>", id, name );
}

void prSelectEnd( AsyncResponseStream *response )
{
  response->print("</select></div>" );
}

void prOption( AsyncResponseStream *response, int value, const char *name, bool selected )
{
  response->printf("<option %s value='%d'>%s</option>", selected ? "selected" : "", value, name );
}

void handleSetupPage( AsyncWebServerRequest *request )
{
  int id = 0;

  if(!request->authenticate("admin", appcfg.admin_password))
  {
    return request->requestAuthentication();
  }

  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->printf( TEMPLATE_HEADER, APP_NAME " - Setup");
  response->println("<form class=\"pure-form pure-form-aligned\" action='/savecfg' method=POST><fieldset>");

  // Setup

  prLegend( response, "Setup 'admin' user" );
  prTextGroup( response, id++, "Password", "admin_password", appcfg.admin_password );

  // WiFi
  prLegend( response, "WiFi Network Scan" );
  response->printf("<pre>%s</pre>\n", wifiHandler.getScannedNetworks() );

  prLegend( response, "WiFi" );

  prSelectStart( response, id++, "Mode", "wifi_mode" );
  prOption( response, WIFI_AP, "Access Point", appcfg.wifi_mode == WIFI_AP );
  prOption( response, WIFI_STA, "Station", appcfg.wifi_mode == WIFI_STA );
  prSelectEnd( response );

  prTextGroup( response, id++, "SSID", "wifi_ssid", appcfg.wifi_ssid );
  prTextGroup( response, id++, "Password", "wifi_password", appcfg.wifi_password );

  // OTA (Over The Air - firmware update)
  prLegend( response, "Over The Air - firmware update (OTA)");
  prTextGroup( response, id++, "Hostname", "ota_hostname", appcfg.ota_hostname );
  prTextGroup( response, id++, "Password", "ota_password", appcfg.ota_password );

  // OpenHAB
  prLegend( response, "OpenHAB");
  prCheckBoxGroup( response, id++, "Callback Enabled", "ohab_enabled", appcfg.ohab_enabled );

  prSelectStart( response, id++, "OpenHAB Version", "ohab_version" );
  prOption( response, 1, "1.8", appcfg.ohab_version == 1 );
  prOption( response, 2, "&gt;=2.0", appcfg.ohab_version == 2 );
  prSelectEnd( response );

  prTextGroup( response, id++, "Item Name", "ohab_itemname", appcfg.ohab_itemname );
  prTextGroup( response, id++, "Host", "ohab_host", appcfg.ohab_host );
  prTextGroup( response, id++, "Port", "ohab_port", appcfg.ohab_port );
  prCheckBoxGroup( response, id++, "Use Authentication", "ohab_useauth", appcfg.ohab_useauth );
  prTextGroup( response, id++, "User", "ohab_user", appcfg.ohab_user );
  prTextGroup( response, id++, "Password", "ohab_password", appcfg.ohab_password );


  // Alexa
  prLegend( response, "Alexa");
  prCheckBoxGroup( response, id++, "Enabled", "alexa_enabled", appcfg.alexa_enabled );
  prTextGroup( response, id++, "Devicename", "alexa_devicename", appcfg.alexa_devicename );


  // MQTT
  prLegend( response, "MQTT");
  prCheckBoxGroup( response, id++, "Enabled", "mqtt_enabled", appcfg.mqtt_enabled );
  prTextGroup( response, id++, "Client ID", "mqtt_clientid", appcfg.mqtt_clientid );
  prTextGroup( response, id++, "Host", "mqtt_host", appcfg.mqtt_host );
  prTextGroup( response, id++, "Port", "mqtt_port", appcfg.mqtt_port );
  prCheckBoxGroup( response, id++, "Use Authentication", "mqtt_useauth", appcfg.mqtt_useauth );
  prTextGroup( response, id++, "User", "mqtt_user", appcfg.mqtt_user );
  prTextGroup( response, id++, "Password", "mqtt_password", appcfg.mqtt_password );
  prTextGroup( response, id++, "In Topic", "mqtt_intopic", appcfg.mqtt_intopic );
  prTextGroup( response, id++, "Out Topic", "mqtt_outtopic", appcfg.mqtt_outtopic );

  response->println("<p><input class='pure-button pure-button-primary' type='submit' value='Save Configuration'></p>");
  response->println("</fieldset></form>");
  response->print( TEMPLATE_FOOTER );
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
  response->printf( TEMPLATE_HEADER, APP_NAME " - Save Configuration");
  response->print("<pre>");

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
  response->print( TEMPLATE_FOOTER );
  request->send(response);
  app.delayedSystemRestart();
}


void handleRootPage( AsyncWebServerRequest *request )
{
  webPowerState = relayHandler.isPowerOn();

  if(request->hasParam( "power" ))
  {
    AsyncWebParameter* p = request->getParam("power");
    const char *pv = p->value().c_str();
    if ( pv != 0 && strlen( pv ) > 0 )
    {
      if ( strcmp( "ON", pv ) == 0 )
      {
        webPowerState = true;
        relayHandler.delayedOn();
      }
      if ( strcmp( "OFF", pv ) == 0 )
      {
        webPowerState = false;
        relayHandler.delayedOff();
      }
    }
    request->redirect("/");
    return;
  }

  char titleBuffer[100];
  sprintf( titleBuffer, APP_NAME " - %s", appcfg.ota_hostname );

  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->printf( TEMPLATE_HEADER, titleBuffer );

  response->print("<form class='pure-form'>");
  prLegend( response, "Current Status");

  response->printf("<button id=\"status-button\" class=\"pure-button\" style=\"background-color: #%s\">Power is %s</button>",
   webPowerState ? "80ff80" : "ff8080", webPowerState ? "ON" : "OFF" );
   prLegend( response, "Actions");
  response->print(
  "<a href=\"/?power=ON\" class=\"pure-button button-on\">ON</a>"
  "<a href=\"/?power=OFF\" class=\"pure-button button-off\">OFF</a>");

  response->print("</form>");
  response->print( TEMPLATE_FOOTER );
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
  server.on( "/setup.html", HTTP_GET, handleSetupPage );
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

  server.on("/pure-min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css",
            PURE_MIN_CSS_GZ, PURE_MIN_CSS_GZ_LEN);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/layout.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css",
            LAYOUT_CSS_GZ, LAYOUT_CSS_GZ_LEN);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/info.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->printf( TEMPLATE_HEADER, APP_NAME " - Info");

    response->print("<form class='pure-form'>");


    prLegend( response, "Application");

    response->print(
      "<p>Name: " APP_NAME "</p>"
      "<p>Version: " APP_VERSION "</p>"
      "<p>Author: Dr. Thorsten Ludewig &lt;t.ludewig@gmail.com></p>");

    prLegend( response, "RESTful API");
      char ipAddress[16];
      strcpy( ipAddress, WiFi.localIP().toString().c_str());

    response->printf("<p><a href='http://%s/on'>http://%s/on</a> - Socket ON</p>"
      "<p><a href='http://%s/off'>http://%s/off</a> - Socket OFF</p>"
      "<p><a href='http://%s/state'>http://%s/state</a> - Socket JSON status (0 or 1)</p>",
      ipAddress, ipAddress, ipAddress, ipAddress, ipAddress, ipAddress );

      prLegend( response, "Build");
      response->print(
        "<p>Date: " __DATE__ "</p>"
        "<p>Time: " __TIME__ "</p>");

    prLegend( response, "Services");
    response->printf( "<p>OpenHAB Callback Enabled: %s</p>", (appcfg.ohab_enabled) ? "true" : "false" );
    response->printf( "<p>Alexa Enabled: %s</p>", (appcfg.alexa_enabled) ? "true" : "false" );
    response->printf( "<p>MQTT Enabled: %s</p>", (appcfg.mqtt_enabled) ? "true" : "false" );

    response->print("</form>");
    response->print( TEMPLATE_FOOTER );
    request->send( response );
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
