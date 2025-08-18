# VFD Tube Clock - Home Assistant Integration

This document provides instructions for integrating the VFD Tube Clock with Home Assistant for remote control and monitoring.

## Features

### VFD Filament Control
- **Remote Control**: Toggle the VFD filament on/off via Home Assistant
- **Physical Button Control**: On-board button support for local control
- **REST API**: HTTP endpoints for programmatic control and status monitoring
- **Real-time Status**: Live monitoring of all clock parameters

### Physical Button Support
- **Button 1 (SW1)**: Connected to D1/A1 pin - toggles VFD filament state
- **Button 2 (SW2)**: Connected to D0/A0 pin - toggles display mode (time/custom text)
- **Hardware Debouncing**: 50ms debounce delay prevents multiple triggers
- **Internal Pull-ups**: Buttons connect to ground when pressed (no external resistors needed)

### Home Assistant Integration Options
- **REST API Integration**: Direct HTTP communication with the clock
- **Status Monitoring**: Real-time status of all clock systems
- **Automation Support**: Scheduled on/off cycles and monitoring

## API Endpoints

### GET /status
Returns comprehensive status in JSON format:
```json
{
  "filament_state": true,
  "display_mode": "time",
  "custom_text": "HELLO   ",
  "flash_messages_enabled": true,
  "current_time": "14:30:25",
  "wifi_connected": true,
  "ip_address": "192.168.1.100",
  "uptime_ms": 123456789,
  "boost_duty_cycle": 110,
  "target_voltage": 30.0
}
```

### GET /filament/toggle
Toggles the VFD filament state and returns confirmation:
```json
{
  "filament_state": false,
  "message": "Filament disabled"
}
```

## Setup Instructions

### Prerequisites
- VFD Tube Clock with updated firmware flashed to ESP32
- Home Assistant instance running on your network
- Clock and Home Assistant on the same network

### Integration Setup

#### REST Integration (Recommended)
The VFD Tube Clock uses a REST API approach for Home Assistant integration, which preserves all custom functionality while providing full remote control.

1. **Identify Clock IP Address**
   - Check your router's DHCP client list
   - Monitor serial output during clock startup
   - Visit the clock's web interface to confirm connectivity

2. **Configure Home Assistant**
   - Copy the configuration from `home-assistant-config.yaml`
   - Add it to your Home Assistant `configuration.yaml` file
   - Replace `192.168.1.100` with your clock's actual IP address
   - Restart Home Assistant

3. **Verify Integration**
   - Check Developer Tools → States for new entities
   - Test the filament control switch
   - Monitor status sensors for real-time updates

## Physical Button Configuration

### Default Button Functions
- **Button 1 (SW1)**: Toggles VFD filament on/off
- **Button 2 (SW2)**: Toggles between time display and custom text display

### Hardware Requirements
- Tactile momentary switches connected to D1/A1 (SW1) and D0/A0 (SW2)
- Switches should connect the respective pins to ground when pressed
- No external pull-up resistors required (internal pull-ups enabled)

### Customizing Button Functions
Button handlers can be modified in the firmware source code:

```cpp
void handleButton1Press() {
  // Default: Toggle filament
  // Modify this function for custom behavior
}

void handleButton2Press() {
  // Default: Toggle display mode  
  // Modify this function for custom behavior
}
```

### Alternative Button Functions
Consider implementing these features:
- Brightness level cycling
- Flash message toggle
- Configuration mode entry
- Time zone cycling
- 12/24 hour format toggle
- Voltage target adjustment
- WiFi configuration reset

## Home Assistant Entities

After successful integration, the following entities will be available:

### Controls
- `switch.vfd_clock_filament_control` - Toggle VFD filament on/off
- `rest_command.vfd_clock_toggle_filament` - Direct API command

### Sensors
- `sensor.vfd_clock_status` - Main status sensor with current time
- `binary_sensor.vfd_clock_filament` - Filament state indicator
- `binary_sensor.vfd_clock_wifi_connected` - Network connectivity status
- `sensor.vfd_clock_display_mode` - Current display mode (time/custom)
- `sensor.vfd_clock_uptime` - Device uptime in seconds
- `sensor.vfd_clock_boost_duty_cycle` - Boost converter duty cycle
- `sensor.vfd_clock_target_voltage` - Target voltage setting

## Example Automations

### Scheduled Power Management
```yaml
automation:
  - alias: "VFD Clock - Night Mode"
    trigger:
      platform: time
      at: "23:00:00"
    action:
      service: switch.turn_off
      target:
        entity_id: switch.vfd_clock_filament_control

  - alias: "VFD Clock - Morning Activation"
    trigger:
      platform: time
      at: "07:00:00"
    action:
      service: switch.turn_on
      target:
        entity_id: switch.vfd_clock_filament_control
```

### Connectivity Monitoring
```yaml
automation:
  - alias: "VFD Clock - Offline Notification"
    trigger:
      platform: state
      entity_id: binary_sensor.vfd_clock_wifi_connected
      to: "off"
      for: "00:05:00"
    action:
      service: notify.persistent_notification
      data:
        title: "VFD Clock Offline"
        message: "VFD Clock has been offline for 5 minutes"
```

## Troubleshooting

### Common Issues

#### Clock Not Responding
- Check the clock's IP address hasn't changed
- Check network connectivity between Home Assistant and clock
- Test API endpoints directly: `curl http://[clock-ip]/status`

#### Entities Not Updating
- Check Home Assistant logs for REST sensor errors
- Adjust `scan_interval` in configuration if needed
- Check timeout settings for your network

#### Button Functionality Issues
- Check button hardware connections:
  - SW1 → D1/A1 pin (GPIO2)
  - SW2 → D0/A0 pin (GPIO3)
- Monitor serial output at 115200 baud for button press confirmations
- Ensure buttons connect pins to ground when pressed

### Debugging Tools

#### Serial Monitor
Enable debug output to monitor:
- Button press events
- Filament state changes  
- Network connectivity
- API request handling

#### Web Interface
Access the clock's built-in web interface:
- Navigate to `http://[clock-ip]/`
- Verify current status and settings
- Test manual control functions

#### API Testing
Test endpoints directly:
```bash
# Get current status
curl http://[clock-ip]/status

# Toggle filament
curl http://[clock-ip]/filament/toggle
```

## Hardware Requirements

### Circuit Board Connections
- **VFD Filament Control**: D2 pin (GPIO4) drives transistor circuit
- **Button 1 (SW1)**: D1/A1 pin (GPIO2) with internal pull-up
- **Button 2 (SW2)**: D0/A0 pin (GPIO3) with internal pull-up
- **Power**: Standard ESP32-C3 power requirements

### Network Requirements
- 2.4GHz WiFi network access
- Home Assistant on same network segment
- Stable network connection for reliable API communication

## Development and Customization

### Extending Functionality
The firmware provides a foundation for additional features:
- Custom button behaviors via `handleButton1Press()` and `handleButton2Press()`
- Additional API endpoints for new functionality
- Extended status reporting and monitoring
- Integration with other home automation systems

### Contributing
This is an open-source project. Contributions welcome for:
- Additional Home Assistant integrations
- Extended API functionality
- Bug fixes and improvements
- Documentation updates
