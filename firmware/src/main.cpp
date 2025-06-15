#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include "mcp3221.h"  // Abstraction for the MCP3221 ADC. This is a 12-bit ADC. Communicates over I2C.
#include "max6921.h"  // MAX6921 VFD driver class
#include "webui.h"
#include "credentials.h" // Wifi credentials.

// IV-21 Clock with an ESP32 MCU.
//
// This code is for a Seeeduino ESP32-C3 board with a MAX6921 VFD driver and MCP3221 ADC.
// This can be adapted for other ESP32 boards with minor changes.
// It initializes WiFi, time synchronization (via NTP server over Internet), PWM signal generation, and VFD display.
// It reads voltage from an MCP3221 ADC and adjusts the PWM duty cycle to maintain a target voltage for input into the VFD driver (MAX6921 IC).
// It displays the current time on a VFD and monitors the voltage every 40ms.

// Info about the Seeeduino XIAO ESP32-C3 board:
// https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/

// Time configuration.
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = -6 * 3600;                               // Central Time is GMT-5 (CST)
const int DAYLIGHT_OFFSET_SEC = 3600;                                // Daylight saving time offset (1 hour)
const int DST = 0;                                                   // Set to 1 to make daylight savings adjustments
bool timeSet = false;                                                // Flag to indicate if time has been set

// Flash message configuration
static bool flashMessageMode = true;                                 // Enable/disable flash message feature
const String flashMessages[] = {                                     // Array of messages to flash
  "COMRADE ",
  "SOVIET  ",
  "MARX    ",
  "LENIN   ",
  "STALIN  ",
  "WORKERS ",
  "UNITE   ",
  "POWER   ",
  "PARTY   ",
  "STATE   ",
  "RED DAWN",
  "FOR USSR",
  "GO RED  ",
  "OUR LAND"
};
const int numFlashMessages = sizeof(flashMessages) / sizeof(flashMessages[0]);
// const unsigned long FLASH_INTERVAL_MIN = 30000;                      // Minimum time between flashes (30 seconds)
// const unsigned long FLASH_INTERVAL_MAX = 120000;                     // Maximum time between flashes (2 minutes)
const unsigned long FLASH_INTERVAL_MIN = 7000;                       // Minimum time between flashes (in ms)
const unsigned long FLASH_INTERVAL_MAX = 15000;                      // Maximum time between flashes (in ms)
const unsigned long FLASH_DURATION = 500;                            // How long each message is displayed (in ms)
const unsigned long GLITCH_DURATION = 400;                           // How long the glitch effect lasts (in ms)
const unsigned long GLITCH_FRAME_TIME = 50;                          // Time between glitch frames (in ms)
unsigned long nextFlashTime = 0;                                     // When the next flash should occur
unsigned long flashEndTime = 0;                                      // When the current flash should end
unsigned long glitchEndTime = 0;                                     // When the glitch effect should end
unsigned long nextGlitchFrame = 0;                                   // When to show next glitch frame
bool isFlashing = false;                                             // Currently displaying a flash message
bool isGlitching = false;                                            // Currently showing glitch effect
bool isGlitchingOut = false;                                         // Currently showing glitch-out effect
int currentFlashIndex = 0;                                           // Index of current flash message
String currentGlitchText = "";                                       // Current glitch text being displayed

// VFD tube filament. Used to turn on the VFD tube filament heater. Applies voltage to transistor.
const int VFD_FILAMENT_PIN = D2;         // D2 pin is GPIO4 on Seeeduino ESP32-C3. Used to turn on the filament current to the VFD.

// LED
const int PWM_LED_INDICATOR_PIN = D6;    // D6 pin is GPIO21 on Seeeduino ESP32-C3. Used for PWM output to the brightness of the indicator LED/

// Voltage Boost PWM configuration.
const int PWM_VBOOST_PIN = D7;           // D7 pin is GPIO20 on Seeeduino ESP32-C3. Used for PWM output to the booster circuit driving the VFD.

// MAX6921 VFD IC configuration. This is the VFD driver IC that controls the IV-21 (or similar) VFD tube. Using SPI for Communication.
const int MAX6921_DIN_PIN = MOSI;        // MOSI pin (default) is GPIO10 on Seeeduino ESP32-C3. Used for SPI data input to MAX6921 IC.
const int MAX6921_CLK_PIN = SCK;         // SCK pin (default) is GPIO8 on Seeeduino ESP32-C3. Used for SPI clock signal to MAX6921 IC.
const int MAX6921_LOAD_PIN = D3;         // D3 pin is GPIO5 on Seeeduino ESP32-C3. Used for load signal to MAX6921 IC.

// MCP3221 ADC configuration. This is the ADC used to read the voltage from the booster circuit after the voltage divider. Using I2C for Communication.
const int MCP3221_SDA_PIN = SDA;        // SDA pin (default) is GPIO6 on Seeeduino ESP32-C3. Used for I2C communication with MCP3221 ADC.
const int MCP3221_SCL_PIN = SCL;        // SCL pin (default) is GPIO7 on Seeeduino ESP32-C3. Used for I2C communication with MCP3221 ADC.

// Voltage Boost PWM configuration.
const int MAX_VBOOST_PWM_DUTY_CYCLE = 220;                             // Maximum duty cycle for PWM signal (8-bit resolution, 0-255 range). In testing, above about 85% (220 for 8-bit value) yielded diminishing returns.
const int MIN_VBOOST_PWM_DUTY_CYCLE = 5;                               // Minimum duty cycle to keep the VFD lit (8-bit resolution, 0-255 range)
const int VBOOST_PWM_RESOLUTION = 8;                                   // 8-bit resolution (0-255 values)
const int VBOOST_PWM_DUTY_MAX_VALUE = pow(2, VBOOST_PWM_RESOLUTION);   // Convert bit resolution to max value (256 for 8-bit resolution)
const int VBOOST_PWM_FREQUENCY = 25000;                                // Default frequency in Hz
const float VBOOST_TARGET_VOLTAGE_V = 30;                              // 30 Volts
int boostDutyCycle = 110;                                              // Start with moderate duty cycle for IV-21

// Indicator LED PWM configuration.
const int LED_PWM_BIT_RESOLUTION = 8;                                  // 8-bit resolution (0-255 values)
const int LED_PWM_FREQUENCY_HZ = 5000;                                 // Frequency in Hz
const int LED_PWM_DUTY_CYCLE = 10;                                     // 128 = 50% duty cycle for 8-bit resolution (0-255)

// Voltage monitoring variables.
const int VOLTAGE_UPDATE_INTERVAL = 10;                                // Print voltage every 10 checks
const double VOLTAGE_MULTIPLIER = 390000.0 / 20000.0;                  // Voltage divider ratio based on the resistors used in the voltage divider circuit.
int voltageUpdateCounter = 0;

// Web server configuration
WebServer server(80);
bool isDisplayTimeMode = true;  // true = time, false = custom text
String customText = "HELLO   ";  // Default custom text (8 characters max)

// MCP3221 configuration. The MCP3221 is a 12-bit ADC with I2C interface.
const uint8_t MCP3221_ADDRESS = 0x4E;                                  // Typical default is 0x4D, but that did not work for this ADC.
const float MCP3221_REFERENCE_VOLTAGE_V = 3.3;                         // Reference voltage for MCP3221 in volts.
const float MCP3221_RESOLUTION = 12;                                   // MCP3221 is a 12-bit ADC, so the resolution is 2^12 = 4096.
MCP3221 mcp3221(MCP3221_ADDRESS, MCP3221_REFERENCE_VOLTAGE_V, MCP3221_RESOLUTION);

// Initialize digit pins array
uint8_t DIGIT_PINS[] = {
  15,  // Digit 1:     MAX6921 pin 7  (OUT-15)     (Adapter pin 1)      (IV-21 pin 6:  Digit 8 Grid)   
  1,   // Digit 2:     MAX6921 pin 25 (OUT-1)      (Adapter pin 11)     (IV-21 pin 15: Digit 7 Grid)   
  13,  // Digit 3:     MAX6921 pin 9  (OUT-13)     (Adapter pin 3)      (IV-21 pin 7:  Digit 6 Grid)   
  2,   // Digit 4:     MAX6921 pin 24 (OUT-2)      (Adapter pin 12)     (IV-21 pin 14: Digit 5 Grid)   
  14,  // Digit 5:     MAX6921 pin 8  (OUT-14)     (Adapter pin 4)      (IV-21 pin 8:  Digit 4 Grid)   
  0,   // Digit 6:     MAX6921 pin 26 (OUT-0)      (Adapter pin 10)     (IV-21 pin 13: Digit 3 Grid)      
  12,  // Digit 7:     MAX6921 pin 10 (OUT-12)     (Adapter pin 6)      (IV-21 pin 9:  Digit 2 Grid)   
  11,  // Digit 8:     MAX6921 pin 11 (OUT-11)     (Adapter pin 8)      (IV-21 pin 12: Digit 1 Grid)   
  10,  // Digit 9:     MAX6921 pin 12 (OUT-10)     (Adapter pin 7)      (IV-21 pin 11: Symbols Grid) (TODO: Currently not used/supported in this code)
  17,  // Digit 10:    MAX6921 pin 5  (OUT-17)     (NOT CONNECTED)
  18,  // Digit 11:    MAX6921 pin 4  (OUT-18)     (NOT CONNECTED)
  19,  // Digit 12:    MAX6921 pin 3  (OUT-19)     (NOT CONNECTED)
};

// TODO: Support digit 9 - symbols grid.
// TODO: Support digit 9 - symbols grid.
// TODO: Support digit 9 - symbols grid.

// Initialize segment pins array
uint8_t SEGMENT_PINS[] = {
  16,  // Segment A:             MAX6921 pin 6  (OUT-16)    (Adapter pin 2)       (IV-21 pin 5:  Segment A Anode)
  8,   // Segment B:             MAX6921 pin 18 (OUT-8)     (Adapter pin 19)      (IV-21 pin 3:  Segment B Anode)
  5,   // Segment C:             MAX6921 pin 21 (OUT-5)     (Adapter pin 15)      (IV-21 pin 18: Segment C Anode)
  3,   // Segment D:             MAX6921 pin 23 (OUT-3)     (Adapter pin 13)      (IV-21 pin 17: Segment D Anode)
  6,   // Segment E:             MAX6921 pin 20 (OUT-6)     (Adapter pin 16)      (IV-21 pin 19: Segment E Anode (Short lead))
  9,   // Segment F:             MAX6921 pin 17 (OUT-9)     (Adapter pin 20)      (IV-21 pin 4:  Segment Segment F Anode / Dot Symbol Anode)
  7,   // Segment G:             MAX6921 pin 19 (OUT-7)     (Adapter pin 17)      (IV-21 pin 2:  Segment G Anode / - Symbol Anode)
  4,   // Segment H (period):    MAX6921 pin 22 (OUT-4)     (Adapter pin 14)      (IV-21 pin 16: Segment DP Anode)
};

// Calculate array sizes
// const uint8_t NUM_DIGITS = sizeof(DIGIT_PINS) / sizeof(DIGIT_PINS[0]);
// const uint8_t NUM_SEGMENTS = sizeof(SEGMENT_PINS) / sizeof(SEGMENT_PINS[0]);
const uint8_t NUM_DIGITS = 8;
const uint8_t NUM_SEGMENTS = 8;

// MAX6921 VFD Display instance
MAX6921 vfdDisplay(MAX6921_DIN_PIN, MAX6921_CLK_PIN, MAX6921_LOAD_PIN, DIGIT_PINS, NUM_DIGITS, SEGMENT_PINS, NUM_SEGMENTS);

// Prototypes
void initWifi();
void initTime();
void initIndicatorLedPwmSignal(int dutyCycle);
void printLocalTime();
String getFormattedTime();

// Voltage monitoring/adjustment functions
void initADC();
void initBoostPwmSignal();
int updateBoostDutyCycle(int currentDuty, bool printInfo = false);
void checkVoltage();

void updateDisplay();
void updateTimeDisplay();

// Flash message functions
void initFlashMessages();
void updateFlashMessages();
void scheduleNextFlash();
String generateGlitchText();

// Web server handlers
void initWebServer();
void updateCustomDisplay();
void handleRoot();
void handleToggleMode();
void handleToggleMessageMode();
void handleSetText();
void handleNotFound();

void setup()
{ 
  Serial.begin(115200);

#ifdef DEBUG
  // Wait for serial to be ready (helpful for debugging)
  delay(3000);
#endif

  Serial.println("Beginning setup...");

  // Init Indicator LED PWM
  Serial.println("Init Indicator LED PWM Signal (on)...");
  initIndicatorLedPwmSignal(LED_PWM_DUTY_CYCLE);

  Serial.println("Init WiFi...");
  initWifi();

  Serial.println("Init Time...");
  initTime();

  // Initialize web server
  Serial.println("Init Web Server...");
  initWebServer();

  // Turn on VFD Filament heater
  Serial.println("Turning on VFD Filament...");
  pinMode(VFD_FILAMENT_PIN, OUTPUT);
  digitalWrite(VFD_FILAMENT_PIN, HIGH); // Turn on the filament (5V before filament resistor) to heat the VFD tube

  // Initialize I2C with explicit pins
  Serial.println("Init I2C for ADC...");
  Wire.begin(MCP3221_SDA_PIN, MCP3221_SCL_PIN);
  delay(100);

  // Init MCP3221 ADC
  Serial.println("Init MCP3221 ADC (using I2C)...");
  initADC();
  delay(100);

  // Init Voltage Booster PWM
  Serial.println("Init Boost PWM Signal...");
  initBoostPwmSignal();
  delay(500);
  
  // Init VFD
  Serial.println("Init MAX6921 VFD IC (using SPI)...");
  vfdDisplay.begin();

  // Init Flash Messages
  Serial.println("Init Flash Messages...");
  initFlashMessages();

  // Init Indicator LED PWM (turn off to indicate setup complete)
  Serial.println("Init Indicator LED PWM Signal (off)...");
  initIndicatorLedPwmSignal(0);

  Serial.println("Setup complete.");
  Serial.print("Web interface available at: http://");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Handle web server requests
  server.handleClient();

  // Update flash messages (only if enabled and in time mode)
  if (flashMessageMode && isDisplayTimeMode) {
    updateFlashMessages();
  }

  // Update VFD display
  updateDisplay();

  // Check and adjust voltage
  checkVoltage();
  
  // Refresh the display
  vfdDisplay.refreshDisplay();
}

void initWifi()
{
  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_passphrase);
  Serial.println("Connecting to WiFi...");
  
  int count = 0;

  // Connection status monitoring
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    count++;

    if (count >= 20) // Timeout after 10 seconds
    {
      Serial.println("Failed to connect to WiFi. Check credentials and network.");
      return;
    }
  }

  Serial.println();
  Serial.print("Connected to WiFi! IP address: ");
  Serial.println(WiFi.localIP());
}

void initTime()
{
  Serial.println("Initializing time from NTP server...");
  
  // Configure time with NTP server and timezone
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  
  // Wait for time to be set
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 10)
  {
    Serial.println("Failed to obtain time, retrying...");
    delay(500);
    attempts++;
  }
  
  if (attempts >= 10)
  {
    Serial.println("Failed to obtain time after 10 attempts!");
    timeSet = false;
  }
  else
  {
    Serial.println("Time synchronized successfully!");
    printLocalTime();
    timeSet = true;
  }
}

void initWebServer()
{
  // Define web server routes
  server.on("/", handleRoot);
  server.on("/toggle", handleToggleMode);
  server.on("/toggleFlashMessage", handleToggleMessageMode);
  server.on("/settext", HTTP_POST, handleSetText);
  server.onNotFound(handleNotFound);
  
  // Start the server
  server.begin();
  Serial.println("Web server started");
}

void initADC()
{
  if (mcp3221.begin())
  {
    Serial.println("MCP3221 found and ready!");
    Serial.print("I2C Address: 0x");
    Serial.println(mcp3221.getAddress(), HEX);
    Serial.print("Reference Voltage: ");
    Serial.print(mcp3221.getVref(), 2);
    Serial.println(" V");
    Serial.print("Resolution: ");
    Serial.println(mcp3221.getResolution());
    Serial.print("Max Value: ");
    Serial.println(mcp3221.getMaxValue());
  }
  else
  {
    Serial.println("MCP3221 not found. Check connections and address.");
  }
}

void initBoostPwmSignal()
{
  // Set up LEDC PWM on the pin
  ledcAttach(PWM_VBOOST_PIN, VBOOST_PWM_FREQUENCY, VBOOST_PWM_RESOLUTION);
  // Set initial duty cycle
  ledcWrite(PWM_VBOOST_PIN, boostDutyCycle);
}

void initIndicatorLedPwmSignal(int dutyCycle)
{
  // Set up LEDC PWM on the pin
  ledcAttach(PWM_LED_INDICATOR_PIN, LED_PWM_FREQUENCY_HZ, LED_PWM_BIT_RESOLUTION);
  // Set initial duty cycle
  ledcWrite(PWM_LED_INDICATOR_PIN, dutyCycle);
}

void initFlashMessages()
{
  // Schedule the first flash message
  scheduleNextFlash();
  Serial.println("Flash messages initialized");
  Serial.print("Flash message mode: ");
  Serial.println(flashMessageMode ? "ENABLED" : "DISABLED");
  Serial.print("Number of flash messages: ");
  Serial.println(numFlashMessages);
}

void scheduleNextFlash()
{
  // Generate a random interval between min and max
  unsigned long interval = random(FLASH_INTERVAL_MIN, FLASH_INTERVAL_MAX + 1);
  nextFlashTime = millis() + interval;
  
  Serial.print("Next flash scheduled in ");
  Serial.print(interval / 1000);
  Serial.println(" seconds");
}

void updateFlashMessages()
{
  unsigned long currentTime = millis();
  
  // Check if we're currently glitching out (transition from flash message back to time)
  if (isGlitchingOut)
  {
    // Update glitch-out frames
    if (currentTime >= nextGlitchFrame)
    {
      currentGlitchText = generateGlitchText();
      nextGlitchFrame = currentTime + GLITCH_FRAME_TIME;
    }
    
    // Check if glitch-out duration has ended
    if (currentTime >= glitchEndTime)
    {
      isGlitchingOut = false;
      Serial.println("Glitch-out effect ended, returning to time display");
      scheduleNextFlash(); // Schedule the next flash
    }

    return;
  }
  
  // Check if we're currently glitching in (transition to flash message)
  if (isGlitching && !isGlitchingOut)
  {
    // Update glitch-in frames
    if (currentTime >= nextGlitchFrame)
    {
      currentGlitchText = generateGlitchText();
      nextGlitchFrame = currentTime + GLITCH_FRAME_TIME;
    }
    
    // Check if glitch-in duration has ended
    if (currentTime >= glitchEndTime)
    {
      isGlitching = false;
      isFlashing = true;
      flashEndTime = currentTime + FLASH_DURATION;
      Serial.println("Glitch-in effect ended, showing flash message");
    }

    return;
  }
  
  // Check if we're currently flashing a message
  if (isFlashing)
  {
    // Check if flash duration has ended
    if (currentTime >= flashEndTime)
    {
      isFlashing = false;
      isGlitchingOut = true;  // Start glitch-out effect
      glitchEndTime = currentTime + GLITCH_DURATION;
      nextGlitchFrame = currentTime;
      Serial.println("Flash message ended, starting glitch-out effect");
    }
    
    // If still flashing, the display will show the flash message
    return;
  }
  
  // Check if it's time for the next flash
  if (!isFlashing && !isGlitching && !isGlitchingOut && currentTime >= nextFlashTime)
  {
    // Start glitching in first
    isGlitching = true;
    glitchEndTime = currentTime + GLITCH_DURATION;
    nextGlitchFrame = currentTime;
    
    // Select a random message from the array
    currentFlashIndex = random(0, numFlashMessages);
    
    Serial.print("Starting glitch-in effect before flash message: \"");
    Serial.print(flashMessages[currentFlashIndex]);
    Serial.println("\"");
  }
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  
  // Print formatted time
  Serial.print("Current time: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

String getFormattedTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    return "NO TIME";
  }
  
  char timeString[16];
  strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
  return String(timeString);
}

int updateBoostDutyCycle(int currentDutyCycle, bool printInfo)
{
  if (!mcp3221.isConnected())
  {
    Serial.println("MCP3221 ADC disconnected!");
    return currentDutyCycle;
  }

  //uint16_t rawValue = mcp3221.readRaw();
  float voltage = mcp3221.readVoltage();
  float voltageConverted = voltage * VOLTAGE_MULTIPLIER;
  
  // Adjust duty cycle based on voltage comparison
  int newDuty = currentDutyCycle;

  if (voltageConverted < VBOOST_TARGET_VOLTAGE_V)
  {
      newDuty = min(MAX_VBOOST_PWM_DUTY_CYCLE, newDuty + 1);
  }
  else if (voltageConverted > VBOOST_TARGET_VOLTAGE_V)
  {
      newDuty = max(MIN_VBOOST_PWM_DUTY_CYCLE, newDuty - 1);
  }

  // Update PWM output
  ledcWrite(PWM_VBOOST_PIN, newDuty);

#ifdef DEBUG
  if (printInfo)
  {
    Serial.println("-----------------");
    Serial.print("Voltage Factor: ");
    Serial.println(VOLTAGE_MULTIPLIER);
    // Serial.print("MCP Value: ");
    // Serial.println(rawValue);
    Serial.print("Low voltage measurement: ");
    Serial.print(voltage, 3);
    Serial.println(" V");
    Serial.print("High voltage (converted): ");
    Serial.print(voltageConverted, 3);
    Serial.println(" V");
    Serial.print("Target voltage: ");
    Serial.print(VBOOST_TARGET_VOLTAGE_V);
    Serial.println(" V");
    Serial.print("Previous duty cycle: ");
    Serial.print(currentDutyCycle);
    Serial.print(" (");
    Serial.print((currentDutyCycle * 100.0) / VBOOST_PWM_DUTY_MAX_VALUE, 1);
    Serial.println("%)");
    Serial.print("Updated duty cycle: ");
    Serial.print(newDuty);
    Serial.print(" (");
    Serial.print((newDuty * 100.0) / VBOOST_PWM_DUTY_MAX_VALUE, 1);
    Serial.println("%)");
    Serial.print("Min duty cycle: ");
    Serial.print(MIN_VBOOST_PWM_DUTY_CYCLE);
    Serial.print(" (");
    Serial.print((MIN_VBOOST_PWM_DUTY_CYCLE * 100.0) / VBOOST_PWM_DUTY_MAX_VALUE, 1);
    Serial.println("%)");
    Serial.print("Max duty cycle: ");
    Serial.print(MAX_VBOOST_PWM_DUTY_CYCLE);
    Serial.print(" (");
    Serial.print((MAX_VBOOST_PWM_DUTY_CYCLE * 100.0) / VBOOST_PWM_DUTY_MAX_VALUE, 1);
    Serial.println("%)");
    Serial.print("Frequency: ");
    Serial.print(VBOOST_PWM_FREQUENCY);
    Serial.println(" Hz");
    Serial.println("--------");
  }
#endif

  return newDuty;
}

void checkVoltage()
{
  static unsigned long lastVoltageCheck = 0;

  // Check and adjust VFD voltage every 40ms (25 times per second)
  if (millis() - lastVoltageCheck > 200)
  {
    bool printInfo = (voltageUpdateCounter >= VOLTAGE_UPDATE_INTERVAL);
    boostDutyCycle = updateBoostDutyCycle(boostDutyCycle, printInfo);
    
    if (printInfo)
    {
      voltageUpdateCounter = 0;
    }
    else
    {
      voltageUpdateCounter++;
    }
    
    lastVoltageCheck = millis();
  }
}

void updateDisplay()
{
  // Check if we should display glitch effect (either in or out)
  if (flashMessageMode && isDisplayTimeMode && (isGlitching || isGlitchingOut))
  {
    // Display the current glitch text
    vfdDisplay.setDisplayText(currentGlitchText.c_str());
  }
  // Check if we should display flash message (only in time mode and when flash is enabled)
  else if (flashMessageMode && isDisplayTimeMode && isFlashing)
  {
    // Display the current flash message
    vfdDisplay.setDisplayText(flashMessages[currentFlashIndex].c_str());
  }
  else if (isDisplayTimeMode)
  {
    updateTimeDisplay();
  }
  else
  {
    updateCustomDisplay();
  }
}

void updateTimeDisplay()
{
  static unsigned long lastUpdate = 0;
  static struct tm currentTimeInfo;

  // Update time every 100ms
  if (millis() - lastUpdate >= 100)
  {
    if (getLocalTime(&currentTimeInfo))
    {
      int hour = currentTimeInfo.tm_hour;
      int minute = currentTimeInfo.tm_min;
      int second = currentTimeInfo.tm_sec;
      
      // Apply DST adjustment if enabled
      if (DST == 1)
      {
        // DST in US: Second Sunday in March to first Sunday in November
        if (currentTimeInfo.tm_yday > 67 && currentTimeInfo.tm_yday < 307)
        {
          hour = (hour + 1) % 24;
        }
      }

      // Calculate fractions of a second from system uptime
      //int centiseconds = (millis() / 100) % 10;  // 0-9 (tenths of seconds)
      //int centiseconds = (millis() / 10) % 100;  // 0-999 (hundredths of seconds)
      
      // Format time string for 8 digits.
      char timeStr[12];
      snprintf(timeStr, sizeof(timeStr), "%02d-%02d-%02d", hour, minute, second);
      //snprintf(timeStr, sizeof(timeStr), "%02d.%02d.%02d.%d", hour, minute, second, centiseconds); // 00.00.00.0
      
      vfdDisplay.setDisplayText(timeStr);
    }
    
    lastUpdate = millis();
  }
  else if (!timeSet)
  {
    vfdDisplay.setDisplayText("--ERR-- ");
  }
}

void updateCustomDisplay()
{
  static unsigned long lastUpdate = 0;
  
  // Update custom text every 100ms (same rate as time for consistency)
  if (millis() - lastUpdate >= 100)
  {
    vfdDisplay.setDisplayText(customText.c_str());
    lastUpdate = millis();
  }
}

// Web server handlers - Comrade VFD Clock Control Interface
void handleRoot()
{
  String html = getWebUI(isDisplayTimeMode, customText, flashMessageMode, getFormattedTime());
  server.send(200, "text/html", html);
}

void handleToggleMode()
{
  isDisplayTimeMode = !isDisplayTimeMode;
  Serial.println("Display mode toggled to: " + String(isDisplayTimeMode ? "Time" : "Custom"));
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleToggleMessageMode()
{
  flashMessageMode = !flashMessageMode;
  Serial.println("Flash Message mode toggled to: " + String(flashMessageMode ? "True" : "False"));
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleSetText()
{
  if (server.hasArg("text")) {
    customText = server.arg("text");
    // Pad with spaces if less than 8 characters
    while (customText.length() < 8) {
      customText += " ";
    }
    // Truncate if more than 8 characters
    if (customText.length() > 8) {
      customText = customText.substring(0, 8);
    }
    Serial.println("Custom text set to: \"" + customText + "\"");
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleNotFound()
{
  server.send(404, "text/plain", "Not Found");
}

String generateGlitchText()
{
  // Characters that can appear in glitch effect
  const char glitchChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-=+*#@$%&!?";
  const int numGlitchChars = sizeof(glitchChars) - 1; // -1 to exclude null terminator
  
  String glitchText = "";
  
  // Generate 8 random characters for the glitch effect
  for (int i = 0; i < 8; i++)
  {
    // Randomly decide if this position should be glitched or show a space
    if (random(0, 100) < 70) // 70% chance to show a glitch character
    {
      int randomIndex = random(0, numGlitchChars);
      glitchText += glitchChars[randomIndex];
    } else
    {
      glitchText += " "; // 30% chance to show a space
    }
  }
  
  return glitchText;
}