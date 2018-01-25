#include <LiquidCrystal.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK  6
#define BME_MISO 5
#define BME_MOSI 4
#define BME_CS   3

#define SEALEVELPRESSURE_HPA (1013.25)
#define ALTITUDE 186 // altitude of Murfreesboro in meters

float temperature;
float humidity;
float pressure;
float dewpoint;

//                BS  E  D4 D5  D6 D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

void setup()
{
  lcd.begin(20, 4);
  lcd.print("Reading sensor");
  bool status;

  status = bme.begin();
  if(!status) {
    lcd.clear();
    lcd.print("Error. Check");
    lcd.setCursor(0,1);
    lcd.print("connections");
    while (1);
   }
}

void loop(){

  delay(2000);  

  getPressure();
  getHumidity();
  getTemperature();
  getDewPoint();

  lcd.clear();

  // note  that my LCD is a 20x4 model, so you may need to squeeze things into 
  // something smaller. 
  
  //print temp
  String tempString = String(temperature,1);
  lcd.print("Temp : ");
  lcd.print(tempString);
  lcd.print((char)223);
  lcd.print("F ");

  //print humidity
  lcd.setCursor(0,1);
  String humidString = String(humidity,0);
  lcd.print("Humid: ");
  lcd.print(humidString);
  lcd.print("% ");


  //print pressure
  lcd.setCursor(0,2);
  lcd.print("Barom: ");
  String baromString = String(pressure,2);
  lcd.print(baromString);
  lcd.print("mb ");

  //print dew point
  lcd.setCursor(0,3);
  String dewString = String(dewpoint,1);
  lcd.print("DewPt: ");
  lcd.print(dewString);
  lcd.print((char)223);
  lcd.print("F ");
}

float getTemperature()
{
  temperature = bme.readTemperature() * 9/5 +32;
}

float getHumidity()
{
 humidity = bme.readHumidity();
}

float getPressure()
{
  pressure = bme.readPressure();
  pressure = bme.seaLevelForAltitude(ALTITUDE,pressure);
  pressure = pressure/100.0F;
}

float getDewPoint()
{
  temperature = bme.readTemperature()* 9/5 +32;
  humidity = bme.readHumidity();

  // dew point calculation
  // 243.04*(LN(RH/100)+((17.625*T)/(243.04+T)))/(17.625-LN(RH/100)-((17.625*T)/(243.04+T)))
  // via http://andrew.rsmas.miami.edu/bmcnoldy/Humidity.html

 dewpoint = 243.04*(log(humidity/100)+((17.625*temperature)/(243.04+temperature)))/(17.625-log(humidity/100)-((17.625*temperature)/(243.04+temperature)));
 
 
 }
  



