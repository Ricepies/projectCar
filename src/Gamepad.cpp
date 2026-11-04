#include "Gamepad.h"

// Helper: normalizes raw 0..255 byte to centered -100..+100 range
static inline i16 normalizeAxis(u8 val, bool invert = false) {
    i16 raw = invert ? (255 - static_cast<i16>(val)) : static_cast<i16>(val);
    i16 scaled = (raw - 128) * 100 / 127;
    if (scaled > 100) {
        return 100;
    }
    if (scaled < -100) {
        return -100;
    }
    return scaled;
}

Gamepad::Gamepad(u8 MOSIpin, u8 MISOpin, u8 CSpin, u8 SCKpin) : MOSI(MOSIpin), MISO(MISOpin), CS(CSpin), SCK(SCKpin) {
    pinMode(MISO, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, INPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(SCK, HIGH);
    digitalWrite(CS, HIGH);
    digitalWrite(MISO, HIGH);
}

u8 Gamepad::transferByte(u8 cmd) {
    u8 in = 0;

    for (u8 bit = 0; bit < 8; ++bit) {
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
            in |= static_cast<u8>(1U << bit);
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

    u8 rawBtnLow = 0xFF;
    u8 rawBtnHigh = 0xFF;

    // Fused 9-byte SPI pipeline
    for (u8 byteIdx = 0; byteIdx < 9; ++byteIdx) {
        u8 cmd = (byteIdx == 0) ? 0x01 : ((byteIdx == 1) ? 0x42 : 0x00);
        u8 incoming = transferByte(cmd);

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
                u16 raw = static_cast<u16>(rawBtnLow) | (static_cast<u16>(rawBtnHigh) << 8);
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

i16 Gamepad::joystickData(GamepadAxis axis) const {
    return joystickData(static_cast<u8>(axis));
}

i16 Gamepad::joystickData(u8 axis) const {
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
    return isButtonDown(static_cast<u8>(button));
}

bool Gamepad::isButtonDown(u8 button) const {
    if (button < 1 || button > 16) {
        return false;
    }
    return (buttonsDown & (1U << (button - 1))) != 0;
}

bool Gamepad::buttonPressed(GamepadButton button) {
    return buttonPressed(static_cast<u8>(button));
}

bool Gamepad::buttonPressed(u8 button) {
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
