#include <Arduino.h>

#define INT_PIN_PULSE 12
#define INT_PIN_TONE 14
#define DIAL_WINDOW_LIMIT 500
#define PULSE_WINDOW_MIN 30
#define PULSE_WINDOW_MAX 200

volatile byte pulseCount = 0;
volatile bool canCount = false;
volatile unsigned long last_pulse_t = 0;
volatile bool ready = false;

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

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up...");

  pinMode(INT_PIN_PULSE, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN_PULSE), PULSE_ISR, FALLING);
}

inline void processTone() {
  if (ready && millis() > (DIAL_WINDOW_LIMIT + last_pulse_t)) {
    Serial.print("New input value ");
    Serial.println(pulseCount);
    last_pulse_t = 0;
    pulseCount = 0;
    ready = false;
  }
}

void loop() {
  processTone();

  delay(100);
}