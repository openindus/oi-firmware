#include "OpenIndus.h"
#include "Arduino.h"

/* Instanciate */
OIDiscrete discrete;

/* Callbacks: Print and detach interrupt */
void callback1(void*) { Serial.println("Interrupt on DIN_1 !"); discrete.detachInterrupt(DIN_1); }
void callback2(void*) { Serial.println("Interrupt on DIN_2 !"); discrete.detachInterrupt(DIN_2); }
void callback3(void*) { Serial.println("Interrupt on DIN_3 !"); discrete.detachInterrupt(DIN_3); }
void callback4(void*) { Serial.println("Interrupt on DIN_4 !"); discrete.detachInterrupt(DIN_4); }
void callback5(void*) { Serial.println("Interrupt on DIN_5 !"); discrete.detachInterrupt(DIN_5); }
void callback6(void*) { Serial.println("Interrupt on DIN_6 !"); discrete.detachInterrupt(DIN_6); }
void callback7(void*) { Serial.println("Interrupt on DIN_7 !"); discrete.detachInterrupt(DIN_7); }
void callback8(void*) { Serial.println("Interrupt on DIN_8 !"); discrete.detachInterrupt(DIN_8); }
void callback9(void*) { Serial.println("Interrupt on DIN_9 !"); discrete.detachInterrupt(DIN_9); }
void callback10(void*) { Serial.println("Interrupt on DIN_10 !"); discrete.detachInterrupt(DIN_10); }

void setup(void)
{
    /* Attach interrupt*/
    discrete.attachInterrupt(DIN_1, callback1, RISING_MODE);
    discrete.attachInterrupt(DIN_2, callback2, RISING_MODE);
    discrete.attachInterrupt(DIN_3, callback3, RISING_MODE);
    discrete.attachInterrupt(DIN_4, callback4, RISING_MODE);
    discrete.attachInterrupt(DIN_5, callback5, RISING_MODE);
    discrete.attachInterrupt(DIN_6, callback6, RISING_MODE);
    discrete.attachInterrupt(DIN_7, callback7, RISING_MODE);
    discrete.attachInterrupt(DIN_8, callback8, RISING_MODE);
    discrete.attachInterrupt(DIN_9, callback9, RISING_MODE);
    discrete.attachInterrupt(DIN_10, callback10, RISING_MODE);
}

void loop(void)
{
    delay(1000);
}