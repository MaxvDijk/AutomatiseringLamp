
#include <SPI.h>
#include <Wire.h>
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "env.h"


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char publishTopic[] = "Max/lightStatus";
const char subscribeTopic[] = "Max/lightStatus";

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c //Screen adress
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int const SENSOR_PIN = 7;
int const LED_PIN = 3;
boolean objectDetect = false;


int ledState = LOW;     // the current state of LED
int lastSensorState;    // the previous state of button
int currentSensorState; //currentSensorState


void setup() {
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  pinMode(LED_PIN, OUTPUT); //Led light gets put as output
  pinMode (SENSOR_PIN, INPUT); //Motionsensor gets put as input

  display.clearDisplay();

  delay(1000);

  currentSensorState = digitalRead(SENSOR_PIN);
  //Startup process
  display.setTextSize(1.5);
  display.setCursor(32,32);             // Start at top-left corner
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.println("Welcome");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setCursor(10,32);             // Start at top-left corner
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.print("Starting up");
  display.display();
  for(int16_t i = 0; i < 7; i++){ //Zorgt er voor dat de stipjes achter Starting up komen
    display.print(".");
    display.display();
    delay(150);
  }
  delay(2000);
  display.clearDisplay();
  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(80);
  }
  display.clearDisplay();

  //Connecting to MQTT
while (WiFi.begin(SSID, PASS) != WL_CONNECTED){
  delay(5000);
}  

bool MQTTconnected = false;
mqttClient.setUsernamePassword(MQTTUsername, MQTTPassword); // Gives login to connect to the MQTT 
while (!MQTTconnected) {
  if (!mqttClient.connect(MQTTURL, MQTTPort)){
  Serial.println("connecting to MQTT....."); // Prints to serial 
  display.setCursor(10,32);             // Start at top-left corner
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.print("Connecting to MQTT ");
  display.display();
  for(int16_t i = 0; i < 7; i++){ //Zorgt er voor dat de stipjes achter Starting up komen
    display.print(".");
    display.display();
    delay(150);
  }
  delay(1000);
  display.clearDisplay();
  } else {
  MQTTconnected = true;
    Serial.println("connected to MQTT"); // Prints to serial 
    display.setCursor(10,32);             // Start at top-left corner
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.print("connected to MQTT!");
    display.display();
    delay(200);
    display.clearDisplay();
  
    }
  }
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(subscribeTopic);
}




void loop() {
  lastSensorState    = currentSensorState;      // save the last state
  currentSensorState = digitalRead(SENSOR_PIN); // read new state
  mqttClient.poll();

  if(lastSensorState == HIGH && currentSensorState == LOW) {
    Serial.println("Motion detected!");
    // toggle state of LED
    ledState = !ledState;
    
    digitalWrite(LED_PIN, ledState);
    if(ledState == HIGH) {
      display.clearDisplay();
      display.setCursor(10,5);             // Start at top-left corner
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.println("Light: On");
      display.fillCircle(3, 8, 3, SSD1306_WHITE);
      display.display();

      mqttClient.beginMessage(publishTopic,true,0);
      mqttClient.print(ledState);
      mqttClient.endMessage();

    }else {
      display.clearDisplay();
      display.setCursor(10,5);             // Start at top-left corner
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.println("Light: Off");
      display.drawCircle(3, 8, 3, SSD1306_WHITE);
      display.display();

      mqttClient.beginMessage(publishTopic,true,0);
      mqttClient.print(ledState);
      mqttClient.endMessage();
    }
    
  }
  delay(400);
}

  


void testdrawcircle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void onMqttMessage(int messageSize) {
  Serial.print("Received a message with topic '");
  Serial.println(mqttClient.messageTopic());
  String message = "";

while (mqttClient.available()) {
  message.concat((char)mqttClient.read());
}
  Serial.println(message);
  if (message == "HIGH")
    Serial.println(message);
  else
    Serial.println(message);

}

void testfillcircle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}
