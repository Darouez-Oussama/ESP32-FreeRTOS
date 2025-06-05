# ESP32 FreeRTOS LED Controller

A real-time embedded system project that demonstrates multitasking capabilities on an ESP32 microcontroller using FreeRTOS. The project implements concurrent control of two LEDs with serial communication for dynamic parameter adjustment.

## Features

- Real-time task management using FreeRTOS
- Concurrent operation of multiple LED control tasks
- Serial communication interface for command input
- Safe inter-task communication using mutex
- PWM-based LED brightness control
- Configurable LED blinking patterns

## Hardware Requirements

- ESP32 DOIT DevKit V1 board
- 2 LEDs
  - LED1 connected to GPIO2 (built-in LED)
  - LED2 connected to GPIO4
- USB cable for programming and serial communication

## Software Requirements

- PlatformIO IDE
- Arduino framework for ESP32
- FreeRTOS (included in ESP32 Arduino core)

## Setup and Installation

1. Clone this repository
2. Open the project in PlatformIO IDE
3. Connect your ESP32 board
4. Build and upload the project

## Usage

The system accepts commands via serial communication at 9600 baud rate. Two types of commands are supported:

1. LED1 control: Send "LED1" followed by delay value (in milliseconds)
2. LED2 control: Send "LED2" followed by PWM value (0-255)

### Example Commands:
- To set LED1 blink delay to 1 second: `LED1 1000`
- To set LED2 brightness to 50%: `LED2 128`

## Task Architecture

The system runs three concurrent tasks:

1. **receiveTask**: Handles serial communication and command parsing
2. **led1BlinkTask**: Controls the blinking pattern of LED1
3. **led2PWMTask**: Manages PWM-based brightness control of LED2

Inter-task communication is handled safely using FreeRTOS mutex.

