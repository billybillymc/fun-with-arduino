/*
  03 - Slew-Rate Limited Follower
  --------------------------------
  The servo follows the pot, but it can only change its commanded angle by
  MAX_STEP degrees per loop iteration. At a 20 ms loop, MAX_STEP=1 means
  the maximum slew rate is ~50 degrees/second.

  Try snapping the pot from one end to the other. Instead of jumping
  instantly, the servo slews smoothly — it physically can't keep up with
  the commanded step. Change MAX_STEP to feel different "max speeds":
    - MAX_STEP = 1   ~ 50 deg/sec  (lazy)
    - MAX_STEP = 3   ~150 deg/sec  (moderate)
    - MAX_STEP = 10  ~500 deg/sec  (servo's own physical limit)

  This is the software analog of "max joint velocity" in MoveIt 2 and
  every other industrial trajectory planner.

  Serial Plotter shows two traces:
    - target  (what the pot says)
    - actual  (what the servo is commanded, slew-limited)

  Hardware:
    - Pot wiper    -> A0
    - Servo signal -> D9
*/

#include <Servo.h>

const int SERVO_PIN = 9;
const int POT_PIN   = A0;
const int MAX_STEP  = 1;   // degrees per 20 ms iteration

Servo srv;
int currentAngle = 90;

void setup() {
  Serial.begin(115200);
  srv.attach(SERVO_PIN);
  srv.write(currentAngle);
  Serial.println("target,actual");
}

void loop() {
  int target = map(analogRead(POT_PIN), 0, 1023, 5, 175);

  if (target > currentAngle) {
    currentAngle = min(target, currentAngle + MAX_STEP);
  } else if (target < currentAngle) {
    currentAngle = max(target, currentAngle - MAX_STEP);
  }

  srv.write(currentAngle);
  Serial.print(target); Serial.print(","); Serial.println(currentAngle);
  delay(20);
}
