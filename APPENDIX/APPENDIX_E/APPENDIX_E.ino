/*
*  By Alexandros Panagiotakopoulos
AM:1716/ OLD AM:16108
UNIVERSITY OF IOANNINA - DEPARTMENT OF INFORMATICS & TELECOMMUNICATIONS
*/

//Thinger.io(),source used: https://www.arduino.cc/reference/en/libraries/thinger.io/

// This Project is an original work created by Alexandros Panagiotakopoulos during the academic year 2021-2022
// and is also part of the Thesis "IoT: Standards & Platforms".

#include <SPI.h> //include the SPI.h official library
#include <Ethernet.h> //include the Etherner.h library
#include <ESP8266WiFi.h> //include the ESP8266 library
#include <ThingerESP8266.h> //include the neccessary libraries
#include "DHT.h" //include the adafruit DHT sensor library


// dht config
#define DHTPIN 2 //declare the pin that the sensor is connected to
#define DHTTYPE DHT11 //declare the DHT type
#define USERNAME "AlexandrosPanag" //define your username's password from thinger.io
#define DEVICE_ID "ESP8266" //define your device's id from thinger.io
#define DEVICE_CREDENTIAL "HL+1&FCQ@Kq4G-lH" //define your randomly generated credentials from thinger.io

#define SSID "Redmi 7" //define your router's id
#define SSID_PASSWORD "mypassword" //define your router's password

DHT dht(DHTPIN, DHTTYPE); //initialize the DHT11 sensor

// thinger.io config
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL); //generate a connection

void setup() {
  Serial.begin(9600); //initialize the Serial baud rate to 9600 bauds/sec
  dht.begin(); //begin the temperature & humidity measurements
  thing.add_wifi(SSID, SSID_PASSWORD); //add the wi-fi password
  thing["DHT11"] >> [](pson& out){ //output into the Thinger.io API the sensor values
    out["HUMIDITY"] = dht.readHumidity(); //output into the Thinger.io API the Humidity (Humidity)
    out["CELSIUS"] = dht.readTemperature(); //output into the Thinger.io the Celsius (Celsius)
    //out["Fahrenheit"] = dht.readTemperature(true); OPTIONAL CODE to output the Fahrenheit
  };
}

void loop() {
  int humidity=dht.readHumidity(); // read the sensor's humidity
  int celcius=dht.readTemperature(); //read the sensor's temperature (celsius)
  //int fahrenheit=dht.readTemperature(true); OPTIONAL CODE to output the temperature into Fahrenheit

  
  Serial.println("Humidity:  "); //print a message into the Serial Port
  Serial.print(humidity); //output the Humidity into the Serial Port
  Serial.println(" RH"); //print a message into the Serial Port

  Serial.println("Celsius:  "); //print a message into the Serial Port
  Serial.print(celcius); //output the Celsius into the Serial Port
  Serial.println("  °C"); //print a message into the Serial Port

  //Serial.println("Fahrenheit: ");
  //Serial.print(fahrenheit);
  //Serial.println(" °F");
  
  thing.handle(); //Handle the connection with the Thinger.io platform
  delay(1000); //delay for about 1 second before taking another measurement
}