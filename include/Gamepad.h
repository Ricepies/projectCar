#pragma once

#include <Arduino.h>

// PS2 Gamepad Button Indices
enum GamepadButton : uint8_t {
    buttonSelect = 1,
    buttonL3 = 2,
    buttonR3 = 3,
    buttonStart = 4,
    buttonUp = 5,
    buttonRight = 6,
    buttonDown = 7,
    buttonLeft = 8,
    buttonL2 = 9,
    buttonR2 = 10,
    buttonL1 = 11,
    buttonR1 = 12,
    buttonTriangle = 13,
    buttonCircle = 14,
    buttonX = 15,
    buttonSquare = 16
};

// PS2 Joystick Analog Channel Indices
enum GamepadAxis : uint8_t { RX = 5, RY = 6, LX = 7, LY = 8 };

/**
 * @class Gamepad
 * @brief Bit-bang SPI communication interface for wireless/wired PS2 Gamepad controller.
 */
class Gamepad {
private:
    uint8_t MOSI;
    uint8_t MISO;
    uint8_t CS;
    uint8_t SCK;

    const uint16_t MASK[16] = {buttonSelect,   buttonL3,     buttonR3, buttonStart, buttonUp, buttonRight,
                               buttonDown,     buttonLeft,   buttonL2, buttonR2,    buttonL1, buttonR1,
                               buttonTriangle, buttonCircle, buttonX,  buttonSquare};

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
    int16_t joystickData(GamepadAxis axis);
    int16_t joystickData(uint8_t axis);
    bool buttonPressed(GamepadButton button);
    bool buttonPressed(uint8_t button);
};
