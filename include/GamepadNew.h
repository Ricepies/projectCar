#pragma once

#include <Arduino.h>
#include "Gamepad.h"
#include "Types.h"

/**
 * @class GamepadNew
 * @brief Streamlined PS2 Gamepad driver featuring a single-loop fused SPI transaction pipeline,
 *        pre-normalized joystick axes, and a bitmask-based button state engine.
 *
 * Fully preserves backwards compatibility with the original Gamepad API:
 *   - ScanForInput()
 *   - joystickData(axis)
 *   - buttonPressed(button)
 */
class GamepadNew {
private:
    u8 MOSI;
    u8 MISO;
    u8 CS;
    u8 SCK;

    // Active-HIGH 16-bit bitfield of currently depressed buttons (bit 0 = button 1, etc.)
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

    // Low-level full-duplex bit-banged SPI byte transfer
    u8 transferByte(u8 cmd);

public:
    GamepadNew(u8 MOSIpin, u8 MISOpin, u8 CSpin, u8 SCKpin);

    /**
     * @brief Polls the PS2 controller through a single 9-byte SPI exchange:
     *        - Bytes 0-2: Handshake & mode detection
     *        - Bytes 3-4: Button state acquisition & active-LOW inversion
     *        - Bytes 5-8: Direct reading and normalization of analog joysticks
     */
    void ScanForInput();

    // Compatibility aliases
    inline void scanForInput() { ScanForInput(); }

    /**
     * @brief Returns pre-calculated joystick value in the range [-100, +100].
     * @param axis Target joystick channel (LX, LY, RX, RY).
     */
    i16 joystickData(GamepadAxis axis) const;
    i16 joystickData(u8 axis) const;

    /**
     * @brief Checks if a button is currently pressed with a non-blocking 200ms debounce.
     * @param button Target button index (e.g. buttonStart, buttonX).
     */
    bool buttonPressed(GamepadButton button);
    bool buttonPressed(u8 button);

    /**
     * @brief Checks continuous instantaneous button state without debounce.
     */
    bool isButtonDown(GamepadButton button) const;
    bool isButtonDown(u8 button) const;

    // Direct accessors for normalized axes (-100 to +100)
    inline i16 getLX() const { return lx; }
    inline i16 getLY() const { return ly; }
    inline i16 getRX() const { return rx; }
    inline i16 getRY() const { return ry; }

    // Direct accessor for controller mode
    inline u8 getControllerMode() const { return controllerMode; }
};
