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
#include "wifiServer.h"

// Constructor for the display
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE); //, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

float old_temp = -273;
float old_humidity = 0;

bool onPressed, offPressed;
bool buttonStateChanged;
bool thermostatChanged;
bool thermostatActive;

void setup()
{
    Serial.begin(115200);
    Serial.println("Thermostat starting...");

    pinMode(A6, INPUT_PULLUP);
    pinMode(A7, INPUT_PULLUP);

    pinMode(13, OUTPUT);

    u8g2.begin();
    u8g2.setFont(u8g2_font_lubB12_te);

    if (!HS300x.begin())
    {
        while (1)
            ;
    }

    setupWifiServer();
}

void readButtons()
{
    buttonStateChanged = false;
    bool off = !digitalRead(A6);
    if (off != offPressed)
    {
        buttonStateChanged = true;
        offPressed = off;
    }
    bool on = !digitalRead(A7);
    if (on != onPressed)
    {
        buttonStateChanged = true;
        onPressed = on;
    }
}

void updateThermostat()
{
    bool newThermostatActive = millis() % 4000 > 2000;
    thermostatChanged = (newThermostatActive != thermostatActive);
    if (thermostatChanged)
    {
        thermostatActive = newThermostatActive;
        digitalWrite(13, thermostatActive);
    }
}

void updateSerial()
{
    if (buttonStateChanged)
    {
        if (onPressed)
        {
            Serial.println("Up pressed");
        }
        else if (offPressed)
        {
            Serial.println("Down pressed");
        }
        else
        {
            Serial.println("Auto");
        }
    }

    static unsigned long lastStarMillis = 0;
    const unsigned long now = millis();
    if (now - lastStarMillis >= 1000)
    {
        lastStarMillis = now;
        Serial.print("*");
    }
}

void updateDisplay()
{
    // read all the sensor values
    float temperature = HS300x.readTemperature();
    float humidity = HS300x.readHumidity();

    char buffer[100];
    snprintf(buffer, 100, "%.1f%cC/%.1f %%\n", temperature, '.', humidity);
    u8g2.firstPage();
    do
    {
        u8g2.drawStr(0, 20, buffer);

        if (onPressed)
        {
            u8g2.drawStr(0, 40, "On");
        }
        else if (offPressed)
        {
            u8g2.drawStr(0, 40, "Off");
        }
        else if (thermostatActive)
        {
            u8g2.drawStr(0, 40, "(on)");
        }
        else
        {
            u8g2.drawStr(0, 40, "(off)");
        }
    } while (u8g2.nextPage());
}

void loop()
{
    static long lastUpdateTimeMillis = millis();
    readButtons();

    // update display once per second, or when button state changed
    long tm = millis();
    if (tm - lastUpdateTimeMillis > 1000 || buttonStateChanged || thermostatChanged)
    {
        updateDisplay();
        lastUpdateTimeMillis = tm;
    }
    updateThermostat();
    updateSerial();
    loopWifiServer();
}
