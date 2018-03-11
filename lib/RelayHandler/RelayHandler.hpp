#ifndef __RELAY_HANDLER_HPP__
#define __RELAY_HANDLER_HPP__

class RelayHandler
{
private:
  bool powerOn;
  bool delayState;
  bool delayAction;
  void on();
  void off();

public:
  RelayHandler();

  const bool isPowerOn();
  void toggle();
  void delayedOn();
  void delayedOff();
  void handle();
};

extern RelayHandler relayHandler;

#endif
