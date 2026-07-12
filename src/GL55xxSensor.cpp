#include "GL55xxSensor.h"

GL55xxSensor::GL55xxSensor(uint8_t pin, float otherResistorOhm, bool ldrOnGround,
                            uint8_t samples, GL55xxKind kind)
  : _pin(pin), _otherResistor(otherResistorOhm), _ldrOnGround(ldrOnGround),
    _samples(samples ? samples : 1), _lux(0.0f), _resistance(0.0f) {
  // Curve constants per part (derived from the respective GL55xx datasheet).
  switch (kind) {
    case GL55xxKind::GL5516:
      _multValue = 29634400.0f; _powValue = 1.6689f; break;
    case GL55xxKind::GL5537_1:
      _multValue = 32435800.0f; _powValue = 1.4899f; break;
    case GL55xxKind::GL5537_2:
      _multValue = 2801820.0f;  _powValue = 1.1772f; break;
    case GL55xxKind::GL5539:
      _multValue = 208510000.0f; _powValue = 1.4850f; break;
    case GL55xxKind::GL5549:
      _multValue = 44682100.0f; _powValue = 1.2750f; break;
    case GL55xxKind::GL5528:
    default:
      _multValue = 32017200.0f; _powValue = 1.5832f; break;
  }
}

void GL55xxSensor::begin() {
#if defined(ESP32) || defined(ARDUINO_ARCH_ESP32)
  analogReadResolution(ADC_BITS);
#endif
  // AVR/ESP8266 have no software-selectable ADC resolution -- the hardware
  // is fixed at 10-bit there anyway, no call needed.
}

void GL55xxSensor::read() {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < _samples; i++) {
    sum += analogRead(_pin);
  }
  float avg = (float)sum / (float)_samples;

  // Clamp against total darkness (avg->0) or saturation (avg->ADC_MAX),
  // otherwise ratio becomes 0 or infinite and pow()/division yields Inf/NaN.
  if (avg < 1.0f) avg = 1.0f;
  if (avg > ADC_MAX - 1.0f) avg = ADC_MAX - 1.0f;

  float ratio = (ADC_MAX / avg) - 1.0f;

  if (_ldrOnGround) {
    _resistance = _otherResistor / ratio;
  } else {
    _resistance = _otherResistor * ratio;
  }
  if (_resistance < 1.0f) _resistance = 1.0f;

  _lux = _multValue / powf(_resistance, _powValue);
}

float GL55xxSensor::getLux() const {
  return _lux;
}

float GL55xxSensor::getFootCandles() const {
  return _lux / LUX_TO_FOOT_CANDLES;
}

float GL55xxSensor::getRawResistance() const {
  return _resistance;
}
