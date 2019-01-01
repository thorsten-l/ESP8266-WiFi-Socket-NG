#include "AlexaHandler.hpp"
#include <App.hpp>
#include <Arduino.h>
#include <RelayHandler.hpp>
#include <WifiHandler.hpp>

fauxmoESP fauxmo;
AlexaHandler alexaHandler;

AlexaHandler::AlexaHandler()
{
  initialized = false;
  fauxmo.enable(false);
}

void AlexaHandler::setup()
{
  LOG1("Alexa setup... enabled=%s\n",
       (appcfg.alexa_enabled) ? "true" : "false");

  fauxmo.createServer(false);
  fauxmo.setPort(80);
  fauxmo.enable(appcfg.alexa_enabled);
  fauxmo.addDevice(appcfg.alexa_devicename);

  fauxmo.onSetState([](unsigned char device_id, const char *device_name,
                       bool state, unsigned char value) {

    Serial.printf("(%ld) Device #%d (%s) state: %s\n", millis(), device_id,
                  device_name, state ? "ON" : "OFF");

    if (state)
    {
      relayHandler.delayedOn();
    }
    else
    {
      relayHandler.delayedOff();
    }
  });

  initialized = true;
}

void AlexaHandler::handle()
{
  if (wifiHandler.isReady())
  {
    if (!initialized)
    {
      setup();
    }

    fauxmo.handle();
  }
}

void AlexaHandler::sendValue( bool state )
{
  if ( state )
  {
    fauxmo.setState( appcfg.alexa_devicename, true, 255 );
  }
  else
  {
    fauxmo.setState( appcfg.alexa_devicename, false, 0 );
  }
}
