/*********
 By Alexandros Panagiotakopoulos
AM:1716/ OLD AM:16108
UNIVERSITY OF IOANNINA - DEPARTMENT OF INFORMATICS & TELECOMMUNICATIONS
*********/
//Random(), source used :https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
//Thinger.io(),source used: https://www.arduino.cc/reference/en/libraries/thinger.io/
//Based on a few older projects that i created, which can be found here: https://github.com/AlexandrosPanag/Texas_Instruments/blob/main/CC3200/ALL%20LEDS%20BLINK/CC3200_3_LEDS_BLINK.ino
//and here: https://github.com/AlexandrosPanag/Texas_Instruments/tree/main/CC3200/Button_Utilization

//in this project, the CC3200 BOARD will send random led states into the Thinger.io platform
//usually the random command takes one loop away to be properly initialized, hence why we require a guardian variable to
//properly protect our code. For a better explanation i encourage you to read my thesis "Internet of Things:Standards & Platforms
// NOTE: The code does take a while in order to be properly compiled so be patient

// This Project is an original work created by Alexandros Panagiotakopoulos during the academic year 2021-2022
// and is also part of the Thesis "IoT: Standards & Platforms".

//Include all the necessary libraries
#include <WiFi.h> //necessary to establish a Wi-Fi connection 
#include <ThingerWifi.h> //necessary to establish a Thinger.io connection

#define USERNAME "AlexandrosPanag" //enter your Thinger.io username here
#define DEVICE_ID "CC3200" //enter your device id from your Thinger.io account here
#define DEVICE_CREDENTIAL "5oNbJhIgnB3bnM5!" //enter the given device credentials from the Thinger.io here

#define SSID "Redmi 7" //declare your router's username (SSID)
#define SSID_PASSWORD "mypassword" //declare your router's password (SSID_PASSWORD)

//Here, all 3 LED statuses will be declared. Each status will be sent to the Thinger.io website which can be remotely
//viewed from there. All statuses represent whether or not, our LED is turned on or off
int red_status=0; //the status which represents whether or not our RED LED is turned On or Off.
int yellow_status=0;//the status which represents whether or not our YELLOW LED is turned On or Off.
int green_status=0; //the status which represents whether or not our GREEN LED is turned On or Off.



int randRed = 0; //variable to store the random generated number for the red led
int randYellow = 0; //variable to store the random generated number for the yellow led
int randGreen = 0; //variable to store the random generated number for the green led



const int leftButton = PUSH1; //initialize the right button
const int rightButton = PUSH2;     // initialize the left button
int rightbuttonState = 0;         // variable for reading the pushbutton status
int leftbuttonState = 0;         // variable for reading the pushbutton status



int guardian=0; //the protector of our code, who can properly help the initialization of the random()
//command, and at the same time, help the Thinger.io connection properly to be established

ThingerWifi thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL); //this command, adds your Thinger.io credentials
//to establish a connection properly

void setup() {
  // set the boards led to output
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(rightButton, INPUT_PULLUP); //declare the right button input
  pinMode(leftButton, INPUT_PULLUP); //declare the left button input    
  // configure wifi network
  thing.add_wifi(SSID, SSID_PASSWORD);
  Serial.begin(9600); //initialize the Serial port to 9600 bauds/sec
  randomSeed(analogRead(0));// if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.

    //output into the Thinger.io the status of your LEDs, which can be viewed from there
    thing["LED STATES"] >> [](pson& out){
    out["GREEN_LED"] = green_status; //output the Green LED status (whether it is turned on or off).
    out["YELLOW_LED"] = yellow_status; //output the Yellow LED status (whether it is turned on or off).
    out["RED_LED"] = red_status; ////output the Red LED status (whether it is turned on or off).
    };
}


void loop() {
  //Generate a random number from 10 to 99, the code must run 3 times at least for the random()
  //command to be properly initialized
    const int randRed = random(10, 100); //choose a random number for the red LED from 10 to 99
    const int randYellow = random(10,100); //choose a random number for the yellow LED from 10 to 99
    const int randGreen = random(10, 100); //choose a random number for the green LED from 10 to 99

  thing.handle(); //handle the connection with the Thinger.io platform
  
  if(guardian>2){ //if the guardian is less than 3, dont output anything. The random command hasn't properly established yet
  Serial.println(); //print a message into the serial port
  Serial.println("**********************************"); //print a message into the serial port
  // generate a random number from 10 to 19, then store them to each counter that represents an LED

  

  if(randRed%2==1){ //if the red random number can't be modulated
    Serial.println("RED WILL TURN ON"); //print a message into the serial port
    digitalWrite(RED_LED, HIGH);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for about 1 second 
    red_status=1; //output into the Thinger.io website "1" under the RED_LED box, which represents that the LED is on.
  }
  else{ //if the red random number can be modulated
    Serial.println("RED WILL TURN OFF");
    digitalWrite(RED_LED, LOW);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for about 1 second 
    red_status=0;//output into the Thinger.io website "0" under the RED_LED box, which represents that the LED is off.
  }


   if(randYellow%2==1){ //if the yellow random number can't be modulated
    Serial.println("YELLOW WILL TURN ON"); //print a message into the serial port
    digitalWrite(YELLOW_LED, HIGH);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for 0.1 second
    yellow_status=1;//output into the Thinger.io website "1" under the YELLOW_LED box, which represents that the LED is on.
  }
  else{
    Serial.println("YELLOW WILL TURN OFF"); ////if the yellow random number can be modulated
    digitalWrite(YELLOW_LED, LOW);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for 0.1 second
    yellow_status=0;//output into the Thinger.io website "0" under the YELLOW_LED box, which represents that the LED is off
  }


  if(randGreen%2==1){ //if the green random number can't be modulated
    Serial.println("GREEN WILL TURN ON"); //print a message into the serial port
    digitalWrite(GREEN_LED, HIGH);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for 0.1 second
    green_status=1;//output into the Thinger.io website "1" under the GREEN_LED box, which represents that the LED is on.
  }
  else{ //if the green random number can be modulated
    Serial.println("GREEN WILL TURN OFF"); //print a message into the serial port
    digitalWrite(GREEN_LED, LOW);   // turn the RED LED on (HIGH is the voltage level)
    delay(100); //wait for 0.1 second
    green_status=0;//output into the Thinger.io website "0" under the RED_LED box, which represents that the LED is off.
  }

  
  Serial.println("***********************************"); //print a message into the serial port
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
      rightbuttonState = digitalRead(rightButton); //declare the right button reading
      leftbuttonState = digitalRead(leftButton); //declare the left button reading
      thing.handle(); //handle the connection with the Thinger.io platform
       if (leftbuttonState == HIGH || rightbuttonState == HIGH) { //if the right or the left button is pressed    
          guardian=0; //then reset the guardian (re-establish the random() command
          break; //and break the while(true) loop
          } 
      }
    }
  }
  guardian++; //increase the counter each time the code has run, until it reaches the maximum run of 3 which will be trapped, on the while true() loop
  //unless the user decides to push one of the buttons
}
