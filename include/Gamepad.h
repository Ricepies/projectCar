#pragma once

#include <Arduino.h>
#include <stdint.h>

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
 * @brief Bit-banged SPI driver for wireless/wired PS2 Gamepad controllers.
 *        Features a fused 9-byte SPI transaction pipeline, pre-normalized joystick axes,
 *        and bitmask-based active-LOW button decoding.
 */
class Gamepad {
private:
    uint8_t MOSI;
    uint8_t MISO;
    uint8_t CS;
    uint8_t SCK;

    // Active-HIGH 16-bit bitfield of currently pressed buttons (bit 0 = button 1, etc.)
    uint16_t buttonsDown = 0;

    // Pre-normalized joystick values (-100 to +100)
    int16_t rx = 0;
    int16_t ry = 0;
    int16_t lx = 0;
    int16_t ly = 0;

    // Receiver controller ID (0x73 = Analog Red Mode, 0x41 = Digital Mode)
    uint8_t controllerMode = 0;

    // Non-blocking debounce timestamps per button (1-indexed, 1..16)
    uint32_t lastPressed[17] = {0};

    // Full-duplex bit-banged SPI byte transfer (LSB first, Mode 3)
    uint8_t transferByte(uint8_t cmd);

public:
    Gamepad(uint8_t MOSIpin, uint8_t MISOpin, uint8_t CSpin, uint8_t SCKpin);

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
    int16_t joystickData(GamepadAxis axis) const;
    int16_t joystickData(uint8_t axis) const;

    /**
     * @brief Checks instantaneous button state (true = down, false = released).
     */
    bool isButtonDown(GamepadButton button) const;
    bool isButtonDown(uint8_t button) const;

    /**
     * @brief Checks if a button is pressed with non-blocking 200ms debounce.
     */
    bool buttonPressed(GamepadButton button);
    bool buttonPressed(uint8_t button);

    // Direct accessors for normalized axes (-100 to +100)
    inline int16_t getLX() const { return lx; }
    inline int16_t getLY() const { return ly; }
    inline int16_t getRX() const { return rx; }
    inline int16_t getRY() const { return ry; }

    // Direct accessor for controller mode
    inline uint8_t getControllerMode() const { return controllerMode; }
};
