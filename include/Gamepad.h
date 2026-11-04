#pragma once

#include <Arduino.h>
#include "Types.h"

// PS2 Gamepad Button Indices
enum GamepadButton : u8 {
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
enum GamepadAxis : u8 { RX = 5, RY = 6, LX = 7, LY = 8 };

/**
 * @class Gamepad
 * @brief Bit-bang SPI communication interface for wireless/wired PS2 Gamepad controller.
 */
class Gamepad {
private:
    u8 MOSI;
    u8 MISO;
    u8 CS;
    u8 SCK;

    const u16 MASK[16] = {buttonSelect,   buttonL3,     buttonR3, buttonStart, buttonUp, buttonRight,
                          buttonDown,     buttonLeft,   buttonL2, buttonR2,    buttonL1, buttonR1,
                          buttonTriangle, buttonCircle, buttonX,  buttonSquare};

    u16 Handkey = 0;
    u8 scan[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    u8 Data[9] = {0};

    u32 lastPressed[19] = {0};
    bool dynamicButtonStatus[19] = {false};

    u8 ReadData(u8 command);
    void ClearData();

public:
    Gamepad(u8 MOSIpin, u8 MISOpin, u8 CSpin, u8 SCKpin);
    void ScanForInput();
    i16 joystickData(GamepadAxis axis);
    i16 joystickData(u8 axis);
    bool buttonPressed(GamepadButton button);
    bool buttonPressed(u8 button);
};
