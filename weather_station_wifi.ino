#include <Adafruit_BME280.h>

/*
  WiFi Web Server -
  Another stage of the work-in-progress. Some BME280 sensor readings grafted into the
  sample WiFi Web Server that comes with the WIFI101 library examples. Nothing spectacular
  here.
  
 */

#include <SPI.h>
#include <WiFi101.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h


#define BME_SCK  30
#define BME_MISO 32
#define BME_MOSI 34
#define BME_CS   36

#define SEALEVELPRESSURE_HPA (1013.25)
#define ALTITUDE 186 // altitude of Murfreesboro in meters


Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI


char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  
  printWiFiStatus();

    bme.begin();
  
}

void loop() {
  // listen for incoming clients

 
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    
         // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    
    
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
          client.println("Temperature: " + String(bme.readTemperature() * 9/5+32,1) +"&deg; F <br>");
          client.println("Relative Humidity: " + String(bme.readHumidity(),1) +"%<br>");
          client.println("Barometer: " + String(bme.readPressure()/100.0F,1) +"mb<br>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
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

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


