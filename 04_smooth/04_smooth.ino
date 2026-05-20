/*
  04 - Moving-Average Filtered Input
  -----------------------------------
  The raw pot reading has ~2-6 counts of noise even when you don't touch
  the dial. This sketch averages the last FILTER_SIZE readings to remove
  the jitter. The trade-off: response to fast pot turns lags by about
  FILTER_SIZE * 20 ms.

  Serial Plotter shows two traces so you can SEE the filter:
    - raw_angle      (jittery)
    - smoothed_angle (clean)

  Tweak FILTER_SIZE to feel the bandwidth/lag trade-off:
    - 4   -> snappy but still some noise
    - 16  -> noticeable smoothing, slight lag (default)
    - 64  -> very clean but ~1.3 second lag

  This is your first digital filter. Every sensor processing pipeline
  has one or more of these. The lesson: filtering improves precision
  at the cost of latency. There's no free lunch.

  Hardware:
    - Pot wiper    -> A0
    - Servo signal -> D9
*/

#include <Servo.h>

const int SERVO_PIN   = 9;
const int POT_PIN     = A0;
const int FILTER_SIZE = 16;

Servo srv;
int buf[FILTER_SIZE];
int idx = 0;
long sum = 0;

void setup() {
  Serial.begin(115200);
  srv.attach(SERVO_PIN);

  // Prime the filter with current pot value so it starts settled
  int seed = analogRead(POT_PIN);
  for (int i = 0; i < FILTER_SIZE; i++) buf[i] = seed;
  sum = (long)seed * FILTER_SIZE;

  Serial.println("raw,smoothed");
}

void loop() {
  int raw = analogRead(POT_PIN);

  // Update circular buffer
  sum -= buf[idx];
  buf[idx] = raw;
  sum += raw;
  idx = (idx + 1) % FILTER_SIZE;
  int filtered = sum / FILTER_SIZE;

  int angle = map(filtered, 0, 1023, 5, 175);
  srv.write(angle);

  // Print both for comparison
  int rawAngle = map(raw, 0, 1023, 5, 175);
  Serial.print(rawAngle); Serial.print(","); Serial.println(angle);
  delay(20);
}
