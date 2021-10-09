#include "OIWire.h"

// OIWire::OIWire(uint8_t bus_num)
// {

// }

// OIWire::~OIWire()
// {

// }

// bool OIWire::setPins(int sdaPin, int sclPin)
// {

// }

// bool OIWire::begin(int sdaPin, int sclPin, uint32_t frequency)
// {

// }

// void OIWire::setClock(uint32_t frequency)
// {

// }

// size_t OIWire::getClock()
// {

// }

// void OIWire::beginTransmission(uint16_t address)
// {

// }

// uint8_t OIWire::endTransmission(bool sendStop)

// }

// uint8_t OIWire::requestFrom(uint16_t address, uint8_t size, bool sendStop)
// {

// }

// size_t OIWire::write(uint8_t data)
// {

// }

// size_t OIWire::write(const uint8_t *data, size_t quantity)
// {

// }

// int OIWire::available(void)
// {

// }

// int OIWire::read(void)
// {

// }

// int OIWire::peek(void)
// {

// }

// void OIWire::flush(void)
// {

// }

// uint8_t OIWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
// {
//     return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop));
// }

// uint8_t OIWire::requestFrom(uint16_t address, uint8_t quantity, uint8_t sendStop)
// {
//     return requestFrom(address, static_cast<size_t>(quantity), static_cast<bool>(sendStop));
// }

// uint8_t OIWire::requestFrom(uint8_t address, uint8_t quantity)
// {
//     return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), true);
// }

// uint8_t OIWire::requestFrom(uint16_t address, uint8_t quantity)
// {
//     return requestFrom(address, static_cast<size_t>(quantity), true);
// }

// uint8_t OIWire::requestFrom(int address, int quantity)
// {
//     return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), true);
// }

// uint8_t OIWire::requestFrom(int address, int quantity, int sendStop)
// {
//     return static_cast<uint8_t>(requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop)));
// }

// void OIWire::beginTransmission(int address)
// {
//     beginTransmission(static_cast<uint16_t>(address));
// }

// void OIWire::beginTransmission(uint8_t address)
// {
//     beginTransmission(static_cast<uint16_t>(address));
// }

// uint8_t OIWire::endTransmission(void)
// {
//     return endTransmission(true);
// }

// OIWire Wire = OIWire;