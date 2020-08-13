#include "ComChannel.h"
#include <Arduino.h>

#if USE_WIFI_CONNECTION

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#else
#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#endif
#endif

#define DNS_NAME 		"mudpi"
#define SERVER_PORT 	5678
#define SerialDebug Serial
#define TIMEOUT 50

WiFiServer server(SERVER_PORT);
WiFiClient client;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 10000;

void nanpy::ComChannel::connect()
{
	SerialDebug.begin(115200);
	SerialDebug.println("");
  WiFi.disconnect(); //Flush WiFi
  delay(2000);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	SerialDebug.print("\nConnecting to ");
	SerialDebug.println(WIFI_SSID);
	uint8_t i = 0;
	while (WiFi.status() != WL_CONNECTED && i++ < TIMEOUT)
	{
		delay(1000);
		Serial.print(".");
	}
	if(i > TIMEOUT)
	{
		SerialDebug.println("");
		SerialDebug.print("Could not connect to ");
		SerialDebug.println(WIFI_SSID);
		while(1)
		{
			delay(500);
		}
	}

	SerialDebug.println("");

	if (MDNS.begin(DNS_NAME))
	{
		SerialDebug.println("MDNS responder started");
		SerialDebug.print("ping ");
		SerialDebug.print(DNS_NAME);
		SerialDebug.println(".local");
	}

	SerialDebug.println("");

	//start UART and the server
	server.begin();
	server.setNoDelay(true);

	SerialDebug.print("ping ");
	SerialDebug.println(WiFi.localIP());

	SerialDebug.print("port:");
	SerialDebug.println(SERVER_PORT);
}

bool nanpy::ComChannel::available()
{
  currentTime = millis();
  if (previousTime == 0) {
    previousTime = currentTime;
  }
  if(client) {
    previousTime = currentTime;
    // read data from the connected client
    if (client.connected() && currentTime - previousTime <= timeoutTime) {
      previousTime = currentTime;
      return (client.available() > 0);
    }
    else {
      client.stop();
      Serial.println("Client Disconnected.");
    }
  }
	else
	{
		// try to connect to a new client
    client = server.available();
    if (client.connected()) {
      SerialDebug.println("Client connected");
    }
	}
	return false;
}

char nanpy::ComChannel::read_buffer[MAX_READ_BUFFER_SIZE] = {0};

void readLineFromSerial(char* extbuff) {
    if(client) {
      if(client.connected()) {
        int i=0;
        char ch = '0';
        char* buff = nanpy::ComChannel::read_buffer;
        do {
            ch = client.read();
            if(ch < 255 && ch >= 0) {
                buff[i++] = ch;
            }
            if (i == MAX_READ_BUFFER_SIZE - 1) {
                buff[i] = '\0';
                break;
            }
        } while(ch != '\0');
        strcpy(extbuff, buff);
      }
    }
};

void nanpy::ComChannel::readLine(char* extbuff) {
    readLineFromSerial(extbuff);
}

void nanpy::ComChannel::println(String& val) {
	client.println(val);
}

void nanpy::ComChannel::println(const char* val) {
	client.println(val);
}

void nanpy::ComChannel::println(int val) {
	client.println(val);
}

void nanpy::ComChannel::println(unsigned int val) {
	client.println(val);
}

void nanpy::ComChannel::println(float val) {
	client.println(val);
}

void nanpy::ComChannel::println(double val) {
	client.println(val);
}

void nanpy::ComChannel::println(long val) {
	client.println(val);
}

void nanpy::ComChannel::println(unsigned long val) {
	client.println(val);
}


#else
char nanpy::ComChannel::read_buffer[MAX_READ_BUFFER_SIZE] = {0};

void readLineFromSerial(char* extbuff) {
    int i=0;
    char ch = '0';
    char* buff = nanpy::ComChannel::read_buffer;
    do {
        ch = COMM_SERIAL.read();
        if(ch < 255 && ch >= 0) {
            buff[i++] = ch;
        }
        if (i == MAX_READ_BUFFER_SIZE - 1) {
            buff[i] = '\0';
            break;
        }
    } while(ch != '\0');
    strcpy(extbuff, buff);
};

bool nanpy::ComChannel::available() {
    if (COMM_SERIAL.available() > 0)
        return true;
    else
        return false;
}

void nanpy::ComChannel::connect() {
    COMM_SERIAL.begin(BAUDRATE);
}

void nanpy::ComChannel::println(String& val) {
    COMM_SERIAL.println(val);
}

void nanpy::ComChannel::println(const char* val) {
    COMM_SERIAL.println(val);
}

void nanpy::ComChannel::println(int val) {
    COMM_SERIAL.println(val);
}

void nanpy::ComChannel::println(unsigned int val) {
    COMM_SERIAL.println(val);
}

void nanpy::ComChannel::println(float val) {
    COMM_SERIAL.println(val);
}

void nanpy::ComChannel::println(double val) {
    COMM_SERIAL.println(val);
}

void nanpy::ComChannel::println(long val) {
    COMM_SERIAL.println(val);
}

void nanpy::ComChannel::println(unsigned long val) {
    COMM_SERIAL.println(val);
}

void nanpy::ComChannel::readLine(char* extbuff) {
    readLineFromSerial(extbuff);
}

#endif
