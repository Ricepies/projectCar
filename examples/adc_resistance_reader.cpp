/**
 * @file adc_resistance_reader.cpp
 * @brief Simple ADC resistance measurement via voltage divider on Raspberry Pi Pico (GP26 / A0).
 */

#include <Arduino.h>

const int adcPin = A0;     // GP26 on Raspberry Pi Pico
const float Vref = 3.3;    // Pico ADC reference voltage (3.3V)
const int R_fixed = 10000; // 10kΩ reference resistor

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    int adcValue = analogRead(adcPin);       // Read ADC value (0-1023)
    float Vout = (adcValue / 1023.0) * Vref; // Convert to voltage

    // Calculate variable resistance using voltage divider formula:
    // Vout = Vref * (R_variable / (R_fixed + R_variable))
    // => R_variable = R_fixed * ((Vref / Vout) - 1)
    if (Vout > 0.05 && Vout < Vref)
    {
        float R_variable = R_fixed * ((Vref / Vout) - 1.0);
        Serial.print("Voltage: ");
        Serial.print(Vout, 2);
        Serial.print(" V | Resistance: ");
        Serial.print(R_variable, 1);
        Serial.println(" Ohm");
    }
    else
    {
        Serial.println("Resistance too high, open circuit, or disconnected");
    }

    delay(1000);
}
