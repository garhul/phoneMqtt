#include "Mosquitto.h"

namespace Mosquitto {
  WiFiClient wcli;
  MQTTClient client(BUFF_SIZE);
  void (*handler)(String topic, String payload);

  void handleMessage(String &topic, String &message) {
    Serial.println("Received on topic [" + topic + "] payload  [" + message + "]");
    const size_t capacity = JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument doc(capacity);

    // const char* json = "{\"cmd\":\"setoff\",\"payload\":\"30f\"}";
    deserializeJson(doc, message);
    const char *cmd = doc["cmd"];
    const char *payload = doc["payload"];

    handler(cmd, payload);
  }

  bool init(const char *broker, const char *topic, void (*msgHandler)(String topic, String payload)) {
    byte attempts = 0;

    client.begin(broker, wcli);
    Serial.println("\n Connecting to broker " + String(broker));

    while (!client.connect(Settings::getDeviceId().c_str()) && attempts < CONN_RETRIES) {
      Serial.print(".");
      attempts++;
      delay(1000);
    }

    if (!client.connected()) {
      Serial.println("Unable to connect to broker");
      return false;
    }

    Mosquitto::loop();

    Serial.println("Subcribed to topic " + String(topic));

    handler = msgHandler;
    client.subscribe(topic);
    client.onMessage(handleMessage);
    delay(2500);
    announce();
    return true;
  }

  void loop() { client.loop(); }

  void announce() {
    // message size must remain small
    Serial.println("Announcing on " + String(Settings::settings.announce_topic) + "   " + Settings::getAnnounceInfo());

    client.publish(Settings::settings.announce_topic, Settings::getAnnounceInfo());
  }

  void send(const char *topic, const char *payload) { client.publish(topic, payload); }

  bool connected() { return client.connected(); }

} // namespace Mosquitto