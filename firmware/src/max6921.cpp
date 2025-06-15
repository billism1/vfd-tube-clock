#include "max6921.h"

MAX6921::MAX6921(int dinPin, int clkPin, int loadPin,
                 const uint8_t* digitPinMap, uint8_t numDigits,
                 const uint8_t* segmentPinMap, uint8_t numSegments) 
  : dinPin(dinPin), clkPin(clkPin), loadPin(loadPin),
    numDigits(numDigits), numSegments(numSegments),
    spiSettings(500000, MSBFIRST, SPI_MODE0), currentDigit(0), 
    lastRefresh(0)
{
  // Validate input parameters
  if (numDigits > MAX_DIGITS) {
    this->numDigits = MAX_DIGITS;
  }
  if (numSegments > MAX_SEGMENTS) {
    this->numSegments = MAX_SEGMENTS;
  }
  
  // Copy digit pins array
  for (int i = 0; i < this->numDigits; i++) {
    digitPins[i] = digitPinMap[i];
  }
  
  // Copy segment pins array  
  for (int i = 0; i < this->numSegments; i++) {
    segmentPins[i] = segmentPinMap[i];
  }
  
  // Initialize display buffer
  strcpy(displayBuffer, "        ");
  for (int i = 0; i < numDigits; i++) {
    digitsWithDots[i] = false;
  }
  
  // Initialize character map
  initCharMap();
}

bool MAX6921::begin()
{
  // Initialize the LOAD pin
  pinMode(loadPin, OUTPUT);
  digitalWrite(loadPin, HIGH);  // Start with LOAD high
  
  // Initialize SPI
  SPI.begin(clkPin, -1, dinPin, -1);  // CLK, MISO (not used), MOSI, SS (not used)
  
  // Clear all outputs
  writeToMAX6921(0);
  delay(50);
  
  return true;
}

void MAX6921::initCharMap()
{
  // Initialize all to 0
  for (int i = 0; i < 128; i++) {
    charMap[i] = 0;
  }
  
  // Numbers
  charMap['0'] = 0b00111111;  // ABCDEF
  charMap['1'] = 0b00000110;  // BC
  charMap['2'] = 0b01011011;  // ABDEG
  charMap['3'] = 0b01001111;  // ABCDG
  charMap['4'] = 0b01100110;  // BCFG
  charMap['5'] = 0b01101101;  // ACDFG
  charMap['6'] = 0b01111101;  // ACDEFG
  charMap['7'] = 0b00000111;  // ABC
  charMap['8'] = 0b01111111;  // ABCDEFG
  charMap['9'] = 0b01101111;  // ABCDFG
  
  // Letters (A-Z)
  charMap['A'] = 0b01110111;  // ABCEFG
  charMap['B'] = 0b01111100;  // CDEFG
  charMap['C'] = 0b00111001;  // ADEF
  charMap['D'] = 0b01011110;  // BCDEG
  charMap['E'] = 0b01111001;  // ADEFG
  charMap['F'] = 0b01110001;  // AEFG
  charMap['G'] = 0b00111101;  // ACDEF
  charMap['H'] = 0b01110110;  // BCEFG
  charMap['I'] = 0b00110000;  // EF
  charMap['J'] = 0b00011110;  // BCDE
  charMap['K'] = 0b01110110;  // BCEFG (same as 'H')
  charMap['L'] = 0b00111000;  // DEF
  charMap['M'] = 0b00010101;  // ACE (approximation)
  charMap['N'] = 0b00110111;  // ABCEF
  charMap['O'] = 0b00111111;  // ABCDEF
  charMap['P'] = 0b01110011;  // ABEFG
  charMap['Q'] = 0b01100111;  // ABCFG
  charMap['R'] = 0b00110011;  // ABEF
  charMap['S'] = 0b01101101;  // ACDFG
  charMap['T'] = 0b01111000;  // DEFG
  charMap['U'] = 0b00111110;  // BCDEF
  charMap['V'] = 0b00011110;  // BCDE (approximation)
  charMap['W'] = 0b00101010;  // BDG (approximation)
  charMap['X'] = 0b01110110;  // BCEFG (approximation) (same as 'H')
  charMap['Y'] = 0b01101110;  // BCDFG
  charMap['Z'] = 0b01011011;  // ABDEG (same as '2')
  
  // Special characters
  charMap[' '] = 0b00000000;  // All off
  charMap['-'] = 0b01000000;  // G only
  charMap['_'] = 0b00001000;  // D only
  charMap['.'] = 0b10000000;  // H only (decimal point)
  charMap[':'] = 0b01001000;  // DG
  charMap['='] = 0b01001000;  // DG (same as colon)
  charMap['!'] = 0b10000110;  // BC with decimal
  charMap['?'] = 0b11010011;  // ABGEH
  charMap['\''] = 0b00000010; // B only
  charMap['"'] = 0b00100010;  // BF
  charMap['<'] = 0b01110000;  // EFG
  charMap['>'] = 0b01000011;  // ABG
  charMap['['] = 0b00111001;  // ADEF
  charMap[']'] = 0b00001111;  // ABCD
  charMap['/'] = 0b01010010;  // BEG
  charMap['\\'] = 0b01100100; // CFG
}

void MAX6921::writeToMAX6921(uint32_t data)
{
  // Sends 3 bytes to the MAX6921
  // Stores them as [byte0, byte1, byte2] where:
  // byte0 contains bits for pins 16-23 (but reversed)
  // byte1 contains bits for pins 8-15 (but reversed)
  // byte2 contains bits for pins 0-7 (but reversed)
  
  // Extract bytes from our data
  uint8_t byte0 = (data >> 16) & 0xFF;  // Bits 23-16
  uint8_t byte1 = (data >> 8) & 0xFF;   // Bits 15-8
  uint8_t byte2 = data & 0xFF;          // Bits 7-0
  
  // Begin SPI transaction
  SPI.beginTransaction(spiSettings);
  
  // Pull LOAD low to start data transfer
  digitalWrite(loadPin, LOW);

  SPI.transfer(byte0);  // Send bits 23-16 first
  SPI.transfer(byte1);  // Send bits 15-8
  SPI.transfer(byte2);  // Send bits 7-0 last
  
  // Pull LOAD high to latch the data
  digitalWrite(loadPin, HIGH);
  
  // End SPI transaction
  SPI.endTransaction();
}

void MAX6921::displayDigit(uint8_t digitIndex, uint8_t segments)
{
  if (digitIndex >= numDigits) return;  // Bounds check
  
  // We need to create the full pin activation pattern
  uint8_t bytes[3] = {0, 0, 0};
  
  // Set the digit pin (cathode)
  uint8_t digitPin = digitPins[digitIndex];
  uint8_t true_order = 23 - digitPin;
  uint8_t target_byte = true_order / 8;
  uint8_t target_bit = true_order % 8;
  bytes[target_byte] |= (1 << (7 - target_bit));
  
  // Now handle segments
  // segment_order = "ABCDEFGH" maps to segmentPins array
  // So if segments has bit 0 set, we activate segmentPins[0] (segment A)
  // If segments has bit 1 set, we activate segmentPins[1] (segment B), etc.
  
  for (int segIndex = 0; segIndex < numSegments; segIndex++) {
    if (segments & (1 << segIndex)) {
      uint8_t segmentPin = segmentPins[segIndex];
      uint8_t seg_true_order = 23 - segmentPin;
      uint8_t seg_target_byte = seg_true_order / 8;
      uint8_t seg_target_bit = seg_true_order % 8;
      bytes[seg_target_byte] |= (1 << (7 - seg_target_bit));
    }
  }
  
  // Now convert bytes array to uint32_t for writeToMAX6921
  uint32_t data = ((uint32_t)bytes[0] << 16) | ((uint32_t)bytes[1] << 8) | bytes[2];
  
  writeToMAX6921(data);
}

void MAX6921::refreshDisplay()
{
  // Multiplex the display at ~1kHz (1ms per digit)
  if (micros() - lastRefresh >= 1000) {
    // Turn off all outputs first to prevent ghosting
    //writeToMAX6921(0);

    // Get the character to display
    char ch = displayBuffer[currentDigit];
    uint8_t segments = 0;

    // Look up segments for this character
    if (ch >= 0 && ch < 128) {
      segments = charMap[(uint8_t)ch];
    }

    // Check if the next character is a decimal point
    if (currentDigit < numDigits && digitsWithDots[currentDigit]) {
      segments |= 0x80;  // Turn on decimal point
    }

    // Display the digit (including spaces as blank)
    displayDigit(currentDigit, segments);

    currentDigit = (currentDigit + 1) % numDigits;

    lastRefresh = micros();
  }
}

void MAX6921::setDisplayText(const char* text)
{
  // First, copy text to a temporary buffer, handling decimal points
  char tempBuffer[numDigits + 1];
  bool tempDigitsWithDots[numDigits + 1];
  int textIndex = 0;
  int bufferIndex = 0;
  
  while (bufferIndex < numDigits && text[textIndex] != 0x00) {
    tempBuffer[bufferIndex] = toupper(text[textIndex]);
    
    // Skip decimal points in positioning (they share digit with previous character)
    if (text[textIndex + 1] == '.') {
      tempDigitsWithDots[bufferIndex] = true;
      textIndex++;
    } else {
      tempDigitsWithDots[bufferIndex] = false;
    }

    bufferIndex++;
    textIndex++;
  }
  
  // Pad with spaces
  while (bufferIndex < numDigits) {
    tempBuffer[bufferIndex] = ' ';
    tempDigitsWithDots[bufferIndex] = false;
    bufferIndex++;
  }

  tempBuffer[numDigits] = '\0';
  tempDigitsWithDots[numDigits] = false;
  
  // Now reverse the string into the display buffer
  // This is specific to how the VFD digits are wired
  for (int i = 0; i < numDigits; i++) {
    displayBuffer[i] = tempBuffer[numDigits - 1 - i];
    digitsWithDots[i] = tempDigitsWithDots[numDigits - 1 - i];
  }

  displayBuffer[numDigits] = '\0';
  digitsWithDots[numDigits] = false;
}