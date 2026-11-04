#include "Gamepad.h"

namespace {

constexpr u8 kStartCmd = 0x01;
constexpr u8 kPollCmd = 0x42;
constexpr u32 kDebounceMs = 200;

// Dynamic axis normalization: maps raw 0..255 byte to centered -100..+100.
inline i8 normalizeAxis(u8 val, bool invert = false) {
    const i16 raw = invert ? static_cast<i16>(255U - val) : static_cast<i16>(val);
    return static_cast<i8>((raw - 128) * 100 / 127);
}

} // namespace

Gamepad::Gamepad(u8 MOSIpin, u8 MISOpin, u8 CSpin, u8 SCKpin) : MOSI(MOSIpin), MISO(MISOpin), CS(CSpin), SCK(SCKpin) {
    pinMode(MISO, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, INPUT);
    pinMode(CS, OUTPUT);

    digitalWrite(SCK, HIGH);
    digitalWrite(CS, HIGH);
    digitalWrite(MISO, HIGH);
}

u8 Gamepad::transferByte(u8 cmd) const {
    u8 in = 0;

    for (u8 bit = 0; bit < 8; ++bit) {
        digitalWrite(MISO, (cmd & 0x01U) ? HIGH : LOW);
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

    // Bytes 0-2: Header handshake & mode detection
    (void)transferByte(kStartCmd);
    controllerMode = transferByte(kPollCmd);
    (void)transferByte(0x00);

    // Bytes 3-4: 16 digital buttons (active-LOW on wire -> invert to active-HIGH bitfield)
    const u8 btnLow = transferByte(0x00);
    const u8 btnHigh = transferByte(0x00);
    buttonsDown = static_cast<u16>(~(static_cast<u16>(btnLow) | (static_cast<u16>(btnHigh) << 8)));

    // Bytes 5-8: Analog sticks (RX, RY, LX, LY). Y axes are inverted so forward is +100.
    axes[0] = normalizeAxis(transferByte(0x00), false);
    axes[1] = normalizeAxis(transferByte(0x00), true);
    axes[2] = normalizeAxis(transferByte(0x00), false);
    axes[3] = normalizeAxis(transferByte(0x00), true);

    digitalWrite(CS, HIGH);
}

void Gamepad::scanForInput() {
    ScanForInput();
}

i16 Gamepad::joystickData(GamepadAxis axis) const {
    return joystickData(static_cast<u8>(axis));
}

i16 Gamepad::joystickData(u8 axis) const {
    const u8 idx = static_cast<u8>(axis - RX);
    return (idx < 4) ? static_cast<i16>(axes[idx]) : 0;
}

bool Gamepad::isButtonDown(GamepadButton button) const {
    return isButtonDown(static_cast<u8>(button));
}

bool Gamepad::isButtonDown(u8 button) const {
    const u8 idx = static_cast<u8>(button - 1);
    return (idx < 16) && (((buttonsDown >> idx) & 1U) != 0);
}

bool Gamepad::buttonPressed(GamepadButton button) {
    return buttonPressed(static_cast<u8>(button));
}

bool Gamepad::buttonPressed(u8 button) {
    const u8 idx = static_cast<u8>(button - 1);
    if (idx >= 16 || ((buttonsDown >> idx) & 1U) == 0) {
        return false;
    }

    const u32 now = millis();
    if (now - lastPressed[idx] > kDebounceMs) {
        lastPressed[idx] = now;
        return true;
    }
    return false;
}

u8 Gamepad::getControllerMode() const {
    return controllerMode;
}
