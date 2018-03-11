#include <Arduino.h>
#include <App.hpp>
#include <RelayHandler.hpp>
#include <WifiHandler.hpp>
#include <fauxmoESP.h>
#include "AlexaHandler.hpp"

static fauxmoESP fauxmo;
AlexaHandler alexaHandler;

AlexaHandler::AlexaHandler()
{
  initialized = false;
  fauxmo.enable(false);
}

void AlexaHandler::setup()
{
  if ( appcfg.alexa_enabled )
  {
    LOG0("Alexa setup...\n");
    fauxmo.addDevice(appcfg.alexa_devicename);
    fauxmo.enable(true);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state)
    {
      Serial.printf("(%ld) Device #%d (%s) state: %s\n", millis(), device_id, device_name, state ? "ON" : "OFF");

      if( state )
      {
        relayHandler.delayedOn();
      }
      else
      {
        relayHandler.delayedOff();
      }
    });

    fauxmo.onGetState([](unsigned char device_id, const char * device_name)
    {
      return relayHandler.isPowerOn();
    });
  }
  initialized = true;
}

void AlexaHandler::handle()
{
  if( appcfg.alexa_enabled && wifiHandler.isReady())
  {
    if( ! initialized )
    {
      setup();
    }

    fauxmo.handle();
  }
}
