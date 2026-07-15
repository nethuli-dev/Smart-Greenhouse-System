/*
 * Configuration file for greenhouse system
 * Contains WiFi credentials and system settings
 */

#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials - Replace with your network details
const char *WIFI_SSID = "Your_Network_Name";
const char *WIFI_PASSWORD = "Your_WiFi_Password";

// Web server configuration
const int WEB_SERVER_PORT = 80;

// Update interval for web dashboard (milliseconds)
const unsigned long WEB_UPDATE_INTERVAL = 2000;

// Sensor calibration values
const int SOIL_DRY_RAW = 1023; // Raw ADC value when dry
const int SOIL_WET_RAW = 300;  // Raw ADC value when wet

#endif