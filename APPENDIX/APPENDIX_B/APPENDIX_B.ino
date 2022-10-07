/*
*  By Alexandros Panagiotakopoulos
AM:1716/ OLD AM:16108
UNIVERSITY OF IOANNINA - DEPARTMENT OF INFORMATICS & TELECOMMUNICATIONS
*/



//Random(), source used :https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
//Thinger.io(),source used: https://www.arduino.cc/reference/en/libraries/thinger.io/
//Based on a few older projects that i created, which can be found here: https://github.com/AlexandrosPanag/Texas_Instruments/blob/main/CC3200/ALL%20LEDS%20BLINK/CC3200_3_LEDS_BLINK.ino
//and here: https://github.com/AlexandrosPanag/Texas_Instruments/tree/main/CC3200/Button_Utilization



//in this project, the CC3200 BOARD will send random led states into the Thinger.io platform
//usually the random command takes one loop away to be properly initialized, hence why we require a guardian variable to
//properly protect our code. For a better explanation i encourage you to read my thesis "Internet of Things:Standards & Platforms
// NOTE: The code does take a while in order to compile

// This Project is an original work created by Alexandros Panagiotakopoulos during the academic year 2021-2022
// and is also part of the Thesis "IoT: Standards & Platforms".

#include <SPI.h> //include the SPI.h library
#include <Ethernet.h> //include the Ethernet.h library
#include <ThingerESP32.h> //include the ThingerESP32 library



#define BUTTON 34 // GIOP21 pin connected to button
#define USERNAME "AlexandrosPanag" //enter your Thinger.io username here
#define DEVICE_ID "ESP32" //enter your device id from your Thinger.io account here
#define DEVICE_CREDENTIAL "5oNbJhIgnB3bnM5!" //enter the given device credentials from the Thinger.io here



#define SSID "Redmi 7" //declare your router's username (SSID)
#define SSID_PASSWORD "mypassword" //declare your router's password (SSID_PASSWORD)



#define BLUE_LED 19 //declare GPIO 18 as Blue Led
#define RED_LED 18 // declare GPIO 19 as Red Led
#define GREEN_LED 2 // declare GPIO 2 as Green Led




int guardian=0; //the protector of our code, who can properly help the initialization of the random()
//command, and at the same time, help the Thinger.io connection properly to be established



//Here, all 3 LED statuses will be declared. Each status will be sent to the Thinger.io website which can be remotely
//viewed from there. All statuses represent whether or not, our LED is turned on or off
int BLUE_STATUS=0; //the status which represents whether or not our RED LED is turned On or Off.
int RED_STATUS=0;//the status which represents whether or not our YELLOW LED is turned On or Off.
int GREEN_STATUS=0; //the status which represents whether or not our GREEN LED is turned On or Off.

int randBLUE = 0; //variable to store the random generated number for the red led
int randRED = 0; //variable to store the random generated number for the yellow led
int randGREEN = 0; //variable to store the random generated number for the green led


int buttonState = 0;         // variable for reading the pushbutton status


// thinger.io config
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL); //generate a connection



// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(BLUE_LED, OUTPUT); // declare the Blue led as an output
  pinMode(RED_LED, OUTPUT); // declare the Red Led as an output
  pinMode(GREEN_LED, OUTPUT); // declare the Green Led as an output
  pinMode(BUTTON, INPUT_PULLUP); //declare the left button input    
  // configure wifi network
  thing.add_wifi(SSID, SSID_PASSWORD);
  Serial.begin(9600); //initialize the Serial port to 9600 bauds/sec
  randomSeed(analogRead(0));// if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.



   //output into the Thinger.io the status of your LEDs, which can be viewed from there
    thing["LED STATES"] >> [](pson& out){
    out["BLUE_LED"] = BLUE_STATUS; //output the Green LED status (whether it is turned on or off).
    out["RED_LED"] = RED_STATUS; //output the Yellow LED status (whether it is turned on or off).
    out["GREEN_LED"] = GREEN_STATUS; ////output the Red LED status (whether it is turned on or off).
    };
}



// the loop function runs over and over again forever
void loop() {
  //Generate a random number from 10 to 99, the code must run 3 times at least for the random()
  //command to be properly initialized
    const int randBLUE = random(10, 100); //choose a random number for the red LED from 10 to 99
    const int randRED = random(10,100); //choose a random number for the yellow LED from 10 to 99
    const int randGREEN = random(10, 100); //choose a random number for the green LED from 10 to 99



 thing.handle(); //handle the connection with the Thinger.io platform
  
  if(guardian>2){ //if the guardian is less than 3, dont output anything. The random command hasn't properly established yet
  Serial.println(); //print a message into the serial port
  Serial.println("**********************************"); //print a message into the serial port
  // generate a random number from 10 to 19, then store them to each counter that represents an LED


 if(randBLUE%2==1){ //if the red random number can't be modulated
    Serial.println("BLUE WILL TURN ON"); //print a message into the serial port
    digitalWrite(BLUE_LED, HIGH);   // turn the BLUE LED on (HIGH is the voltage level)
    delay(100); //wait for about 1 second
    BLUE_STATUS=1; //output into the Thinger.io website "1" under the RED_LED box, which represents that the LED is on.
  }
  else{ //if the red random number can be modulated
    Serial.println("BLUE WILL TURN OFF"); //print a message into the serial port
    digitalWrite(BLUE_LED, LOW);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for about 1 second
    BLUE_STATUS=0;//output into the Thinger.io website "0" under the RED_LED box, which represents that the LED is off.
  }


   if(randRED%2==1){ //if the yellow random number can't be modulated
    Serial.println("RED WILL TURN ON");//print a message into the serial port
    digitalWrite(RED_LED, HIGH);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for 0.1 second
    RED_STATUS=1;//output into the Thinger.io website "1" under the YELLOW_LED box, which represents that the LED is on.
  }
  else{
    Serial.println("RED WILL TURN OFF"); ////if the yellow random number can be modulated
    digitalWrite(RED_LED, LOW);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for 0.1 second
    RED_STATUS=0;//output into the Thinger.io website "0" under the YELLOW_LED box, which represents that the LED is off
  }


  if(randGREEN%2==1){ //if the green random number can't be modulated
    Serial.println("GREEN WILL TURN ON"); //print a message into the serial port
    digitalWrite(GREEN_LED, HIGH);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for 0.1 second
    GREEN_STATUS=1;//output into the Thinger.io website "1" under the GREEN_LED box, which represents that the LED is on.
  }
  else{ //if the green random number can be modulated
    Serial.println("GREEN WILL TURN OFF"); //print a message into the serial port
    digitalWrite(GREEN_LED, LOW);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for 0.1 second
    GREEN_STATUS=0;//output into the Thinger.io website "0" under the RED_LED box, which represents that the LED is off.
  }

  Serial.println("***********************************");
  delay(100); // wait for 0.1 second
  //OPTIONAL COMMANDS --to check whether or not the number is truly random, or not. Just uncomment the lines 139-144.
  //Serial.println("the random red value is:");
  //Serial.print(randRed);
  //Serial.println("the random yellow value is:");
  //Serial.print(randYellow);
  //Serial.prinln("the random green value is:");
  //Serial.print(randGreen);
  
  if(guardian==3){ //if the guardian reaches the third counter (third time the code has run) we want to send
    //the data into our Thinger.io API, and we also don't want our code to lose the connection.
    //However,if the user desires, he can press one of the built-in buttons and let the code compile all over again
    while(true){
      // read the state of the pushbutton value:
      buttonState = digitalRead(34); //declare the right button reading
      thing.handle(); //handle the connection with the Thinger.io platform
       if (buttonState == LOW) { //if the right or the left button is pressed
          guardian=0; //then reset the guardian (re-establish the random() command
          break; //and break the while(true) loop
          }
    }
  }
  }
  guardian++; //increase the counter each time the code has run, until it reaches the maximum run of 3 which will be trapped, on the while true() loop
  //unless the user decides to push one of the buttons
}
