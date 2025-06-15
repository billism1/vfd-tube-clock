# MAX6921 VFD Driver Function Documentation

This document provides comprehensive documentation for all functions in the MAX6921 VFD (Vacuum Fluorescent Display) driver library.

## Overview

The MAX6921 is a 20-output, 76V, serial-interfaced VFD tube driver. This library provides functions to initialize, configure, and control VFD displays using 7-segment characters with support for multiple digits and chips.

## Static Variables and Arrays

### `segment_definitions[128]`
- **Type**: `static const char*`
- **Purpose**: Maps ASCII characters (0-127) to their corresponding segment patterns
- **Format**: Each string contains letters A-H representing which segments to illuminate
- **Example**: `'8'` maps to `"AFBGECD"` (all segments except H/period)

## Functions

### `init_segment_definitions()`
```c
static void init_segment_definitions()
```

**Purpose**: Initializes the segment definitions array with character-to-segment mappings.

**Behavior**:
- Uses static initialization flag to prevent multiple initializations
- Clears all 128 array positions to NULL initially
- Maps digits 0-9, letters A-Z, and special characters to their 7-segment representations
- Supports segments A-G (standard 7-segment) plus H (decimal point/period)

**Segment Layout**:
```
 AAA
F   B
 GGG
E   C
 DDD  H
```

**Character Examples**:
- `'0'`: `"DEFABC"` (segments D,E,F,A,B,C)
- `'8'`: `"AFBGECD"` (all segments except H)
- `'A'`: `"EGFABC"` (segments E,G,F,A,B,C)
- `' '`: `""` (blank/space)

---

### `vfd_init()`
```c
void vfd_init(vfd_t *vfd, pin_config_t *digits, int num_digits, 
              pin_config_t *segments, int num_segments)
```

**Purpose**: Initializes the VFD display structure and hardware configuration.

**Parameters**:
- `vfd`: Pointer to VFD structure to initialize
- `digits`: Array of pin configurations for digit control
- `num_digits`: Number of digits in the display
- `segments`: Array of pin configurations for segment control (A-H)
- `num_segments`: Number of segments (typically 8)

**Behavior**:
- Initializes segment definitions (calls `init_segment_definitions()`)
- Clears and sets up the VFD structure
- Sets default SPI frequency to 500kHz
- Copies digit and segment pin configurations
- Builds list of unique CS (Chip Select) pins from both digits and segments
- Initializes CS pins as OUTPUT and sets them HIGH (inactive)
- Builds internal pin mapping tables
- Sets initial display buffer to " Init "
- Assigns default SPI interface

**Side Effects**:
- Modifies GPIO pin modes for CS pins
- Initializes internal data structures

---

### `vfd_init_spi()`
```c
void vfd_init_spi(vfd_t *vfd, uint32_t frequency)
```

**Purpose**: Initializes the SPI interface with specified frequency.

**Parameters**:
- `vfd`: Pointer to initialized VFD structure
- `frequency`: SPI clock frequency in Hz

**Behavior**:
- Sets the SPI frequency in the VFD structure
- Calls `SPI.begin()` to initialize the SPI peripheral

**Notes**:
- Must be called after `vfd_init()`
- Typical frequencies: 100kHz - 1MHz depending on VFD requirements

---

### `build_pin_maps()`
```c
void build_pin_maps(vfd_t *vfd)
```

**Purpose**: Builds internal pin mapping tables for efficient data transmission.

**Parameters**:
- `vfd`: Pointer to initialized VFD structure

**Behavior**:
- Creates digit pin maps for each display digit
- Creates segment pin maps for each segment (A-H)
- Handles bit order reversal (MAX6921 uses reverse bit order 19-0)
- Maps logical pin numbers to physical byte/bit positions
- Organizes data by chip index for multi-chip displays

**Algorithm**:
1. For each digit/segment, find corresponding chip index
2. Calculate true bit order: `abs(pin_number - 23)`
3. Calculate target byte: `true_order / 8`
4. Calculate target bit: `true_order % 8`
5. Set appropriate bit in mapping table

**Data Structure**: Creates 3D arrays `[digit/segment][chip][byte]` for fast lookup

---

### `vfd_set_text()`
```c
void vfd_set_text(vfd_t *vfd, const char *text)
```

**Purpose**: Sets the text to be displayed on the VFD.

**Parameters**:
- `vfd`: Pointer to VFD structure
- `text`: Null-terminated string to display

**Behavior**:
- Copies input text to internal print buffer
- Limits text length to `MAX_PRINT_BUFFER - 2` characters
- Converts all characters to uppercase
- Appends a space character to prevent brightness issues on last character
- Null-terminates the buffer

**Notes**:
- Text is processed during `vfd_draw()` calls
- Supports decimal points when '.' follows a character
- Unsupported characters display as blank

---

### `concat_pins()`
```c
void concat_pins(uint8_t result[MAX_CHIPS][BYTES_PER_CHIP], 
                 uint8_t src1[MAX_CHIPS][BYTES_PER_CHIP], 
                 uint8_t src2[MAX_CHIPS][BYTES_PER_CHIP])
```

**Purpose**: Combines two pin data arrays using modulo arithmetic.

**Parameters**:
- `result`: Output array for combined data
- `src1`: First source array
- `src2`: Second source array

**Behavior**:
- Performs element-wise addition of two 3D arrays
- Uses modulo 256 to prevent overflow
- Combines digit and segment data for final output

**Formula**: `result[chip][byte] = (src1[chip][byte] + src2[chip][byte]) % 256`

**Use Case**: Combines digit selection pins with segment data pins

---

### `get_char_segments()`
```c
void get_char_segments(vfd_t *vfd, char c, uint8_t result[MAX_CHIPS][BYTES_PER_CHIP])
```

**Purpose**: Converts a character to its corresponding segment pin data.

**Parameters**:
- `vfd`: Pointer to VFD structure
- `c`: Character to convert
- `result`: Output array for segment pin data

**Behavior**:
- Clears result array initially
- Looks up character in segment definitions (ASCII 0-127 only)
- For each segment letter in the definition:
  - Maps A-H to segment indices 0-7
  - ORs the corresponding segment pin data into result
- Handles undefined characters by returning empty result

**Segment Mapping**:
- 'A' → index 0, 'B' → index 1, ..., 'H' → index 7

**Output**: 3D array with bits set for required segment pins across all chips

---

### `vfd_draw()`
```c
void vfd_draw(vfd_t *vfd)
```

**Purpose**: Main display refresh function that outputs current text to the VFD hardware.

**Parameters**:
- `vfd`: Pointer to initialized VFD structure

**Behavior**:
- Processes each digit position in sequence
- Handles decimal point processing (periods modify previous character)
- Combines digit selection and segment data
- Transmits data via SPI to all chips
- Includes debug output (when DEBUG is defined)

**Algorithm**:
1. For each digit position:
   - Get digit selection pin data
   - Get character segment data from text buffer
   - Handle decimal point if next character is '.'
   - Combine digit and segment data
   - Transmit 24 bits (3 bytes) to each chip via SPI

**SPI Protocol**:
- Uses MSB first, SPI Mode 0
- CS pin LOW during transmission, HIGH when idle
- Sends 3 bytes per chip (24 bits total)

**Debug Features**:
- Prints display buffer contents every 2 seconds
- Shows hex data being sent to each chip
- Helps troubleshoot display issues

**Performance**: Optimized for real-time display refresh in main loop

## Usage Example

```c
// Initialize VFD with 4 digits and 8 segments
pin_config_t digits[4] = {{cs_pin: 10, pin_number: 0}, ...};
pin_config_t segments[8] = {{cs_pin: 10, pin_number: 8}, ...};

vfd_t my_vfd;
vfd_init(&my_vfd, digits, 4, segments, 8);
vfd_init_spi(&my_vfd, 500000);

// Display text
vfd_set_text(&my_vfd, "12.34");

// In main loop
while(1) {
    vfd_draw(&my_vfd);
    delay(1);
}
```

## Technical Notes

- **Multiplexing**: Display uses time-division multiplexing, requiring continuous refresh
- **Bit Order**: MAX6921 requires reverse bit order (pin 19 = bit 0)
- **Decimal Points**: Handled by combining '.' with previous character
- **Multi-chip**: Supports multiple MAX6921 chips for larger displays
- **SPI Speed**: 500kHz default, adjustable based on display requirements