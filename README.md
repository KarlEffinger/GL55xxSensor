# GL55xxSensor

Arduino library to read illuminance (lux) from a GL55xx photoresistor (LDR) —
GL5516, GL5528 (a.k.a. LDR5528), GL5537-1, GL5537-2, GL5539, or GL5549 — wired
as a simple voltage divider.

Built for the ESP32 but portable to any Arduino-compatible board.

## Features

- Selectable GL55xx part with built-in, datasheet-derived lux curve per variant
- Configurable fixed resistor value and wiring orientation
- Built-in averaging of multiple ADC readings for a stable value
- Automatically uses the platform's full native ADC resolution (12-bit on
  ESP32 via `analogReadResolution()`, 10-bit on classic AVR boards like
  Uno/Nano/Mega, where that function doesn't exist)
- Guards against division by (near) zero at total darkness or sensor saturation
- `getRawResistance()` exposes the calculated LDR resistance for calibration

## Wiring

Voltage divider between the supply rail and GND, with the ADC pin reading the
midpoint:

```
ldrOnGround = true  (default):  3V3 -- fixed resistor -- [ADC pin] -- LDR -- GND
ldrOnGround = false            :  3V3 -- LDR -- [ADC pin] -- fixed resistor -- GND
```

A 10 kΩ fixed resistor is a common choice for GL5528/LDR5528 at typical
ambient light levels.

On ESP32, use an **ADC1** pin (GPIO32–39) if WiFi is active — ADC2 pins
become unreliable while the WiFi radio is running.

## Installation

- **Arduino Library Manager** (once accepted into the registry): Sketch →
  Include Library → Manage Libraries… → search "GL55xxSensor" → Install
- **Manual**: download/clone this repository into your sketchbook's
  `libraries/` folder (so the path is `libraries/GL55xxSensor/library.properties`)

## Usage

```cpp
#include <GL55xxSensor.h>

#define LDR_PIN 34

// 10kOhm fixed resistor, LDR on ground side, 32-sample average, GL5528 (defaults)
GL55xxSensor lightSensor(LDR_PIN);

void setup() {
  Serial.begin(115200);
  lightSensor.begin();
}

void loop() {
  lightSensor.read();
  Serial.printf("Lux: %.1f  (LDR resistance: %.0f Ohm)\n",
                lightSensor.getLux(), lightSensor.getRawResistance());
  delay(1000);
}
```

See [`examples/GL55xxTest`](examples/GL55xxTest) for the full sketch.

## API

```cpp
GL55xxSensor(uint8_t pin,
             float otherResistorOhm = 10000.0f,
             bool ldrOnGround = true,
             uint8_t samples = 32,
             GL55xxKind kind = GL55xxKind::GL5528);

void begin();                    // call once in setup()
void read();                     // take & average `samples` readings
float getLux() const;
float getFootCandles() const;
float getRawResistance() const;  // LDR resistance in ohms, for calibration
```

`GL55xxKind` values: `GL5516`, `GL5528`, `GL5537_1`, `GL5537_2`, `GL5539`, `GL5549`.

## Credits

Combines ideas from two existing libraries:

- [LightDependentResistor](https://github.com/QuentinCG/Arduino-Light-Dependent-Resistor-Library)
  by Quentin Comte-Gaz (MIT) — configurable divider resistor, wiring
  orientation, and per-part GL55xx lux curve constants
- [TroykaLight](https://github.com/amperka/TroykaLight) by Amperka /
  Igor Dementiev — built-in oversampling of multiple readings

## License

[MIT](LICENSE)
