# SAP-1 Computer Implementation using Arduino
![](https://github.com/Sierrastdio/arduinoSAP-1/blob/main/Images/SAP-1_Architecture.PNG)    
A hardware implementation of the SAP-1 (Simple As Possible) computer architecture using 11 Arduino Uno boards, demonstrating fundamental computer architecture concepts through physical serial communication.

## Project Overview

This project implements a complete SAP-1 computer system where each major component is realized as a separate Arduino board. The boards communicate via serial connections to simulate the data and control buses of a real computer system.

## Architecture

### Components

The system consists of 11 Arduino modules:

- **CLK** - Clock Generator
- **CTR** - Controller (manages instruction cycles)
- **PC** - Program Counter
- **MAR** - Memory Address Register
- **RAM** - Random Access Memory
- **IR** - Instruction Register
- **A** - Accumulator Register
- **ALU** - Arithmetic Logic Unit
- **B** - B Register
- **OUT** - Output Register
- **DISPLAY** - Display Module (16x2 lcd)

### Communication Architecture

#### Serial Ports Configuration

Each Arduino uses NeoSWSerial library for software serial communication:

- **busSerial**: D5 (RX), D6 (TX) - Data bus
- **ctrSerial**: D3 (RX), D4 (TX) - Control bus
- **bSerial**: D7 (RX), D8 (TX) - ALU B input

All NeoSWSerial ports operate at **19200 baud**.

#### Hardware Serial Communication

3 critical connections use Arduino's hardware serial (Serial) for reliable, clock-independent operation:

- **MAR → RAM**: Hardware serial for memory addressing
- **IR → CTR**: Hardware serial for instruction decoding
- **A → ALU**: Hardware serial for stable communication with ALU

These connections operate continuously at 19200 baud, allowing the RAM and CTR modules to respond immediately regardless of clock state.

## Instruction Cycles

### LDA Cycle (Load A)
```
1. IR → CTR (determine instruction)
2. PC CLEAR
3. PC → MAR
4. *MAR → RAM (hardware serial)
5. PC INC
6. RAM → IR
7. *IR → CTR (decode next instruction)
8. IR → MAR (lower 4 bits = memory address)
9. *MAR → RAM
10. RAM → A
```

### LDB Cycle (Load B)
```
1. PC → MAR
2. *MAR → RAM (hardware serial)
3. PC INC
4. RAM → IR
5. *IR → CTR (decode next instruction)
6. IR → MAR (lower 4 bits)
7. *MAR → RAM
8. RAM → B
```

### ADD+OUT Cycle
```
1. A → ALU (via aSerial)
2. B → ALU (via bSerial)
3. ALU ADD
4. ALU → A (via busSerial)
5. PC → MAR
6. *MAR → RAM (hardware serial)
7. PC INC
8. RAM → IR
9. *IR → CTR (decode next instruction)
10. A → OUT
```

*Note: Steps marked with `*` use hardware serial and are clock-independent.*

## Design Principles

### Bus Protocol

- **Data bus** and **control bus** are physically separate
- Modules only enable TX when transmitting, then immediately return to INPUT/listen mode
- This enforces sequential bus access and prevents collisions
- CTR module maintains continuous RX listening on ctrSerial for control signals

### Data Format

- All data transmissions use **binary format** (`0b` prefix)
- Both 8-bit and 4-bit data are transmitted in binary

### Synchronization

- Each instruction cycle ends with one idle clock cycle
- Hardware serial communications (marked with `*`) operate independently of clock
- This allows RAM and CTR to respond immediately when addressed

## Hardware Requirements

- 10x Arduino Uno boards
- Multiple breadboards for circuit organization
- Jumper wires for serial connections
- Power supply capable of powering all boards
- 16x2 lcd(I2C) for display output

## Software Requirements

- Arduino IDE
- NeoSWSerial library

## Installation

1. Clone this repository:
```bash
git clone https://github.com/sierrastdio/arduinoSAP-1.git
```

2. Install the NeoSWSerial library in Arduino IDE:
   - Sketch → Include Library → Manage Libraries
   - Search for "NeoSWSerial" and install

3. Upload the respective code to each Arduino board:
   - Open each module's `.ino` file
   - Select the correct board and port
   - Upload

## Module Structure

Each module follows a consistent structure:

```cpp
#include <NeoSWSerial.h>

// Serial port definitions
NeoSWSerial busSerial(5, 6);    // RX, TX
NeoSWSerial ctrSerial(3, 4);    // RX, TX

void setup() {
  // Initialize serial ports
  // Configure pins
}

void loop() {
  // Listen for control signals
  // Execute module-specific operations
  // Transmit data when required
}
```
## Learning Objectives

This project demonstrates:

- Computer architecture fundamentals
- Instruction fetch-decode-execute cycle
- Register operations and data flow
- Bus communication protocols
- Synchronous vs. asynchronous operation
- Hardware/software interfacing

## Limitations

- Limited instruction set (LDA, LDB, ADD+OUT)
- 4-bit memory addressing
- 8-bit data width
- Single ALU operation (addition)

## Future Enhancements

- STM32 version: Port this implementation to STM32 microcontrollers for additional hardware serial ports

## License

This project has no license. All rights are reserved by the author(s). You may view and fork this repository, but you may not reproduce, distribute, or create derivative works without explicit permission.

## Acknowledgments

- Based on the SAP-1 (Simple As Possible) computer architecture designed by Albert Paul Malvino.
- This project uses the [NeoSWSerial](https://github.com/SlashDevin/NeoSWSerial) library by SlashDevin for software serial communication.

---

**Note**: This is an educational project designed to teach computer architecture concepts through hands-on hardware implementation.
