/**
 * @file adc_resistance_reader.cpp
 * @brief Simple ADC resistance measurement via voltage divider on Raspberry Pi Pico (GP26 / A0).
 */

#include <Arduino.h>
#include "Types.h"

const u8 adcPin = A0;      // GP26 on Raspberry Pi Pico
const f32 Vref = 3.3f;     // Pico ADC reference voltage (3.3V)
const u32 R_fixed = 10000; // 10kΩ reference resistor

void setup() {
    Serial.begin(115200);
}

void loop() {
    u16 adcValue = analogRead(adcPin);                        // Read ADC value (0-1023)
    f32 Vout = (static_cast<f32>(adcValue) / 1023.0f) * Vref; // Convert to voltage

    // Calculate variable resistance using voltage divider formula:
    // Vout = Vref * (R_variable / (R_fixed + R_variable))
    // => R_variable = R_fixed * ((Vref / Vout) - 1)
    if (Vout > 0.05f && Vout < Vref) {
        f32 R_variable = static_cast<f32>(R_fixed) * ((Vref / Vout) - 1.0f);
        Serial.print("Voltage: ");
        Serial.print(Vout, 2);
        Serial.print(" V | Resistance: ");
        Serial.print(R_variable, 1);
        Serial.println(" Ohm");
    } else {
        Serial.println("Resistance too high, open circuit, or disconnected");
    }

    delay(1000);
}
