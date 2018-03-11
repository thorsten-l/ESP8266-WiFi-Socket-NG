#ifndef __ALEXA_HANDLER_H__
#define __ALEXA_HANDLER_H__

class AlexaHandler
{
private:
  bool initialized;
  void setup();

public:
  AlexaHandler();
  void handle();
};

extern AlexaHandler alexaHandler;

#endif
