// Test sketch: reads the GL5528/LDR5528 on GPIO34 and prints lux + raw
// resistance every second over the serial console (115200 baud).
//
// Wiring (ldrOnGround=true, default):
//   3V3 -- 10kOhm resistor -- [GPIO34] -- LDR5528 -- GND

#include <GL55xxSensor.h>

#define LDR_PIN 34

// 10kOhm, ldrOnGround=true, 32 samples, GL5528 (defaults)
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
