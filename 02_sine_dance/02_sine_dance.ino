/*
  02 - Sine Wave Dance
  --------------------
  The servo dances on its own in a smooth sine wave. The pot controls SPEED:
    - Pot fully left  = slow oscillation (~0.1 Hz, 10 sec per cycle)
    - Pot fully right = fast oscillation (~3 Hz, 0.3 sec per cycle)
  The amplitude is fixed: ±60° around the center (30° to 150°).

  This shows you that the servo doesn't need a setpoint stream — you can
  generate the motion yourself with math. Same idea behind walking-robot
  gait generators and any kind of cyclic motion (oscillating fans,
  windshield wipers, pendulum clocks).

  Hardware:
    - Pot wiper        -> A0  (now controls SPEED, not position)
    - Servo signal     -> D9
*/

#include <Servo.h>

const int SERVO_PIN = 9;
const int POT_PIN   = A0;

const float MIN_FREQ_HZ = 0.1f;
const float MAX_FREQ_HZ = 3.0f;
const int   CENTER_DEG  = 90;
const int   AMPLITUDE   = 60;

Servo srv;

void setup() {
  Serial.begin(115200);
  srv.attach(SERVO_PIN);
}

void loop() {
  // Pot 0..1023 -> oscillation frequency in Hz
  float t01  = analogRead(POT_PIN) / 1023.0f;
  float freq = MIN_FREQ_HZ + t01 * (MAX_FREQ_HZ - MIN_FREQ_HZ);

  float t     = millis() / 1000.0f;
  int   angle = CENTER_DEG + (int)(AMPLITUDE * sin(t * freq * TWO_PI));

  srv.write(angle);
  Serial.println(angle);
  delay(15);
}
