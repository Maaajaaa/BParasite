# BParasite Arduino library

[![License: MIT](https://img.shields.io/badge/license-MIT-green)](https://github.com/matthias-bs/ATC_MiThermometer/blob/main/LICENSE)

Arduino BLE Client Library based on [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) for receiving b-parasite BLE data (soil moisture, temperature, humidity, illuminance as well as battery voltage).

This project allows to receive data from a battery-powered bluetooth low energy soil moisture sensor [b-parasite](https://github.com/rbaron/b-parasite). 

The b-parasite firmware sends the sensor and status data as BLE advertisements, i.e. multiple clients can receive and use the sensor data. This is done using the [BTHome Protcol](bthome.io)

This project is heavily based on the [ATC_MiThermometer](https://github.com/matthias-bs/ATC_MiThermometer) - with all its benefits inherited from [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino).

## Example
```
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

```

### Similar Projectss

There's also an implementation of this sensor for ESPHome which I've modified to work with currrent data called [esphome_b_parasite_component](https://github.com/Maaajaaa/esphome_b_parasite_component)

### Todos

- Adding the library to the Arduino and Platformio library collections
- implementing a non-blocking example like [this](https://github.com/matthias-bs/ATC_MiThermometer/blob/main/examples/ATC_MiThermometer_Nonblocking/ATC_MiThermometer_Nonblocking.ino)
