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
 * @brief Standalone, single-file bit-banged SPI driver for PS2 Gamepad controllers.
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

    // Helper: normalizes raw 0..255 byte to centered -100..+100 range
    static inline int16_t normalizeAxis(uint8_t val, bool invert = false) {
        int16_t raw = invert ? (255 - static_cast<int16_t>(val)) : static_cast<int16_t>(val);
        int16_t scaled = (raw - 128) * 100 / 127;
        if (scaled > 100) {
            return 100;
        }
        if (scaled < -100) {
            return -100;
        }
        return scaled;
    }

    // Full-duplex bit-banged SPI byte transfer (LSB first, Mode 3)
    inline uint8_t transferByte(uint8_t cmd) {
        uint8_t in = 0;
        for (uint8_t bit = 0; bit < 8; ++bit) {
            digitalWrite(MISO, (cmd & 0x01) ? HIGH : LOW);
            cmd >>= 1;

            delayMicroseconds(10);
            digitalWrite(SCK, LOW);
            delayMicroseconds(10);

            if (digitalRead(MOSI) == HIGH) {
                in |= static_cast<uint8_t>(1U << bit);
            }

            digitalWrite(SCK, HIGH);
            delayMicroseconds(10);
        }
        digitalWrite(MISO, HIGH);
        delayMicroseconds(50);
        return in;
    }

public:
    Gamepad(uint8_t MOSIpin, uint8_t MISOpin, uint8_t CSpin, uint8_t SCKpin)
        : MOSI(MOSIpin), MISO(MISOpin), CS(CSpin), SCK(SCKpin) {
        pinMode(MISO, OUTPUT);
        pinMode(SCK, OUTPUT);
        pinMode(MOSI, INPUT);
        pinMode(CS, OUTPUT);

        digitalWrite(SCK, HIGH);
        digitalWrite(CS, HIGH);
        digitalWrite(MISO, HIGH);
    }

    /**
     * @brief Polls the PS2 controller through a single 9-byte SPI exchange:
     *        - Bytes 0-2: Handshake & mode detection
     *        - Bytes 3-4: Button state acquisition & active-LOW inversion
     *        - Bytes 5-8: Direct reading and normalization of analog joysticks
     */
    inline void ScanForInput() {
        digitalWrite(CS, LOW);

        uint8_t rawBtnLow = 0xFF;
        uint8_t rawBtnHigh = 0xFF;

        // Fused 9-byte SPI pipeline
        for (uint8_t byteIdx = 0; byteIdx < 9; ++byteIdx) {
            uint8_t cmd = (byteIdx == 0) ? 0x01 : ((byteIdx == 1) ? 0x42 : 0x00);
            uint8_t incoming = transferByte(cmd);

            switch (byteIdx) {
                // Bytes 0-2: Handshake & mode detection
                case 0:
                    break;
                case 1:
                    controllerMode = incoming;
                    break;
                case 2:
                    break;

                // Bytes 3-4: Digital buttons (active-LOW)
                case 3:
                    rawBtnLow = incoming;
                    break;
                case 4: {
                    rawBtnHigh = incoming;
                    uint16_t raw = static_cast<uint16_t>(rawBtnLow) | (static_cast<uint16_t>(rawBtnHigh) << 8);
                    buttonsDown = ~raw; // Invert active-LOW: 1 = pressed, 0 = released
                    break;
                }

                // Bytes 5-8: Analog joysticks (direct normalization)
                case 5:
                    rx = normalizeAxis(incoming, false);
                    break;
                case 6:
                    ry = normalizeAxis(incoming, true); // Y inverted so forward is +100
                    break;
                case 7:
                    lx = normalizeAxis(incoming, false);
                    break;
                case 8:
                    ly = normalizeAxis(incoming, true); // Y inverted so forward is +100
                    break;
            }
        }

        digitalWrite(CS, HIGH);
    }

    inline void scanForInput() { ScanForInput(); }

    /**
     * @brief Returns pre-calculated joystick value in the range [-100, +100].
     * @param axis Target joystick channel (LX, LY, RX, RY).
     */
    inline int16_t joystickData(GamepadAxis axis) const { return joystickData(static_cast<uint8_t>(axis)); }

    inline int16_t joystickData(uint8_t axis) const {
        switch (axis) {
            case LX:
                return lx;
            case LY:
                return ly;
            case RX:
                return rx;
            case RY:
                return ry;
            default:
                return 0;
        }
    }

    /**
     * @brief Checks instantaneous button state (true = down, false = released).
     */
    inline bool isButtonDown(GamepadButton button) const { return isButtonDown(static_cast<uint8_t>(button)); }

    inline bool isButtonDown(uint8_t button) const {
        if (button < 1 || button > 16) {
            return false;
        }
        return (buttonsDown & (1U << (button - 1))) != 0;
    }

    /**
     * @brief Checks if a button is pressed with non-blocking 200ms debounce.
     */
    inline bool buttonPressed(GamepadButton button) { return buttonPressed(static_cast<uint8_t>(button)); }

    inline bool buttonPressed(uint8_t button) {
        if (button < 1 || button > 16) {
            return false;
        }
        bool down = (buttonsDown & (1U << (button - 1))) != 0;
        if (down && (millis() - lastPressed[button] > 200)) {
            lastPressed[button] = millis();
            return true;
        }
        return false;
    }

    // Direct accessors for normalized axes (-100 to +100)
    inline int16_t getLX() const { return lx; }
    inline int16_t getLY() const { return ly; }
    inline int16_t getRX() const { return rx; }
    inline int16_t getRY() const { return ry; }

    // Direct accessor for controller mode
    inline uint8_t getControllerMode() const { return controllerMode; }
};
