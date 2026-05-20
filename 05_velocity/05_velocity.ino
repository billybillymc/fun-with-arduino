/*
  05 - Velocity Control (Joystick Mode)
  --------------------------------------
  Instead of mapping pot directly to position, the pot now controls
  VELOCITY. The servo position integrates over time:
    - Pot centered (with a deadzone) -> servo holds position
    - Pot turned right -> servo moves toward 180 at speed proportional
                          to how far right you turn it
    - Pot turned left  -> servo moves toward 0   at speed proportional
                          to how far left you turn it

  This is exactly how an RC car's throttle stick or a video game joystick
  works: input = velocity, not position. Notice the very different feel
  from sketches 01-04. Holding still on a velocity input maintains
  position; releasing snaps to neutral; you can't "snap" to a position,
  you have to drive there.

  Serial Plotter shows commanded velocity (deg/sec) and current position.

  Hardware:
    - Pot wiper    -> A0
    - Servo signal -> D9
*/

#include <Servo.h>

const int   SERVO_PIN     = 9;
const int   POT_PIN       = A0;
const int   CENTER        = 512;     // pot value at midpoint
const int   DEADZONE      = 50;      // ignore offsets within +/- this
const float MAX_VEL_DPS   = 120.0f;  // max velocity, degrees per second
const float MIN_ANGLE     = 5.0f;
const float MAX_ANGLE     = 175.0f;

Servo srv;
float position = 90.0f;
unsigned long lastUpdateMs = 0;

void setup() {
  Serial.begin(115200);
  srv.attach(SERVO_PIN);
  srv.write((int)position);
  lastUpdateMs = millis();
  Serial.println("velocity_dps,position_deg");
}

void loop() {
  unsigned long now = millis();
  float dt = (now - lastUpdateMs) / 1000.0f;   // seconds since last update
  lastUpdateMs = now;

  int raw    = analogRead(POT_PIN);
  int offset = raw - CENTER;                    // -512..+511

  float velocity = 0.0f;
  if (offset > DEADZONE) {
    float scale = (offset - DEADZONE) / (511.0f - DEADZONE);
    velocity = scale * MAX_VEL_DPS;
  } else if (offset < -DEADZONE) {
    float scale = (offset + DEADZONE) / (512.0f - DEADZONE);
    velocity = scale * MAX_VEL_DPS;             // already negative
  }

  position += velocity * dt;
  position  = constrain(position, MIN_ANGLE, MAX_ANGLE);

  srv.write((int)position);

  Serial.print(velocity, 1); Serial.print(","); Serial.println(position, 1);
  delay(20);
}
