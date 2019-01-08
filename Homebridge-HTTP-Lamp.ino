#include "ESP8266WiFi.h"
#include <WiFiClient.h>

//Naming Vars
const int LAMP = D1; //Relay
const int SWITCH = D2; //Capacitive touch
const int RED = D6; //Red LED
const int GREEN = D7; //Green LED
const int BLUE = D5; //Blue LED
const int GROUND = D8; //Emulated ground
String PLACE = "bedroom"; //Room name
String ACCESSORY = "lamp"; //Accessory name

//Network variables
const char* ssid = "ssid"; //WiFi Name
const char* password = "password"; //WiFi Password
const IPAddress ip(192, 168, 0, 201); //Board IP
const IPAddress dns(8, 8, 4, 4); //DNS
const IPAddress gateway(192, 168, 0, 1); //Router
const IPAddress subnet(255, 255, 255, 0); //Subnet mask
WiFiServer server(80); //Server

//control variables
bool previousState = false; //Previous State
int LampState = HIGH; //Lamp State

//Color variables
String OnHexString = "00ff00"; //Lamp HEX color on (start Green)
String OffHexString = "ff0000"; //Lamp HEX color off (start Red)
int onRed = 0; //Red LED color on
int onGreen = 255; //Green LED color on
int onBlue = 0; //Blue LED color on
int offRed = 255; //Red LED color off
int offGreen = 0; //Green LED color off
int offBlue = 0; //Blue LED color off

//Turn Lamp ON or OFF
void ChangeLampState(bool state){
  digitalWrite(LAMP, !state); //Turn Relay ON/OFF
  LampState = state; //Set Lamp State
}

//Handle HTTP Requests
void HomebridgeRequests(){
  WiFiClient client = server.available(); //Verify if there is a request
  if (!client) {
    return; //Return if there is no requests
  }

  Serial.println("new client");
  while (!client.available()) { //Wait for client
    delay(1);
  }

  String req = client.readStringUntil('\r'); //Get the request url
  Serial.println(req);

  //Page preparation
  client.println("HTTP/1.1 200 OK"); //Set response to HTTP Code 200 OK
  client.println("Content-Type: text/html"); //Set content type to HTML
  client.println();

  //Verify the URL accessed on the format IP/PLACE+ACCESSORY/object/state
  //Example: 192.168.0.201/bedroomlamp/switch/on  
  if (req.indexOf("/" + PLACE + ACCESSORY + "/switch/on") != -1) { //Turn on switch
    ChangeLampState(HIGH);
    client.println("<!DOCTYPE HTML>\r\n<html>\r\nBedroom lamp is now on</html>");
  }
  else if (req.indexOf("/" + PLACE + ACCESSORY + "/switch/off") != -1) { //Turn off switch    
    ChangeLampState(LOW);
    client.println("<!DOCTYPE HTML>\r\n<html>\r\nBedroom lamp is now off</html>");
  }
  else if (req.indexOf("/" + PLACE + ACCESSORY + "/switch/status") != -1) { //Verify switch status    
    client.println(LampState);
  }  
  else if (req.indexOf("/" + PLACE + ACCESSORY + "/color/status") != -1) { //Verify color status
    if(LampState){
      client.println(OnHexString);
    }
    else if(!LampState){
      client.println(OffHexString);
    }     
  }
  else if (req.indexOf("/" + PLACE + ACCESSORY + "/color") != -1) { //Set color
    if(LampState){
      OnHexString = "";
      OnHexString = (req.substring(req.length()-15, req.length()-9));
      setHex(OnHexString);
    }
    else if(!LampState){
      OffHexString = "";
      OffHexString = (req.substring(req.length()-15, req.length()-9));
      setHex(OffHexString);
    }      
  }
  else
  {
    Serial.println("invalid request"); //Invalid path
    client.stop();
    return;
  }
 
  while (client.read() >= 0);
  client.stop();  
  delay(1);
  Serial.println("Client disonnected"); 
}

//Handle Capacitive Touch
void SwitchRequests(){ //Handle
  LampState = !digitalRead(LAMP);
  if(digitalRead(SWITCH) == HIGH){
    if(previousState == false){
      previousState = true;
      ChangeLampState(!LampState);
    }
  }
  else if(digitalRead(SWITCH) == LOW){
    previousState = false;
  } 
}

//Manage RGB LED Color
void ManageRGBLed(){
  if(!LampState){
    analogWrite(RED, offRed);
    analogWrite(GREEN, offGreen);
    analogWrite(BLUE, offBlue);  
  }
  else{
    analogWrite(RED, onRed);
    analogWrite(GREEN, onGreen);
    analogWrite(BLUE, onBlue);
  }
}

//Set RGB color based on HEX String
void setHex(String hexString) {
  long number = (long) strtol( &hexString[0], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  r = map(r, 0, 255, 0, 1023);
  g = map(g, 0, 255, 0, 1023);
  b = map(b, 0, 255, 0, 1023);
  if(LampState){
      onRed = r;
      onGreen = g;
      onBlue = b;
    }
    else if(!LampState){
      offRed = r;
      offGreen = g;
      offBlue = b;
    }     
}

void setup(void){
  //Prepare pins
  pinMode(LAMP, OUTPUT);
  pinMode(SWITCH, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GROUND, OUTPUT);
  digitalWrite(GROUND, LOW); //Emulate ground
  
  Serial.begin(9600);
  WiFi.begin(ssid, password); //Start WiFi
  
  while (WiFi.status() != WL_CONNECTED) { //Wait WiFi to connect and make LED blue
     analogWrite(RED, 0);
     analogWrite(GREEN, 0);
     analogWrite(BLUE, 255);
     delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  WiFi.config(ip, dns, gateway, subnet); //Configure WiFi
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
}

void loop() {
  HomebridgeRequests();
  SwitchRequests();
  ManageRGBLed();
}
