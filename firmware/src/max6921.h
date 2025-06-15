#ifndef MAX6921_H
#define MAX6921_H

#include <Arduino.h>
#include <SPI.h>

// Maximum supported digits and segments (for array sizing)
#define MAX_DIGITS 12
#define MAX_SEGMENTS 8

class MAX6921 {
private:
  // Pin definitions
  int dinPin;
  int clkPin;
  int loadPin;
  
  // Dynamic configuration
  uint8_t numDigits;
  uint8_t numSegments;
  
  // Pin mappings (externally provided)
  uint8_t digitPins[MAX_DIGITS];
  uint8_t segmentPins[MAX_SEGMENTS];
  
  // SPI settings
  SPISettings spiSettings;
  
  // Display management
  uint8_t currentDigit;
  unsigned long lastRefresh;
  char displayBuffer[MAX_DIGITS + 1];
  bool digitsWithDots[MAX_DIGITS + 1];
  
  // Character mapping
  uint8_t charMap[128];
  
  // Internal methods
  void initCharMap();
  void writeToMAX6921(uint32_t data);
  void displayDigit(uint8_t digitIndex, uint8_t segments);

public:
  // Constructor now takes pin mappings as parameters
  MAX6921(int dinPin, int clkPin, int loadPin,
          const uint8_t* digitPinMap, uint8_t numDigits,
          const uint8_t* segmentPinMap, uint8_t numSegments);
  
  // Public methods
  bool begin();
  void refreshDisplay();
  void setDisplayText(const char* text);
  
  // Getters for configuration
  uint8_t getNumDigits() const { return numDigits; }
  uint8_t getNumSegments() const { return numSegments; }
};

#endif