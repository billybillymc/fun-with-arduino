/*
  06 - Teach Pendant with EEPROM Persistence
  -------------------------------------------
  Record a sequence of servo positions by demonstration, then play it
  back forever. Save the sequence to non-volatile EEPROM so it survives
  power-off — the Arduino can play it standalone, unplugged from your
  computer.

  In Serial Monitor (115200 baud, "No line ending" or "Newline"):

    r  - record current pot angle into memory
    p  - switch to PLAYBACK mode (loops through stored positions)
    c  - clear memory and return to RECORD
    s  - show current state (mode, count, all stored positions)
    w  - save current sequence to EEPROM (persists across power-off)
    l  - load saved sequence from EEPROM and play it back
    e  - erase EEPROM

  On boot, if a saved sequence is found, the Arduino auto-plays it.
  So the workflow for untethered operation is:
    1. Record positions (r r r r r)
    2. Save them (w)
    3. Unplug from computer
    4. Plug into any USB-C power source (wall charger, power bank)
    5. Servo plays the sequence forever

  Hardware:
    - Pot wiper    -> A0
    - Servo signal -> D9
    - Built-in LED is used as a status indicator
        off  = RECORD mode
        on   = PLAYBACK mode
        flash = position captured
*/

#include <Servo.h>
#include <EEPROM.h>

const int SERVO_PIN = 9;
const int POT_PIN   = A0;
const int LED_PIN   = LED_BUILTIN;

const int MAX_POSITIONS = 10;
const unsigned long STEP_MS = 800;

// EEPROM layout
const int  ADDR_MAGIC = 0;
const int  ADDR_COUNT = 1;
const int  ADDR_DATA  = 2;
const byte MAGIC      = 0xA5;

Servo srv;
enum Mode { RECORD, PLAYBACK };
Mode mode = RECORD;

int positions[MAX_POSITIONS];
int count = 0;
int playIdx = 0;
unsigned long lastPlayMs = 0;

int currentAngle() {
  return map(analogRead(POT_PIN), 0, 1023, 5, 175);
}

void captureAngle() {
  if (mode != RECORD) {
    Serial.println("[type 'c' first to clear and re-enter RECORD]");
    return;
  }
  if (count >= MAX_POSITIONS) {
    Serial.println("[memory full]");
    return;
  }
  int a = currentAngle();
  positions[count++] = a;
  Serial.print("  recorded #"); Serial.print(count);
  Serial.print(": "); Serial.print(a); Serial.println(" deg");
  digitalWrite(LED_PIN, HIGH); delay(80); digitalWrite(LED_PIN, LOW);
}

void enterPlayback() {
  if (count == 0) {
    Serial.println("[nothing to play - record some positions first]");
    return;
  }
  mode = PLAYBACK;
  playIdx = 0;
  lastPlayMs = millis();
  Serial.print(">>> PLAYBACK ("); Serial.print(count); Serial.println(" positions, looping)");
  digitalWrite(LED_PIN, HIGH);
}

void enterRecord() {
  mode = RECORD;
  count = 0;
  playIdx = 0;
  Serial.println(">>> RECORD (memory cleared)");
  digitalWrite(LED_PIN, LOW);
}

void showStatus() {
  Serial.print("mode=");  Serial.print(mode == RECORD ? "RECORD" : "PLAYBACK");
  Serial.print("  count="); Serial.print(count);
  Serial.print("  [");
  for (int i = 0; i < count; i++) {
    Serial.print(positions[i]);
    if (i < count - 1) Serial.print(",");
  }
  Serial.println("]");
}

void saveToEEPROM() {
  EEPROM.write(ADDR_MAGIC, MAGIC);
  EEPROM.write(ADDR_COUNT, count);
  for (int i = 0; i < count; i++) {
    EEPROM.put(ADDR_DATA + i * sizeof(int), positions[i]);
  }
  Serial.print("saved "); Serial.print(count); Serial.println(" positions to EEPROM");
}

bool loadFromEEPROM() {
  if (EEPROM.read(ADDR_MAGIC) != MAGIC) {
    Serial.println("[no saved data]");
    return false;
  }
  int stored = EEPROM.read(ADDR_COUNT);
  if (stored < 0 || stored > MAX_POSITIONS) {
    Serial.println("[corrupt data]");
    return false;
  }
  count = stored;
  for (int i = 0; i < count; i++) {
    EEPROM.get(ADDR_DATA + i * sizeof(int), positions[i]);
  }
  Serial.print("loaded "); Serial.print(count); Serial.println(" positions from EEPROM");
  return true;
}

void eraseEEPROM() {
  EEPROM.write(ADDR_MAGIC, 0xFF);
  Serial.println("erased EEPROM");
}

void setup() {
  Serial.begin(115200);
  srv.attach(SERVO_PIN);
  pinMode(LED_PIN, OUTPUT);

  // Auto-load on boot. If a sequence is saved, play it immediately.
  if (loadFromEEPROM() && count > 0) {
    Serial.println("auto-playing saved sequence");
    enterPlayback();
  }

  Serial.println("=== TEACH PENDANT ===");
  Serial.println("r=record  p=play  c=clear  s=status  w=save  l=load  e=erase");
}

void loop() {
  unsigned long now = millis();

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r' || c == ' ') continue;
    switch (c) {
      case 'r': captureAngle();    break;
      case 'p': enterPlayback();   break;
      case 'c': enterRecord();     break;
      case 's': showStatus();      break;
      case 'w': saveToEEPROM();    break;
      case 'l':
        if (loadFromEEPROM() && count > 0) enterPlayback();
        break;
      case 'e': eraseEEPROM();     break;
      default:
        Serial.print("[unknown: '"); Serial.print(c); Serial.println("']");
    }
  }

  if (mode == RECORD) {
    srv.write(currentAngle());
  } else {
    if (now - lastPlayMs > STEP_MS) {
      srv.write(positions[playIdx]);
      Serial.print("  -> "); Serial.print(positions[playIdx]); Serial.println(" deg");
      playIdx = (playIdx + 1) % count;
      lastPlayMs = now;
    }
  }

  delay(5);
}
