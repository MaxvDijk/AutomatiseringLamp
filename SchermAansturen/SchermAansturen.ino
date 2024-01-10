// Libraries
#include <SPI.h>
#include <Wire.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <ArduinoMqttClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "env.h"

// WiFiClient en een MQTTClient worden aangemaakt
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
HttpClient http = HttpClient(wifiClient, IPBridge, HuePort);

const char publishTopic[] = "Max/lightStatus";  // Hier wordt de topic gedefinieerd waarnaartoe ge published wordt
const char subscribeTopic[] = "Max/lightStatus"; // Hier wordt gesubscribed op een top
int const SENSOR_PIN = 7;  // Bewegings sensor pin wordt gedefinieerd 
int const LED_PIN = 3; // Led lamp pin wordt gedefinieerd
int ledState = LOW; // De huidige status van de ledlamp
int lastSensorState; // De vorige status van de bewegings sensor
int currentSensorState; // De huidige status van de bewegings sensor

#define SCREEN_WIDTH 128 // OLED scherm breedte, in pixels
#define SCREEN_HEIGHT 64 // OLED scherm hoogte, in pixels
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3c // scherm adress
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT); // Led licht wordt als output aangegeven 
  pinMode (SENSOR_PIN, INPUT); // Motionsensor wordt als input aangegeven
  currentSensorState = digitalRead(SENSOR_PIN); // Huidige sensor status wordt opgehaald door het lezen van de bewegings sensor

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // scherm wordt gestart. Start het scherm niet dan komt hij in een loop
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  
  // Hier probeert hij connectie te maken met de WiFi 
  while (WiFi.begin(SSID, PASS) != WL_CONNECTED){
  delay(5000);
}  
  display.clearDisplay(); // Cleart het scherm
  Startup(); // Voert de functie startup uit waar hij het opstart scherm laat zien

bool MQTTconnected = false;
mqttClient.setUsernamePassword(MQTTUsername, MQTTPassword); // Geeft login gegevens om te verbinden met MQTT server
while (!MQTTconnected) {
  if (!mqttClient.connect(MQTTURL, MQTTPort)){
  Serial.println("Connecting to MQTT....."); // Print naar serial dat hij aan het verbinden is. 
  text(1, 10, 32, 1, "Connecting to MQTT ") // Print tekst naar scherm
  dots(7, 80); //Plaatst 7 punten met een delay van 80 achter de tekst
  delay(200);
  display.clearDisplay();
  } else {
  MQTTconnected = true;
    Serial.println("connected to MQTT"); // Prints to serial 
    text(1, 10, 32, 1, "connected to MQTT!"); // Print tekst naar scherm
    delay(100);
    display.clearDisplay();
    }
  }
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(subscribeTopic);

}

void loop() {

  lastSensorState    = currentSensorState;      // Slaat de laatste status op
  currentSensorState = digitalRead(SENSOR_PIN); // Leest de nieuwe status
  mqttClient.poll();

  if(lastSensorState == HIGH && currentSensorState == LOW) {
    Serial.println("Motion detected!"); // Print naar serial dat er een motion detected is 
    ledState = !ledState; // schakeld de status van de led 
    
    digitalWrite(LED_PIN, ledState);
    if(ledState == HIGH) {
      display.clearDisplay();
      text(1, 10, 5, 1, "Light: On"); // voert de functie uit die tekst op het scherm zet
      display.fillCircle(3, 8, 3, SSD1306_WHITE); // Maakt een wit balletje voor de tekst 
      display.display();

      // Voert de functie uit om te communiceren met de HUE api. Geeft de waarde true mee wat de licht status aanpast
      controlHueLamp(true);
      // Stuurt bericht naar MQTT
      mqttClient.beginMessage(publishTopic,true,0);
      mqttClient.print(ledState);
      mqttClient.endMessage();

    }else {
      display.clearDisplay();
      text(1, 10, 5, 1, "Light: Off"); // Voert de functie uit die tekst op het scherm zet
      display.drawCircle(3, 8, 3, SSD1306_WHITE); // Zet een witte circle voor de tekst
      display.display();

      // Voert de functie uit om te communiceren met de HUE api. geeft de waarde false mee wat de licht status aanpast
      controlHueLamp(false);
      //Sending message to MQTT
      mqttClient.beginMessage(publishTopic,true,0);
      mqttClient.print(ledState);
      mqttClient.endMessage();
    }
    
  }
  delay(200);
} 

// Hier onder staan alle functies voor dit project.

//Receives an MQTT message
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

void controlHueLamp(bool turnOn) { //deze functie zorgt voor de communicatie naar de API
  // Bouw de URL voor de PUT-aanvraag
  String url = "/api/" + String(key) + "/lights/2/state";
  String body = "{\"on\":" + String(turnOn ? "true" : "false") + "}";

  // Voer de PUT-aanvraag uit
  http.beginRequest();
  http.put(url.c_str(), "application/json", body.c_str()); //Hier plakt hij de url samen en geeft hij een put bericht mee om de status van het licht te veranderen
  http.endRequest();

  // Lees het antwoord van de server
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();

  // Controleer het antwoord
  if (statusCode == 200) {
    Serial.println("Succesvolle aanvraag!");
  } else {
    Serial.print("Fout bij de aanvraag. Statuscode: ");
    Serial.println(statusCode);
    Serial.print("Antwoord van server: ");
    Serial.println(response);
  }
}

void text(float Textsize, int X, int Y, int Colour, char* Text){ //Deze functie zorgt er voor dat ik makkelijk teskt kan plaatsen op het scherm
  display.setTextSize(Textsize);       //Zet de grote van de tekst
  display.setCursor(X,Y);              //Zet de cursor op de aangegeven pixels
  if (Colour == 1)
    display.setTextColor(SSD1306_WHITE);  //Maakt tekst wit
  else if (Colour == 2)
    display.setTextColor(SSD1306_BLACK); //Maakt tekst zwart
  else
    display.setTextColor(SSD1306_INVERSE); //Maakt de tekst een andere kleur dan de back ground. zwart of wit
  display.println(Text); // Print de input
  display.display();  // Zorgt ervoor dat het op het scherm zichtbaar wordt
}

void Startup(){ //hier zie je het opstart process van het scherm
  text(2, 32, 32, 1 ,"Welcome");
  delay(2000);
  display.clearDisplay();
  text(1.5, 10, 32, 1, "Starting up");
  dots(7, 150);
  delay(400);
  display.clearDisplay();
  testdrawcircle();
  delay(400);
  test2drawcircle();
  delay(2000);
  display.clearDisplay();
}

void dots(int16_t amountDots, int16_t Delay){
    for(int16_t i = 0; i < amountDots; i++){ //Zorgt er voor dat de stipjes achter een woord komen
    display.print(".");
    display.display();
    delay(Delay);
  }
}

void testdrawcircle(void) { //circles vormen van klein naar groot
  display.clearDisplay();
  for(int16_t i=0; i<max(display.width(),display.height()); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_INVERSE);
    display.display();
    delay(2);
  }
}
void test2drawcircle(void) { //circles vormen van groot naar klein
  for(int16_t i=128; i>0 ; i-=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_INVERSE);
    display.display();
    delay(2);
  }
}

