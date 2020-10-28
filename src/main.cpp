#include "Mosquitto.h"
#include "Network.h"
#include "Settings.h"
#include "WebServer.h"
#include "defaults.h"
#include "types.h"
#include <Arduino.h>
#include <MQTT.h>

WiFiClient net;

volatile byte pulseCount = 0;
volatile bool canCount = false;
volatile unsigned long last_pulse_t = 0;
volatile bool ready = false;

void messageHandler(String cmd, String payload) {
  Serial.print("|CMD: " + cmd + " |");
  Serial.print("|Payload:" + payload + " |");
  Serial.println("");

  // Add something for ringing ?
}

void ICACHE_RAM_ATTR PULSE_ISR() {
  noInterrupts();
  unsigned long duration = millis() - last_pulse_t;
  last_pulse_t = millis();

  if (duration < PULSE_WINDOW_MAX && duration > PULSE_WINDOW_MIN) {
    pulseCount++;
  }

  ready = true;
  interrupts();
}

void setup(void) {
  Serial.begin(115200);
  Settings::init();

  Network::init(Settings::settings.ssid, Settings::settings.pass);

  if (Network::getMode() == Network::MODES::ST) {
    Mosquitto::init(Settings::settings.broker, Settings::settings.topic, messageHandler);
  }

  WebServer::init(messageHandler);
  analogWriteFreq(440);
  analogWrite(INT_PIN_TONE, 50);
  pinMode(INT_PIN_PULSE, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN_PULSE), PULSE_ISR, FALLING);
}

inline void processTone() {
  if (ready && millis() > (DIAL_WINDOW_LIMIT + last_pulse_t)) {
    Serial.print("New input value ");
    Serial.println(pulseCount);
    // Make programmable messages (programmable via setup)

    Mosquitto::send(Settings::settings.topic, String(pulseCount).c_str());

    last_pulse_t = 0;
    pulseCount = 0;
    ready = false;
  }
}

void loop() {
  Network::checkAlive();

  if (!Mosquitto::connected()) {
    Mosquitto::init(Settings::settings.broker, Settings::settings.topic, messageHandler);
  }

  WebServer::loop();
  Mosquitto::loop();

  processTone();
  delay(100);
}