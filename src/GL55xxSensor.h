/*
 * GL55xxSensor -- Illuminance (lux) from a GL55xx photoresistor (LDR) via a
 * voltage divider on an ADC pin.
 *
 *  - Configurable fixed resistor
 *  - Configurable wiring orientation
 *  - Selectable GL55xx part family (built-in datasheet-derived curve constants)
 *  - Built-in averaging of multiple readings
 *  - Automatically uses the platform's full native ADC resolution (12-bit
 *    on the ESP32/ESP32-S/C series via analogReadResolution(), 10-bit on
 *    classic AVR boards like Uno/Nano/Mega)
 *  - Clamps against division by (near) zero at total darkness/saturation.
 *  - getRawResistance() for calibration (read real-world measurements).
 *
 * Circuit (voltage divider):
 *   ldrOnGround = true  (default): 3V3 -- fixed resistor -- [tap] -- LDR -- GND
 *   ldrOnGround = false           : 3V3 -- LDR -- [tap] -- fixed resistor -- GND
 *   tap = ADC pin.
 */

#ifndef GL55XXSENSOR_H_
#define GL55XXSENSOR_H_

#include <Arduino.h>

enum class GL55xxKind : uint8_t {
  GL5516,
  GL5528,   // = LDR5528, default
  GL5537_1,
  GL5537_2,
  GL5539,
  GL5549
};

class GL55xxSensor {
  public:
    // pin             : ADC-capable pin (on ESP32: GPIO32-39/ADC1, WiFi-safe)
    // otherResistorOhm: fixed divider resistor value in ohms (typical: 10000 = 10kOhm)
    // ldrOnGround     : true = LDR sits between the tap and GND (see circuit above)
    // samples         : number of readings read() averages per call
    // kind            : GL55xx variant, determines the lux curve
    GL55xxSensor(uint8_t pin, float otherResistorOhm = 10000.0f,
                 bool ldrOnGround = true, uint8_t samples = 32,
                 GL55xxKind kind = GL55xxKind::GL5528);

    // Call once in setup() -- sets the platform's full native ADC resolution
    // (12-bit on ESP32, otherwise 10-bit / no-op).
    void begin();

    // Takes `samples` readings, averages them, and updates lux/resistance.
    // Blocks briefly (analogRead is synchronous).
    void read();

    float getLux() const;
    float getFootCandles() const;
    float getRawResistance() const;  // LDR resistance in ohms, for calibration

  private:
    uint8_t _pin;
    float   _otherResistor;
    bool    _ldrOnGround;
    uint8_t _samples;
    float   _multValue;
    float   _powValue;

    float _lux;
    float _resistance;

    static constexpr float LUX_TO_FOOT_CANDLES = 10.764f;

// Full native ADC resolution per platform. ESP8266, like AVR, has a fixed
// 10-bit ADC with no analogReadResolution() -- falls into the default case.
#if defined(ESP32) || defined(ARDUINO_ARCH_ESP32)
    static constexpr uint8_t ADC_BITS = 12;
#else
    static constexpr uint8_t ADC_BITS = 10;
#endif
    static constexpr float ADC_MAX = (float)((1UL << ADC_BITS) - 1);
};

#endif  // GL55XXSENSOR_H_
