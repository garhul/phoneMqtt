#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include "Settings.h"
#include "types.h"
#include <ESP8266WebServer.h>
#include <LittleFS.h>

namespace WebServer {
  void init(void (*f)(String, String));
  void loop();

  void _cmd();
  void _control();
  void _info();
  void _settings();
  void _reboot();
  void _setup();
  void _clearCredentials();
} // namespace WebServer

#endif
