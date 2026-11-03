#pragma once

#include <Arduino.h>

// PS2 Gamepad Button Indices
#define buttonSelect   1
#define buttonL3       2
#define buttonR3       3
#define buttonStart    4
#define buttonUp       5
#define buttonRight    6
#define buttonDown     7
#define buttonLeft     8
#define buttonL2       9
#define buttonR2       10
#define buttonL1       11
#define buttonR1       12
#define buttonTriangle 13
#define buttonCircle   14
#define buttonX        15
#define buttonSquare   16

// PS2 Joystick Analog Channel Indices
#define RX 5
#define RY 6
#define LX 7
#define LY 8

/**
 * @class Gamepad
 * @brief Bit-bang SPI communication interface for wireless/wired PS2 Gamepad controller.
 */
class Gamepad
{
private:
    uint8_t MOSI;
    uint8_t MISO;
    uint8_t CS;
    uint8_t SCK;

    const uint16_t MASK[16] = {
        buttonSelect,
        buttonL3,
        buttonR3,
        buttonStart,
        buttonUp,
        buttonRight,
        buttonDown,
        buttonLeft,
        buttonL2,
        buttonR2,
        buttonL1,
        buttonR1,
        buttonTriangle,
        buttonCircle,
        buttonX,
        buttonSquare
    };

    uint16_t Handkey = 0;
    uint8_t scan[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t Data[9] = {0};

    uint32_t lastPressed[19] = {0};
    bool dynamicButtonStatus[19] = {false};

    uint8_t ReadData(uint8_t command);
    void ClearData();

public:
    Gamepad(uint8_t MOSIpin, uint8_t MISOpin, uint8_t CSpin, uint8_t SCKpin);
    void ScanForInput();
    int16_t joystickData(uint8_t axis);
    bool buttonPressed(uint8_t button);
};
