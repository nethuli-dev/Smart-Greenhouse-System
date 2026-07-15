/*
 * Smart Greenhouse Monitoring & Management System
 * ------------------------------------------------
 * Board:  Arduino Uno
 * Role:   Reads DHT11 (temperature/humidity) and soil moisture sensor,
 *         drives the water pump and cooling fan relays automatically,
 *         and streams a CSV data string to the ESP8266 over
 *         SoftwareSerial for cloud upload.
 *
 * Data string format sent to ESP8266:
 *   T:<tempC>,H:<humidity%>,S:<soilMoisture%>,P:<pumpState 0/1>,F:<fanState 0/1>
 *
 * Done by FC08 Group
 */

#include <Arduino.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include "config.h"

// ---------- Pin definitions ----------
#define DHTPIN 2 // DHT11 data pin
#define DHTTYPE DHT11
#define SOIL_PIN A0      // Soil moisture sensor analog output
#define PUMP_RELAY_PIN 7 // Relay channel 1 -> water pump
#define FAN_RELAY_PIN 8  // Relay channel 2 -> cooling fan
#define ESP_RX_PIN 10    // Arduino RX  <- ESP8266 TX
#define ESP_TX_PIN 11    // Arduino TX  -> ESP8266 RX

// Set to true if your relay module is "active LOW" (most common cheap modules)
#define RELAY_ACTIVE_LOW true

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial espSerial(ESP_RX_PIN, ESP_TX_PIN);

// ---------- Control thresholds (tune to your crop) ----------
const int SOIL_DRY_THRESHOLD = 40;      // % - below this, start irrigation
const int SOIL_WET_THRESHOLD = 60;      // % - above this, stop irrigation
const float TEMP_HIGH_THRESHOLD = 30.0; // C - above this, turn fan on
const float TEMP_LOW_THRESHOLD = 27.0;  // C - below this, turn fan off

// Soil sensor calibration - measure these with YOUR sensor
const int SOIL_RAW_DRY = 1023; // reading in dry air
const int SOIL_RAW_WET = 300;  // reading in a cup of water

const unsigned long READ_INTERVAL = 5000; // ms between sensor reads
unsigned long lastReadTime = 0;

bool pumpState = false;
bool fanState = false;

// Variables for data logging
float currentTemp = 0;
float currentHumidity = 0;
int currentSoilMoisture = 0;

void relayWrite(int pin, bool on)
{
    bool level = RELAY_ACTIVE_LOW ? !on : on;
    digitalWrite(pin, level ? HIGH : LOW);
}

int readSoilMoisturePercent()
{
    int raw = analogRead(SOIL_PIN);
    int percent = map(raw, SOIL_RAW_DRY, SOIL_RAW_WET, 0, 100);
    return constrain(percent, 0, 100);
}

void setup()
{
    Serial.begin(9600);
    espSerial.begin(9600);
    dht.begin();

    pinMode(PUMP_RELAY_PIN, OUTPUT);
    pinMode(FAN_RELAY_PIN, OUTPUT);
    relayWrite(PUMP_RELAY_PIN, false);
    relayWrite(FAN_RELAY_PIN, false);

    Serial.println(F("========================================"));
    Serial.println(F(" Smart Greenhouse - Arduino Uno Controller"));
    Serial.println(F("========================================"));
    delay(2000);
}

void processCommands()
{
    if (espSerial.available())
    {
        String cmd = espSerial.readStringUntil('\n');
        cmd.trim();
        Serial.print(F("[CMD RECEIVED] "));
        Serial.println(cmd);

        if (cmd == "PUMP_ON")
        {
            pumpState = true;
            relayWrite(PUMP_RELAY_PIN, true);
            Serial.println(F("[ACTION] Manual pump ON"));
        }
        else if (cmd == "PUMP_OFF")
        {
            pumpState = false;
            relayWrite(PUMP_RELAY_PIN, false);
            Serial.println(F("[ACTION] Manual pump OFF"));
        }
        else if (cmd == "FAN_ON")
        {
            fanState = true;
            relayWrite(FAN_RELAY_PIN, true);
            Serial.println(F("[ACTION] Manual fan ON"));
        }
        else if (cmd == "FAN_OFF")
        {
            fanState = false;
            relayWrite(FAN_RELAY_PIN, false);
            Serial.println(F("[ACTION] Manual fan OFF"));
        }
        else if (cmd == "GET_DATA")
        {
            String dataStr = "T:" + String(currentTemp, 1) +
                             ",H:" + String(currentHumidity, 1) +
                             ",S:" + String(currentSoilMoisture) +
                             ",P:" + String(pumpState ? 1 : 0) +
                             ",F:" + String(fanState ? 1 : 0);
            espSerial.println(dataStr);
            Serial.println(F("[RESPONSE] Data sent to ESP"));
        }
    }
}

void loop()
{
    unsigned long now = millis();

    if (now - lastReadTime >= READ_INTERVAL)
    {
        lastReadTime = now;

        currentHumidity = dht.readHumidity();
        currentTemp = dht.readTemperature();
        currentSoilMoisture = readSoilMoisturePercent();

        if (isnan(currentHumidity) || isnan(currentTemp))
        {
            Serial.println(F("[ERROR] Failed to read DHT11 sensor!"));
            // Don't return, continue with soil moisture data only
            currentHumidity = 0;
            currentTemp = 0;
        }

        // ---- Irrigation control ----
        if (currentSoilMoisture < SOIL_DRY_THRESHOLD && !pumpState)
        {
            pumpState = true;
            relayWrite(PUMP_RELAY_PIN, true);
            Serial.println(F("[ACTION] Soil dry -> Water pump ON"));
        }
        else if (currentSoilMoisture > SOIL_WET_THRESHOLD && pumpState)
        {
            pumpState = false;
            relayWrite(PUMP_RELAY_PIN, false);
            Serial.println(F("[ACTION] Soil moist enough -> Water pump OFF"));
        }

        // ---- Cooling control ----
        if (currentTemp > TEMP_HIGH_THRESHOLD && !fanState)
        {
            fanState = true;
            relayWrite(FAN_RELAY_PIN, true);
            Serial.println(F("[ACTION] Temperature high -> Cooling fan ON"));
        }
        else if (currentTemp < TEMP_LOW_THRESHOLD && fanState)
        {
            fanState = false;
            relayWrite(FAN_RELAY_PIN, false);
            Serial.println(F("[ACTION] Temperature normal -> Cooling fan OFF"));
        }

        // ---- Report ----
        String dataStr = "T:" + String(currentTemp, 1) +
                         ",H:" + String(currentHumidity, 1) +
                         ",S:" + String(currentSoilMoisture) +
                         ",P:" + String(pumpState ? 1 : 0) +
                         ",F:" + String(fanState ? 1 : 0);

        Serial.print(F("[DATA] "));
        Serial.println(dataStr);
        espSerial.println(dataStr);
    }

    // ---- Listen for manual override commands from ESP8266 / mobile app ----
    processCommands();
}