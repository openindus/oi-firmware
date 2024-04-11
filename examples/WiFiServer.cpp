#include "OpenIndus.h"
#include <Arduino.h>
#include <WiFi.h>

char ssid[] = "yourSSID";       // your network SSID (name)
char pass[] = "yourPasword";    // your network password
int keyIndex = 0;               // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup(void)
{
    Serial.begin(115200);

    // connect to WPA/WPA2 network
    WiFi.begin(ssid, pass);

    // attempt to connect to Wifi network
    do {
        Serial.print(".");
        status = WiFi.status();
        // wait 5 seconds for connection
        delay(5000);
    } while (status != WL_CONNECTED);

    Serial.print("Connected !");

    server.begin();

    // print the SSID of the network
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

void loop(void) 
{
    // listen for incoming clients
    WiFiClient client = server.available();

    if (client) {
        Serial.println("new client");

        // an http request ends with a blank line
        bool currentLineIsBlank = true;

        while (client.connected()) {
            
            if (client.available()) {
                char c = client.read();
                Serial.write(c);

                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");  // the connection will be closed after completion of the response
                    client.println("Refresh: 5");  // refresh the page automatically every 5 sec
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");

                    // output the value of each analog input pin
                    for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
                        int sensorReading = analogRead(analogChannel);
                        client.print("analog input ");
                        client.print(analogChannel);
                        client.print(" is ");
                        client.print(sensorReading);
                        client.println("<br />");
                    }

                    client.println("</html>");
                    break;
                }

                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }

        // give the web browser time to receive the data
        delay(1);

        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}