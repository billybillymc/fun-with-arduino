/*
  01 - Basic Pot to Servo
  -----------------------
  Turn the pot, the servo follows. The fundamental input-output loop.
  Prints commanded angle to Serial Monitor (and Serial Plotter) at 115200.

  Hardware (already wired):
    - Pot wiper        -> A0
    - Pot outer pins   -> +5V rail and GND rail
    - Servo signal     -> D9
    - Servo VCC (red)  -> +5V rail
    - Servo GND (brown)-> GND rail
    - Decoupling cap   -> across rails
*/

#include <Servo.h>

const int SERVO_PIN = 9;
const int POT_PIN   = A0;

Servo srv;

void setup() {
  Serial.begin(115200);
  srv.attach(SERVO_PIN);
}

void loop() {
  int raw   = analogRead(POT_PIN);
  int angle = map(raw, 0, 1023, 5, 175);  // clamp away from mechanical stops
  srv.write(angle);
  Serial.println(angle);
  delay(20);
}
