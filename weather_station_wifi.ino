/*
  WiFi Web Server
*/

#include <SPI.h>
#include <WiFi101.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// please enter your sensitive data in the Secret tab/arduino_secrets.h
#include "arduino_secrets.h"


#define BME_SCK  30
#define BME_MISO 32
#define BME_MOSI 34
#define BME_CS   36

#define SEALEVELPRESSURE_HPA (1013.25)
#define ALTITUDE 186 // altitude of Murfreesboro in meters

float temperature;
float humidity;
float pressure;
float dewpoint;
float td;
float temp;

bool bmestatus;
int t;

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

char ssid[] = SECRET_SSID;    // network SSID (name)
char pass[] = SECRET_PASS;    // network password (use for WPA, or use as key for WEP)

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

  bmestatus = bme.begin();
  if (!bmestatus) {
    Serial.println("Error. Check BME280 connections");
    while (1);
  }

  // give the bme280 time to initialize, otherwise the webserver will wedge
  // as it tries to read the sensors.
 
  Serial.println("Pausing for BME...");

  while (t < 10) {
    delay (1000);
    Serial.print(String(t) + "...");
    t = t + 1;
  }
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

          getPressure();
          getHumidity();
          getTemperature();
          getDewPoint();
          
          // this will probably be edited slightly to return JSON

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  
          client.println("Refresh: 60");   
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("Temperature: " + String(temperature, 1) + "&deg; F <br>");
          client.println("Relative Humidity: " + String(humidity, 0) + "%<br>");
          client.println("Barometer: " + String(pressure, 2) + "mb<br>");
          client.println("Dew Point: " + String(dewpoint, 1) + "&deg; F<br>");
          
          //temp, relative humidity, barometer, dewpoint
          client.println(" "  + String(temperature, 1) + "|" + String(humidity, 0) + "|" + String(pressure, 2) + "|" + String(dewpoint, 1) +"<br>");
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
    delay(1000);

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

float getTemperature()
{
  // temp seems to be consistenly 5 degrees F too high, so making a dirty adjustment here.
  temperature = bme.readTemperature() * 9 / 5 + 32 - 5;
}

float getHumidity()
{
  humidity = bme.readHumidity();
}

float getPressure()
{
  pressure = bme.readPressure();
  pressure = bme.seaLevelForAltitude(ALTITUDE, pressure);
  pressure = pressure / 100.0F;
}

float getDewPoint()
{
  temp = bme.readTemperature();
  humidity = bme.readHumidity();

  // Here follows the August-Roche-Magnus approximation of dew point:
  // 243.04*(LN(RH/100)+((17.625*T)/(243.04+T)))/(17.625-LN(RH/100)-((17.625*T)/(243.04+T)))
  // (via http://andrew.rsmas.miami.edu/bmcnoldy/Humidity.html)
  // if you think this is gnarly, you should see the formula for calculating heat indices
  // note that the inputs for this formula should be in Celsius

  td = 243.04 * (log(humidity / 100) + ((17.625 * temp) / (243.04 + temp))) / (17.625 - log(humidity / 100) - ((17.625 * temp) / (243.04 + temp)));
  dewpoint = td * 9 / 5 + 32 - 5;

}
