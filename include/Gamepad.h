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
 * @brief Bit-banged SPI driver for wireless/wired PS2 Gamepad controllers.
 *        Features a fused 9-byte SPI transaction pipeline, pre-normalized joystick axes,
 *        and bitmask-based active-LOW button decoding.
 */
class Gamepad {
private:
    u8 MOSI;
    u8 MISO;
    u8 CS;
    u8 SCK;

    // Active-HIGH 16-bit bitfield of currently pressed buttons (bit 0 = button 1, etc.)
    u16 buttonsDown = 0;

    // Pre-normalized joystick values (-100 to +100)
    i16 rx = 0;
    i16 ry = 0;
    i16 lx = 0;
    i16 ly = 0;

    // Receiver controller ID (0x73 = Analog Red Mode, 0x41 = Digital Mode)
    u8 controllerMode = 0;

    // Non-blocking debounce timestamps per button (1-indexed, 1..16)
    u32 lastPressed[17] = {0};

    // Full-duplex bit-banged SPI byte transfer (LSB first, Mode 3)
    u8 transferByte(u8 cmd);

public:
    Gamepad(u8 MOSIpin, u8 MISOpin, u8 CSpin, u8 SCKpin);

    /**
     * @brief Polls the PS2 controller through a single 9-byte SPI exchange:
     *        - Bytes 0-2: Handshake & mode detection
     *        - Bytes 3-4: Button state acquisition & active-LOW inversion
     *        - Bytes 5-8: Direct reading and normalization of analog joysticks
     */
    void ScanForInput();

    inline void scanForInput() { ScanForInput(); }

    /**
     * @brief Returns pre-calculated joystick value in the range [-100, +100].
     * @param axis Target joystick channel (LX, LY, RX, RY).
     */
    i16 joystickData(GamepadAxis axis) const;
    i16 joystickData(u8 axis) const;

    /**
     * @brief Checks instantaneous button state (true = down, false = released).
     */
    bool isButtonDown(GamepadButton button) const;
    bool isButtonDown(u8 button) const;

    /**
     * @brief Checks if a button is pressed with non-blocking 200ms debounce.
     */
    bool buttonPressed(GamepadButton button);
    bool buttonPressed(u8 button);

    // Direct accessors for normalized axes (-100 to +100)
    inline i16 getLX() const { return lx; }
    inline i16 getLY() const { return ly; }
    inline i16 getRX() const { return rx; }
    inline i16 getRY() const { return ry; }

    // Direct accessor for controller mode
    inline u8 getControllerMode() const { return controllerMode; }
};
