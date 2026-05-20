# Arduino Breadboard · Project 01 Sketches

Six self-contained sketches for the **pot + SG90 servo + capacitor** breadboard
you already have. No additional hardware required for any of them.

To use: in Arduino IDE, `File → Open` the `.ino` file you want, click Upload.

Required wiring (already in place):

- Pot wiper        → A0
- Pot outer pins   → +5V rail and GND rail
- Servo signal     → D9
- Servo VCC (red)  → +5V rail
- Servo GND (brown)→ GND rail
- Arduino 5V       → +5V rail
- Arduino GND      → GND rail
- Decoupling cap   → across rails near servo

## The sketches

| # | Folder | What it does | Concept |
|---|---|---|---|
| 01 | `01_basic` | Pot directly commands servo position. Baseline. | Direct input → output mapping |
| 02 | `02_sine_dance` | Servo oscillates autonomously; pot controls speed. | Time-based motion generation |
| 03 | `03_slew_limit` | Pot commands position, but servo speed is capped. | Max-velocity / slew rate limiting |
| 04 | `04_smooth` | Moving-average filter removes pot jitter. | Digital low-pass filter, bandwidth/lag trade-off |
| 05 | `05_velocity` | Pot now controls velocity, not position. | Velocity-mode joystick control |
| 06 | `06_teach_pendant` | Record/playback sequences with EEPROM save. | State machine, command dispatch, non-volatile memory |

## Recommended order

1. Start with **01** to confirm everything works.
2. Try **02** for instant gratification (servo dances by itself).
3. **03** and **04** to feel the difference between mechanical and digital filtering.
4. **05** to experience velocity-mode control (different mental model).
5. **06** is the big one — record a sequence, save to EEPROM, unplug from your
   computer, and watch your Arduino run the sequence standalone.

## For sketch 06 specifically

Open Serial Monitor at 115200 baud. Line-ending dropdown: "No line ending" or
"Newline" (avoid "Both NL & CR").

Type a single letter and hit Enter:

- `r` — record current pot angle
- `p` — play back recorded sequence
- `c` — clear and return to record mode
- `s` — show current state
- `w` — save sequence to EEPROM (persists across power-off)
- `l` — load saved sequence and play it
- `e` — erase EEPROM

After saving with `w`, unplug from computer. Plug into any USB-C wall charger or
USB power bank. The Arduino auto-loads and auto-plays the saved sequence.
