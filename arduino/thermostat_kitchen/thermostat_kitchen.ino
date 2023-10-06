// display
// https://www.tinytronics.nl/shop/nl/displays/oled/1.3-inch-oled-display-128*64-pixels-wit-i2c
// SH1106 128X64_NONAME
// U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// temperature
// https://docs.arduino.cc/tutorials/nano-33-ble-sense/humidity-and-temperature-sensor

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
// HTS221 on Arduino Nano 33 BLE Sense
#include <Arduino_HS300x.h>

// Constructor for the display
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

float old_temp = -273;
float old_humidity = 0;

void setup() {
  // u8g2.begin() is required and will sent the setup/init sequence to the display
  u8g2.begin();

  if (!HS300x.begin()) {
    while (1);
  }
}

void loop() {
  // read all the sensor values
  float temperature = HS300x.readTemperature();
  float humidity    = HS300x.readHumidity();

  char buffer[100];
  snprintf(buffer, 100, "%.1f%cC/%.1f %%", temperature, '.', humidity);

  // draw something on the display with the `firstPage()`/`nextPage()` loop
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_lubB12_te);
    u8g2.drawStr(0, 20, buffer);
  } while ( u8g2.nextPage() );
  delay(1000);
}
