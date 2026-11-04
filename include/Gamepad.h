#pragma once

#include <Arduino.h>
#include "Types.h"

// PS2 Gamepad Button Indices (1-based, matching bit positions 0..15 in the 16-bit button word)
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

// PS2 Joystick Analog Channel Indices (matching byte offsets 5..8 in the 9-byte SPI response)
enum GamepadAxis : u8 { RX = 5, RY = 6, LX = 7, LY = 8 };

/**
 * @class Gamepad
 * @brief High-performance bit-banged SPI driver for wireless/wired PS2 Gamepad controllers.
 *        Features compile-time LUT axis normalization, zero-branch O(1) axis/button queries,
 *        and bitmask-based active-LOW button decoding.
 */
class Gamepad {
private:
    u8 MOSI;
    u8 MISO;
    u8 CS;
    u8 SCK;

    // Active-HIGH 16-bit bitfield of currently pressed buttons (bit 0 = buttonSelect, etc.)
    u16 buttonsDown = 0;

    // Pre-normalized joystick values (-100 to +100), indexed by (axis - RX): [RX, RY, LX, LY]
    i8 axes[4] = {0, 0, 0, 0};

    // Receiver controller ID (0x73 = Analog Red Mode, 0x41 = Digital Mode)
    u8 controllerMode = 0;

    // Non-blocking debounce timestamps per button (0-indexed: button - 1)
    u32 lastPressed[16] = {0};

    // Full-duplex bit-banged SPI byte transfer (LSB first, Mode 3)
    u8 transferByte(u8 cmd) const;

public:
    Gamepad(u8 MOSIpin, u8 MISOpin, u8 CSpin, u8 SCKpin);

    /**
     * @brief Polls the PS2 controller through a 9-byte SPI exchange:
     *        - Bytes 0-2: Handshake & mode detection
     *        - Bytes 3-4: Button state acquisition & active-LOW inversion
     *        - Bytes 5-8: Direct reading and LUT normalization of analog joysticks
     */
    void ScanForInput();
    void scanForInput();

    /**
     * @brief Returns pre-calculated joystick value in the range [-100, +100].
     * @param axis Target joystick channel (LX, LY, RX, RY).
     */
    i16 joystickData(GamepadAxis axis) const;
    i16 joystickData(u8 axis) const;

    // Direct O(1) accessors for normalized axes (-100 to +100)
    i16 getLX() const;
    i16 getLY() const;
    i16 getRX() const;
    i16 getRY() const;

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

    // Direct accessor for controller mode
    u8 getControllerMode() const;
};
