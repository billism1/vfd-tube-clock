// MCP3221.h
#ifndef MCP3221_H
#define MCP3221_H

#include <Arduino.h>
#include <Wire.h>

class MCP3221 {
private:
  uint8_t _address;
  float _vref;
  uint16_t _resolution;
  uint16_t _maxValue;
  
public:
  // Constructor
  MCP3221(uint8_t address = 0x4D, float vref = 3.3, uint16_t resolution = 12);
  
  // Initialize the sensor (test connection)
  bool begin();
  
  // Read raw ADC value
  uint16_t readRaw();
  
  // Read voltage value
  float readVoltage();
  
  // Read multiple samples and return average voltage
  float readAverageVoltage(int samples = 10);
  
  // Check if device is connected
  bool isConnected();
  
  // Set reference voltage
  void setVref(float vref);
  
  // Get reference voltage
  float getVref();

  // Set ADC resolution
  void setResolution(uint16_t resolution);

  // Get ADC resolution
  uint16_t getResolution();
  
  // Get ADC resolution
  uint16_t getMaxValue();
  
  // Get I2C address
  uint8_t getAddress();
};

#endif