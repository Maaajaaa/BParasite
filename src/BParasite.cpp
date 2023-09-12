///////////////////////////////////////////////////////////////////////////////////////////////////
//BParasite.cpp
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
// 20221223 Added support for ATC1441 format
// 20230912 Modified to work with b-parasite by Maaajaaa
//
// ToDo: 
// -
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <BParasite.h>


/*!
 * \class MyAdvertisedDeviceCallbacks
 * 
 * \brief Callback for advertised device found during scan
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice* advertisedDevice) {
    log_d("Advertised Device: %s", advertisedDevice->toString().c_str());
    /*
     * Here we add the device scanned to the whitelist based on service data but any
     * advertised data can be used for your preffered data.
     */
    if (advertisedDevice->haveServiceData()) {
      /* If this is a device with data we want to capture, add it to the whitelist */
      if (advertisedDevice->getServiceData(NimBLEUUID("D2FC")) != "") {
        log_d("Adding %s to whitelist", std::string(advertisedDevice->getAddress()).c_str());
        NimBLEDevice::whiteListAdd(advertisedDevice->getAddress());
      }
    }
  }
};


// Set up BLE scanning
void BParasite::begin(void)
{
    NimBLEDevice::init("");
    _pBLEScan = BLEDevice::getScan(); //create new scan
    _pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    _pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
    _pBLEScan->setInterval(100);
    _pBLEScan->setFilterPolicy(BLE_HCI_SCAN_FILT_NO_WL);
    _pBLEScan->setWindow(99);  // less or equal setInterval value
}


// Get sensor data by running BLE device scan
unsigned BParasite::getData(uint32_t duration) {
    BLEScanResults foundDevices = _pBLEScan->start(duration, false /* is_continue */);
  
    Serial.println("Whitelist contains:");
    for (auto i=0; i<NimBLEDevice::getWhiteListCount(); ++i) {
        log_d("%s", NimBLEDevice::getWhiteListAddress(i).toString().c_str());
    }
  
    Serial.println("Assigning scan results...");
    for (unsigned i=0; i<foundDevices.getCount(); i++) {
        
        // Match all devices found against list of known sensors
        for (unsigned n = 0; n < _known_sensors.size(); n++) {
           log_v("Found: %s  comparing to: %s",
                  foundDevices.getDevice(i).getAddress().toString().c_str(), 
                  BLEAddress(_known_sensors[n]).toString().c_str());
            if (foundDevices.getDevice(i).getAddress() == BLEAddress(_known_sensors[n])) {
                log_d(" -> Match! Index: %d", n);
                data[n].valid = true;
                
                int len = foundDevices.getDevice(i).getServiceData().length();
                log_d("Length of ServiceData: %d", len);
                
                
                std::string bledata = foundDevices.getDevice(i).getServiceData();

                //temperature
                if(bledata[1] == 0x02){
                    data[n].temperature = bledata[3] << 8 | bledata[2];
                }else{    
                    Serial.println("varying (but valid as per standard) data format of temperature mismatch not implemented yet");
                }

                if(bledata[4] == 0x03){
                    // Relative air humidity as 2 bytes unsigned int
                    data[n].humidity = bledata[6] << 8 | bledata[5];
                }else{    
                    Serial.println("varying (but valid as per standarasd) data format of humiditz mismatch not implemented yet");
                }

                //bit 7 is 0x05 illuminance, followed by 3 bytes of illuminance, factor is 0.01 lux
                // Ambient light in lux
                data[n].illuminance = (bledata[8] << 16 | bledata[9] << 8 | bledata[10]);

                if(bledata[11] == 0x0c){
                    // Battery voltage in millivolts.
                    data[n].batt_voltage = bledata[13] << 8 | bledata[12];
                }else{    
                    Serial.println("varying (but valid as per standarasd) data format of voltage mismatch not implemented yet");
                }

                if(bledata[14] ==  0x14){
                    // Relative soil moisture in 0.01 %
                    data[n].soil_moisture = bledata[16] << 8 | bledata[15];
                }else{    
                    Serial.println("varying (but valid as per standarasd) data format of moisture mismatch not implemented yet");
                }                
                // Received Signal Strength Indicator [dBm]
                data[n].rssi = foundDevices.getDevice(i).getRSSI();
            }
        }
    }
    return foundDevices.getCount();
}

        
// Set all array members invalid
void BParasite::resetData(void)
{
    for (int i=0; i < _known_sensors.size(); i++) {
        data[i].valid = false;
    }
}
