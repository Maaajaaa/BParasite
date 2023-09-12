///////////////////////////////////////////////////////////////////////////////////////////////////
// BParasite_Client.ino
//
// Bluetooth low energy thermometer/hygrometer sensor client for MCUs supported by NimBLE-Arduino.
// For bparasite sensors using the BTHome protocol https://github.com/rbaron/b-parasite
//
// 
//
// Based on:
// ---------
// NimBLE-Arduino by h2zero (https://github.com/h2zero/NimBLE-Arduino)
// LYWSD03MMC.py by JsBergbau (https://github.com/JsBergbau/MiTemperature2)
// ATC_MiThermometer by matthias-bs (https://github.com/matthias-bs/ATC_MiThermometer)
//
// created: 11/2022
//
//
// MIT License
//
// Copyright (c) 2022 Matthias Prinke
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// History:
//
// 20221123 Created
// 20230912 Modified to work with b-parasite by Maaajaaa
//
// To Do: 
// -
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "BParasite.h"

const int scanTime = 5; // BLE scan time in seconds
// List of known sensors' BLE addresses
std::vector<std::string> knownBLEAddresses = {"01:23:45:67:89:AB","01:23:45:67:89:CD"};

BParasite parasite(knownBLEAddresses);

// Iteration counter
int iteration = 0;

void setup() {


    Serial.begin(115200);
    // Print free heap before initialization
    Serial.println("Starting:    Free heap is " + String(ESP.getFreeHeap()));
    
    // Initialization
    parasite.begin();
    
    // Print free heap after initialization
    Serial.println("Initialized: Free heap is " + String(ESP.getFreeHeap()));
}

void loop() {
    // Set sensor data invalid
    parasite.resetData();
    
    // Get sensor data - run BLE scan for <scanTime>
    unsigned found = parasite.getData(scanTime);

    for (int i=0; i < parasite.data.size(); i++) {  
        if (parasite.data[i].valid) {
            Serial.println();
            Serial.printf("Sensor %d: %s\n", i, knownBLEAddresses[i].c_str());
            Serial.printf("%.2f°C\n", parasite.data[i].temperature/100.0);
            Serial.printf("%.2f%% humidity\n", parasite.data[i].humidity/100.0);
            Serial.printf("%.3fV\n",  parasite.data[i].batt_voltage/1000.0);
            Serial.printf("%.2f%% soil moisture\n", parasite.data[i].soil_moisture/100.0);
            Serial.printf("%.2flux\n", parasite.data[i].illuminance/100.0);
            Serial.printf("%ddBm\n",  parasite.data[i].rssi);
            Serial.println();
         }
    }
    Serial.println("BLE Devices found (total): " + String(found));

    // Delete results from BLEScan buffer to release memory
    parasite.clearScanResults();
    
    // Print iteration counter and free heap
    Serial.println("Iteration " + String(iteration++) + " - Free heap is " + String(ESP.getFreeHeap()));
    Serial.println("---");

    delay(2000);
}
