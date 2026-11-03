/**
 * @file main.cpp
 * @brief 4WD Smart Car Firmware with PS2 Controller for Raspberry Pi Pico (RP2040).
 *
 * Hardware Pin Connections:
 *  - Motors (PWM H-Bridge):
 *      FL (Front Left):   GP0, GP1
 *      FR (Front Right):  GP2, GP3
 *      RL (Rear Left):    GP6, GP7
 *      RR (Rear Right):   GP4, GP5
 *  - PS2 Gamepad (Bit-banged SPI):
 *      DAT / MOSI:        GP12
 *      CMD / MISO:        GP13
 *      SEL / CS:          GP14
 *      CLK / SCK:         GP15
 *  - Status Buzzer:
 *      Buzzer:            GP16
 */

#include <Arduino.h>
#include "Motors.h"
#include "Gamepad.h"

// Hardware Pin Definitions
constexpr uint8_t BUZZER_PIN = 16;

// 4WD Motors
Motor motorFL(0, 1);
Motor motorFR(2, 3);
Motor motorRL(6, 7);
Motor motorRR(4, 5);

// PS2 Gamepad
Gamepad gamepad(12, 13, 14, 15);

// System State
bool motorStarted = false;

void beep(unsigned int durationMs = 80, unsigned int frequencyHz = 1500)
{
    tone(BUZZER_PIN, frequencyHz, durationMs);
    delay(durationMs);
    noTone(BUZZER_PIN);
}

void setAllMotors(int16_t fl, int16_t fr, int16_t rl, int16_t rr)
{
    motorFL.run(fl);
    motorFR.run(fr);
    motorRL.run(rl);
    motorRR.run(rr);
}

void stopAllMotors()
{
    motorFL.stop();
    motorFR.stop();
    motorRL.stop();
    motorRR.stop();
}

void setup()
{
    Serial.begin(115200);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    stopAllMotors();

    // Startup indication chime
    beep(60, 1200);
    delay(60);
    beep(80, 1800);
}

void loop()
{
    gamepad.ScanForInput();

    // Toggle motor drive enable on Start button
    if (gamepad.buttonPressed(buttonStart))
    {
        motorStarted = !motorStarted;
        if (motorStarted)
        {
            beep(100, 2000); // High pitch: enabled
        }
        else
        {
            beep(120, 800);  // Low pitch: disabled
            stopAllMotors();
        }
    }

    if (motorStarted)
    {
        int16_t lx = gamepad.joystickData(LX);
        int16_t ly = gamepad.joystickData(LY);
        int16_t rx = gamepad.joystickData(RX);
        int16_t ry = gamepad.joystickData(RY);

        // Primary drive control via Left Stick, fallback to Right Stick
        if (abs(lx) > 5 || abs(ly) > 5)
        {
            setAllMotors(
                lx + ly,
                ly - lx,
                lx + ly,
                ly - lx
            );
        }
        else if (abs(rx) > 5 || abs(ry) > 5)
        {
            setAllMotors(
                ry + rx,
                ry - rx,
                ry - rx,
                ry + rx
            );
        }
        else
        {
            stopAllMotors();
        }
    }
    else
    {
        stopAllMotors();
    }

    delay(20);
}
