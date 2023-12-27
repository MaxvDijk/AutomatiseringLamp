
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
int currentSensorState;


void setup() {
  Serial.begin(9600);
  

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  pinMode(LED_PIN, OUTPUT);
  pinMode (SENSOR_PIN, INPUT); 
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
  delay(1000);
  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_BLACK);
    display.display();
    delay(80);
  }
  display.clearDisplay();

}

void loop() {
  // put your main code here, to run repeatedly:
  lastSensorState    = currentSensorState;      // save the last state
  currentSensorState = digitalRead(SENSOR_PIN); // read new state

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
    }else {
      display.clearDisplay();
      display.setCursor(10,5);             // Start at top-left corner
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.println("Light: Off");
      display.drawCircle(3, 8, 3, SSD1306_WHITE);
      display.display();
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
