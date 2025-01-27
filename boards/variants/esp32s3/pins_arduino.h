#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

#define USB_VID 0x303a
#define USB_PID 0x1001

#define EXTERNAL_NUM_INTERRUPTS 46
#define NUM_DIGITAL_PINS        0
#define NUM_ANALOG_INPUTS       2

// Some boards have too low voltage on this pin (board design bug)
// Use different pin with 3V and connect with 48
// and change this setup for the chosen pin (for example 38)
static const uint8_t LED_BUILTIN = SOC_GPIO_PIN_COUNT+48;
#define BUILTIN_LED  LED_BUILTIN // backward compatibility
#define LED_BUILTIN LED_BUILTIN
#define RGB_BUILTIN LED_BUILTIN
#define RGB_BRIGHTNESS 64

#define analogInputToDigitalPin(p)  (((p)<20)?(analogChannelToDigitalPin(p)):-1)
#define digitalPinToInterrupt(p)    (((p)<48)?(p):-1)
#define digitalPinHasPWM(p)         (p < 46)

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t TX2 = 36;
static const uint8_t RX2 = 37;

static const uint8_t SDA = 36;
static const uint8_t SCL = 37;

static const uint8_t SS    = 18;
static const uint8_t MOSI  = 15;
static const uint8_t MISO  = 16;
static const uint8_t SCK   = 17;

static const uint8_t ETHERNET_SS    = 18;
static const uint8_t SD_SS          = 47;
static const uint8_t USBHOST_SS     = 48;

// static const uint8_t A0 = 1;
// static const uint8_t A1 = 2;

#endif /* Pins_Arduino_h */
