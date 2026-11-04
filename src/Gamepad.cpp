#include "Gamepad.h"

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

Gamepad::Gamepad(uint8_t MOSIpin, uint8_t MISOpin, uint8_t CSpin, uint8_t SCKpin)
    : MOSI(MOSIpin), MISO(MISOpin), CS(CSpin), SCK(SCKpin) {
    pinMode(MISO, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, INPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(SCK, HIGH);
    digitalWrite(CS, HIGH);
    digitalWrite(MISO, HIGH);
}

uint8_t Gamepad::transferByte(uint8_t cmd) {
    uint8_t in = 0;

    for (uint8_t bit = 0; bit < 8; ++bit) {
        if (cmd & 0x01) {
            digitalWrite(MISO, HIGH);
        } else {
            digitalWrite(MISO, LOW);
        }
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

void Gamepad::ScanForInput() {
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

int16_t Gamepad::joystickData(GamepadAxis axis) const {
    return joystickData(static_cast<uint8_t>(axis));
}

int16_t Gamepad::joystickData(uint8_t axis) const {
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

bool Gamepad::isButtonDown(GamepadButton button) const {
    return isButtonDown(static_cast<uint8_t>(button));
}

bool Gamepad::isButtonDown(uint8_t button) const {
    if (button < 1 || button > 16) {
        return false;
    }
    return (buttonsDown & (1U << (button - 1))) != 0;
}

bool Gamepad::buttonPressed(GamepadButton button) {
    return buttonPressed(static_cast<uint8_t>(button));
}

bool Gamepad::buttonPressed(uint8_t button) {
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
