#ifndef __MOSQUITTO_H__
#define __MOSQUITTO_H__

#include <ArduinoJson.h>
#include <MQTT.h>
#include <Settings.h>
#include <WiFiClient.h>
#define CONN_RETRIES 3
#define BUFF_SIZE 1024

namespace Mosquitto {
  // TODO:: add function prototype for message handling
  bool init(const char *broker, const char *topic, void (*msgHandler)(String topic, String payload));
  bool connected();
  void announce();
  void send(const char *topic, const char *payload);
  void loop();

} // namespace Mosquitto

#endif
