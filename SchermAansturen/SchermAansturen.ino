

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define TFT_CS        10
#define TFT_RST        8 
#define TFT_DC         9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); // pins worden gezet

void setup() {
  Serial.begin(9600);

  tft.initR(INITR_GREENTAB);          // Init ST7735S chip, Green tab

  tft.fillScreen(ST77XX_BLACK);       // Kleur scherm
  testdrawtext("TEST", ST77XX_WHITE); // Text, kleur tekst
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  tft.invertDisplay(true);
  delay(500);
  tft.invertDisplay(false);
  delay(500);

}

void testdrawtext(char *text, uint16_t color) { //functie om tekst op het scherm te krijgen
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}
