#include "MCP3221.h"

MCP3221::MCP3221(uint8_t address, float vref, uint16_t resolution)
{
  _address = address;
  _vref = vref;
  setResolution(resolution);
}

bool MCP3221::begin()
{
  Wire.beginTransmission(_address);
  return (Wire.endTransmission() == 0);
}

uint16_t MCP3221::readRaw()
{
  Wire.requestFrom(_address, 2);
  
  if (Wire.available() >= 2)
  {
    uint16_t result = Wire.read() << 8; // High byte
    result |= Wire.read();              // Low byte
    return result & 0x0FFF;             // MCP3221 is 12-bit
  }
  
  return 0; // Return 0 if read failed
}

float MCP3221::readVoltage()
{
  uint16_t raw = readRaw();
  return (float)raw * _vref / _maxValue;
}

float MCP3221::readAverageVoltage(int samples)
{
  if (samples <= 0) samples = 1;
  
  long sum = 0;
  for (int i = 0; i < samples; i++)
  {
    sum += readRaw();
    delay(1); // Small delay between readings
  }
  
  uint16_t avgRaw = sum / samples;
  return (float)avgRaw * _vref / _maxValue;
}

bool MCP3221::isConnected()
{
  Wire.beginTransmission(_address);
  return (Wire.endTransmission() == 0);
}

void MCP3221::setVref(float vref)
{
  _vref = vref;
}

float MCP3221::getVref()
{
  return _vref;
}

void MCP3221::setResolution(uint16_t resolution)
{
  _resolution = resolution;
  _maxValue = pow(2, resolution); // Calculate max value based on resolution
}

uint16_t MCP3221::getResolution()
{
  return _resolution;
}

uint16_t MCP3221::getMaxValue()
{
  return _maxValue;
}

uint8_t MCP3221::getAddress()
{
  return _address;
}