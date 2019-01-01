#ifndef __ALEXA_HANDLER_H__
#define __ALEXA_HANDLER_H__

#include <fauxmoESP.h>

class AlexaHandler
{
private:
  bool initialized;
  void setup();

public:
  AlexaHandler();
  void handle();
  void sendValue( bool state );
};

extern AlexaHandler alexaHandler;
extern fauxmoESP fauxmo;

#endif
