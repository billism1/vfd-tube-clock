# VFD Tube Clock

A WiFi-enabled VFD (Vacuum Fluorescent Display) tube clock built with the Seeeduino ESP32-C3 microcontroller, designed for the IV-21 VFD tube with web-based control interface.

Note that I am currently making PCB design modifications. The below photos and video are from the first few PCBs I had printed for testing the initial design. The main board's KiCad project does not currently match this due to some changes I am in the process of making. Though, the software flashed to the ESP32 will work with the old and the new PCB designs.

## Project Photos

<div align="center">
<img src="images/20250615_112021 (Large).jpg" alt="VFD Clock Front View 1" width="300"/>
<img src="images/20250615_112439 (Large).jpg" alt="VFD Clock Front View 2" width="300"/>
</div>

## Web Interface

The device features a Soviet-themed web interface for controlling display modes and settings. 

**[View Web Interface Screenshots in images dir →](images/)**

## Video

[![VFD Clock Demo Video](https://img.youtube.com/vi/QAAFPpQU498/0.jpg)](https://youtu.be/QAAFPpQU498)

*Click to watch the demo video*

## Overview

This project creates a retro-styled digital clock using Soviet-era VFD tubes. The clock connects to WiFi to sync time via NTP servers and features a web interface for configuration and control. The design combines the nostalgic appeal of vintage display technology with modern microcontroller capabilities.

## How VFD Tubes Work

Vacuum Fluorescent Displays (VFDs) work by using phosphor-coated segments that glow when struck by electrons. The IV-21 tube contains multiple digit segments and a heated filament cathode. The display appears solid to the human eye through rapid multiplexing - segments and digits are turned on and off intermittently faster than human perception can detect, creating the illusion of a continuous, stable display across all digits.

## Hardware Specifications

### Compatible Hardware
**VFD Tubes:**
- **Primary:** IV-21 VFD tube (8 digits)
- **Compatible:** IC-18 VFD tube (larger form factor, same digit count)
- **Future:** Should be aasily modifiable for IV-27 VFD tube (13 digits)

**Microcontrollers:**
- **Primary:** Seeeduino ESP32-C3. The PCB is designed specifically for this module. You can use surface mount soldering or pin headers.
- **Other ESP32 variants:** Code can be adapted, but PCB would need modification/redesign

### Key Components
- **Microcontroller:** Seeeduino ESP32-C3
- **VFD Driver:** MAX6921 IC for precise segment control
- **Voltage Booster:** Custom inductor-based circuit (5V → 27V)
- **Filament Control:** Transistor-controlled heater circuit (~2.4V)
- **User Interface:** Two tactile switch buttons, status LED
- **Connectivity:** WiFi for NTP time synchronization

## Circuit Design

### MAX6921 VFD Driver
The MAX6921 IC serves as the interface between the microcontroller and VFD tube. It receives 3 bytes of data from the ESP32 and distributes the correct voltages to the appropriate pins at precise timing intervals for proper message display. Multiple MAX6921s can be chained together for tubes with more digits.

### Voltage Booster Circuit
A simple but effective boost converter design using:
- Inductor for energy storage
- Transistor for switching
- Diode for rectification
- PWM signal generation and adjustment via ESP32

The circuit boosts the regulated 5V supply to 27V, which is fed into the MAX6921's VPP pin to power the VFD segments.

### Filament Heater Control
The VFD filament heater is controlled via a transistor switch and can be turned on/off as needed. A trim potentiometer allows fine-tuning of the ~2.4V filament voltage for optimal tube operation.

### PCB Design
- **Designed in KiCad 9.0**
- **Main Board:** Custom redesigned PCB
- **VFD Holder PCB:** Recreated from reference projects

## Software Features

### Core Functionality
- **WiFi Connectivity:** Automatic connection to configured network
- **NTP Time Sync:** Retrieves accurate time from "pool.ntp.org"
- **Web Server:** Built-in HTTP server for device control
- **Multiple Display Modes:**
  - Time display mode
  - Custom message mode
  - "Flash message" mode with glitchy visual effects

### Web Interface
Features a Soviet-themed design aesthetic matching the vintage tube origin:
- Time mode control
- Custom message input
- Flash message configuration (can be disabled)
- Soviet-inspired UI styling

### Flash Message Mode
A fun "gag" feature that rapidly displays random words with glitchy effects, creating a subliminal message appearance. The word array and styling reflect the Soviet heritage of the VFD tubes.

## Planned Improvements

### Hardware Enhancements
- **Zener Diode Protection:** Add overvoltage protection to voltage booster output
- **Multi-digit Support:** Extend design for tubes with more digits (IV-27 with 13 digits)
- **Button Functionality:** Implement features for the two tactile switches

### Software Features
- **Text Scrolling:** Support for messages longer than 8 characters
- **Smart PWM Control:** Implement PID algorithm for voltage regulation
- **Enhanced Web UI:** Additional control options and settings
- **Button Integration:** Assign meaningful functions to hardware buttons

## Acknowledgments

This project was heavily inspired by and references the excellent work of:

- **[Tiny IV-21 VFD Clock](https://hackaday.io/project/167749-tiny-iv-21-vfd-clock)** - Original design inspiration. Uses an Atmega328P MCU and RTC hardware.
- **[Tiny IV-21 VFD Clock with ESP32](https://hackaday.io/project/202799-tiny-iv-21-vfd-clock-with-esp32)** - ESP32 implementation inspired from the above mentioned "Tiny IV-21 VFD Clock" project. Excludes the external RTC (real-time clock) component and uses wifi and Internet for automatic time acquisition.
- **[GitHub Repository](https://github.com/cinchcircuits/iv-21-clock-esp32)** - GitHub repo for the above mentioned "Tiny IV-21 VFD Clock with ESP32" project.

Special thanks to the creators of these projects for sharing their knowledge and making this hobby accessible to others.

And... yes - I used an LLM (claude.ai) to create this readme file from key points. But I did make manual edits. =)

## Getting Started

### Prerequisites
- VS Code with PlatformIO extension
- Basic soldering skills
- Components as listed in the BOM (Bill of Materials)

### Installation
1. Clone this repository
2. Open the `firmware` folder in VS Code
3. PlatformIO should automatically detect the project configuration
4. Configure WiFi credentials in the code
5. Build and upload to your Seeeduino ESP32-C3 using PlatformIO
6. Assemble the hardware according to the schematic

### Configuration
1. Power on the device
2. Connect to the same WiFi network
3. Access the web interface via the device's IP address
4. Configure display modes and messages as desired

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

---

*Built with ❤️ for the love of vintage display technology and modern microcontrollers.*