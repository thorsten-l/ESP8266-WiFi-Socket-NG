#include <Arduino.h>
#include <App.hpp>
#include <RelayHandler.hpp>
#include <WifiHandler.hpp>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "MqttHandler.hpp"

MqttHandler mqttHandler;

static WiFiClient wifiClient;
static PubSubClient client(wifiClient);
static long lastReconnectAttempt = 0;

static void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  Serial.println();

  if ( length == 1 )
  {
    if ( payload[0] == '1' )
    {
      relayHandler.delayedOn();
    }

    if ( payload[0] == '0' )
    {
      relayHandler.delayedOff();
    }
  }
  else
  if ( length == 2 )
  {
    if (( payload[0] == 'o' || payload[0] == 'O' )
        && ( payload[1] == 'n' || payload[1] == 'N' ))
    {
      relayHandler.delayedOn();
    }
  }
  else
  if ( length == 3 )
  {
    if (( payload[0] == 'o' || payload[0] == 'O' )
        && ( payload[1] == 'f' || payload[1] == 'F' )
        && ( payload[2] == 'f' || payload[2] == 'F' ))
    {
      relayHandler.delayedOff();
    }
  }
}

MqttHandler::MqttHandler()
{
  initialized = false;
}

bool MqttHandler::reconnect()
{
  if (( appcfg.mqtt_useauth && client.connect(appcfg.mqtt_clientid, appcfg.mqtt_user, appcfg.mqtt_password ))
      || ( !appcfg.mqtt_useauth && client.connect(appcfg.mqtt_clientid)))
  {
    LOG0( "mqtt broker connected\n" );
    client.subscribe( appcfg.mqtt_intopic );
  }

  return client.connected();
}


////////

void MqttHandler::setup()
{
  LOG0("MQTT Setup...\n");
  client.setServer( appcfg.mqtt_host, appcfg.mqtt_port );
  client.setCallback( callback );
  initialized = true;
}

void MqttHandler::handle()
{
  if ( appcfg.mqtt_enabled && wifiHandler.isReady())
  {
    if ( initialized == false )
    {
      setup();
    }

    if (!client.connected())
    {
      long now = millis();

      if (now - lastReconnectAttempt > 5000)
      {
        lastReconnectAttempt = now;

        if (reconnect())
        {
          lastReconnectAttempt = 0;
        }
      }
    }
    else
    {
      client.loop();
    }
  }
}

void MqttHandler::sendValue( const char* value )
{
  if( appcfg.mqtt_enabled && wifiHandler.isReady() && client.connected())
  {
    client.publish( appcfg.mqtt_outtopic, value );
  }
}
