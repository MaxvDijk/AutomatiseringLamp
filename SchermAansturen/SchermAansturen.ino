

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define TFT_CS        10
#define TFT_RST        8 
#define TFT_DC         9

int const SENSOR_PIN = 7;
boolean objectDetect = false;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); // pins worden gezet

void setup() {

  pinMode (SENSOR_PIN, INPUT); 

  Serial.begin(9600);

  tft.initR(INITR_GREENTAB);          // Init ST7735S chip, Green tab

  tft.fillScreen(ST77XX_BLACK);       // Kleur scherm
   // Text, kleur tekst
  delay(1000);
  tft.drawCircle(30,  10, 10, ST77XX_WHITE);

}

void loop() {
  // put your main code here, to run repeatedly:

  if ( digitalRead( SENSOR_PIN))        // If detector Output is HIGH, there is no object
  {
    if (objectDetect == true) {         // If we had previously detected an object
      objectDetect = false;
      delay(10);
      tft.fillCircle(30, 10, 10, ST77XX_RED);             // Set flag to no object detected
      tft.setCursor(30, 30);
      tft.println("TEST");
        
    }
  }
  else                                      // If the Output is LOW, there is an object detected
  {
    objectDetect = true;                    // Set flag
    delay(10);
    tft.fillCircle(30, 10, 10, ST77XX_GREEN);        // turn on LED to show the detection.
    tft.setCursor(30, 30);
    tft.println("object detected");
    Serial.println("Object Detected");      // Also send detection to the serial monitor window
  }
  delay(1000);

}

void testdrawtext(char *text, uint16_t color) { //functie om tekst op het scherm te krijgen
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}
void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}
