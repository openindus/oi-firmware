#include "OpenIndus.h"
#include "Arduino.h"

void setup(void)
{
    
}

void loop(void)
{
    delay(1000);
}



// #include "OpenIndus.h"
// #include "Arduino.h"
// #include "RS.h"

// uint8_t buff[100];

// void setup()
// {
//     RS485.begin();
// }

// void loop()
// {
//     RS485.write("Hello OpenIndus\r\n");
//     delay(1000);
//     int ret = RS485.available();
//     printf("%i bytes available to read\n", ret);
//     if (ret) {
//         RS485.read(buff, 100);
//         printf("%s", buff);
//     }
// }