# VFD Tube Clock

This project is a WiFi-connected clock that uses an IV-21 VFD (Vacuum Fluorescent Display) tube for the display and a custom PCB controlled by an ESP32 microcontroller. Built with PlatformIO using the Arduino framework, it should work with other ESP32 variants and Arduino-compatible MCUs.

## Design Overview

The PCB is designed around the Seeeduino ESP32-C3 devboard but can be adapted for other microcontrollers. The schematic and PCB design files were created using KiCad 9.0, with firmware development in VS Code with PlatformIO.

## Project Background

This is the first time I've had PCBs printed. I've been a software developer for many years and have been dabbling in electronics as a hobby over the past couple of years. I've been fascinated by vintage display technology - there's something captivating about the warm glow and classic look of old tube displays, plus those beautiful colors that display tubes like Nixie tubes and VFDs produce. I love the idea of bridging old and new technology by controlling these vintage displays with modern microcontrollers and WiFi connectivity.

**Current Status:** I'm actively refining the PCB design based on initial testing. The photos and videos below show the first prototype batch of 5 PCBs, while the KiCad files reflect ongoing improvements to the design.

## Project Photos

<div align="center">
<img src="images/20250615_112021 (Large).jpg" alt="VFD Clock Front View 1" width="300"/>
<img src="images/20250615_112439 (Large).jpg" alt="VFD Clock Front View 2" width="300"/>
</div>

## Web Interface

Though most of the source code I wrote myself, one of my weak areas is web UI design. I used an LLM (claude.ai) to generate a Soviet themed web UI for controlling the clock over WiFi. Soviet - because these IV-21 VFD tubes were manufactured in Soviet USSR.

**[View Web Interface Screenshots in images dir →](images/)**

## Video

[![VFD Clock Demo Video](https://img.youtube.com/vi/QAAFPpQU498/0.jpg)](https://youtu.be/QAAFPpQU498)

*Click to watch the demo video*

## How VFD Tubes Work

VFDs use phosphor materials, typically zinc oxide (ZnO) doped with zinc sulfide or other compounds, that emit light when struck by electrons from a heated filament cathode. The phosphor coating on the anode segments converts the electron energy into visible light, usually producing a characteristic blue-green glow, though the exact color depends on the specific phosphor formulation. The IV-21 specifically uses a zinc-based phosphor that creates its distinctive aqua-blue appearance.

The electrical requirements involve multiple voltages: a low filament voltage to heat the cathode (~2.5V DC for the IV-21), and higher anode/grid voltages to accelerate electrons toward the phosphor-coated segments (typically ~27V DC for the IV-21).

Multiplexing is the key technique that makes multi-digit VFD clocks practical - instead of driving all digits simultaneously (which would require complex, expensive circuitry), we rapidly cycle through each digit position, lighting only one at a time. By switching between digits faster than your eye can detect (typically 100Hz or higher), each digit appears continuously lit even though it's only powered for a fraction of the time. This creates the illusion of a stable, solid display across all digits while dramatically simplifying the driver electronics.

## Key Circuit Design Components

### Voltage Booster Circuit
The VFD needs 27V but our power supply only provides 5V, so we need a boost converter to step up the voltage. This circuit works like a voltage pump - it stores energy in an inductor coil when a transistor switches on, then releases that energy at a higher voltage when the transistor switches off. A diode ensures the boosted voltage flows in the right direction, and the ESP32 generates the rapid on/off switching signal (PWM) that makes it all work.
The circuit takes our 5V input and pumps it up to the 27V needed by the MAX6921 driver chip, which then distributes power to light up the VFD segments.

### MAX6921 VFD Driver
The MAX6921 IC serves as the interface between the microcontroller and VFD tube. It receives 3 bytes of data from the ESP32 telling it which segments to illuminate, then distributes the boosted 27V to the appropriate pins at precise timing intervals for proper message display. Multiple MAX6921s can be chained together for tubes with more digits or multiple tubes.

## Software Features

### Core Functionality
- **WiFi Connectivity:** Automatic connection to configured network
- **NTP Time Sync:** Retrieves accurate time from "pool.ntp.org"
- **Web Server:** Built-in HTTP server for device control
- **Multiple Display Modes:**
  - Time display mode
  - Custom message mode
  - "Flash message" mode with glitchy visual effects

### Flash Message Mode
As a novelty feature (which can be disabled), the clock occasionally displays random words that appear to "glitch" into existence—brief flashes of random characters before settling on the final message for a brief period, creating a subliminal effect. The word selection (easily changed in code) and styling pay homage to the Soviet heritage of these VFD tubes.

## Planned Improvements

### Hardware Enhancements
- **Better Voltage Regulation**
- **Multi-digit Support:** Extend design for tubes with more digits (IV-27 with 13 digits)

### Software Features
- **Better Wifi Management:** Default to AP mode for after compilation WiFi configuration
- **Text Scrolling:** Support for messages longer than 8 characters
- **Smart PWM Control:** Implement PID algorithm for voltage regulation
- **Enhanced Web UI:** Additional control options and settings
- **Button Integration:** Assign meaningful functions to hardware buttons which are not programmed to do anything currently

## Acknowledgments

This project was heavily inspired by and references the excellent work of:

- **[Tiny IV-21 VFD Clock](https://hackaday.io/project/167749-tiny-iv-21-vfd-clock)** - Original design inspiration. At least this was the first similar project that I came across. Uses an Atmega328P microcontroller and external (to the microcontroller module) RTC (real-time clock) component.
- **[Tiny IV-21 VFD Clock with ESP32](https://hackaday.io/project/202799-tiny-iv-21-vfd-clock-with-esp32)** - ESP32 implementation inspired from the above mentioned "Tiny IV-21 VFD Clock" project. Excludes the external RTC component and uses WiFi/Internet for automatic time synchronization.

Special thanks to the creators of these projects for sharing their knowledge and making this hobby accessible to others.

## Project Organization

### Where to find stuff
- [Main board PCB KiCad design files](./pcb/main-board/)
- [Sub-board PCB (VFD tube holder) design files](./pcb/sub-board/)
- [Main board PCB gerber files (ready to print PCBs)](./pcb-fab/gerber-main-board/)
- [An interactive BOM (Bill of Materials) for the main board](./pcb-fab/gerber-main-board/bom/)
- [Sub-board PCB - VFD tube holder (ready to print PCBs)](./pcb-fab/gerber-sub-board/)
- [Source code](./firmware/) (Built with PlatformIO for simplified dependency management and cross-platform development)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.
