# ESP32 FreeRTOS LED Controller

A real-time embedded system project that demonstrates multitasking capabilities on an ESP32 microcontroller using FreeRTOS. The project implements concurrent control of two LEDs with serial communication for dynamic parameter adjustment.

## Features

- Real-time task management using FreeRTOS
- Concurrent operation of multiple LED control tasks
- Serial communication interface for command input
- Safe inter-task communication using mutex
- PWM-based LED brightness control
- Configurable LED blinking patterns

## Implementation Details

### Mutex and Task Scheduling with FreeRTOS

This project utilizes FreeRTOS for multitasking, managing three independent tasks with a mutex for safe data sharing:
- **Mutex**: A `SemaphoreHandle_t Key` is created using `xSemaphoreCreateMutex()` to protect the shared `commandPacket` array. The mutex ensures that only one task accesses the array at a time, preventing data corruption during concurrent reads and writes. Tasks use `xSemaphoreTake()` to acquire the mutex and `xSemaphoreGive()` to release it, with `portMAX_DELAY` allowing indefinite waiting if the mutex is locked.
- **Task Scheduling**: FreeRTOS employs a preemptive scheduler that assigns priorities to tasks. The `receiveTask` runs at priority 2 (higher), while `led1BlinkTask` and `led2PWMTask` run at priority 1 (lower). The scheduler dynamically switches between tasks based on priority and availability, with `vTaskDelay()` yielding CPU time to allow other tasks to run. Each task has a 4096-byte stack to handle its operations without overflow.

### How the Code Works

The system initializes and manages three tasks to control LEDs based on serial commands:

1. **Setup**:
   - Initializes `LED1_PIN` (2) and `LED2_PIN` (4) as outputs.
   - Configures Serial at 9600 baud and waits for readiness.
   - Creates a mutex (`Key`) for shared resource protection.
   - Launches three tasks with `xTaskCreate`: `receiveTask` (priority 2), `led1BlinkTask` (priority 1), and `led2PWMTask` (priority 1), each with a 4096-byte stack.

2. **receiveTask**:
   - Continuously monitors the Serial buffer.
   - Acquires the mutex, reads up to 32 bytes into `commandPacket`, and prints the received data for debugging.
   - Releases the mutex and delays 100ms to prevent CPU hogging.
   - Note: The current implementation reads raw bytes but doesn’t parse commands; it assumes future logic will interpret "LED1" or "LED2" prefixes.

3. **led1BlinkTask**:
   - Maintains a `localDelay` (default 500ms) for LED1 blinking.
   - Acquires the mutex to check if `commandPacket` starts with "LED1"; if so, updates `localDelay` with the 5th byte (assumed as a delay value).
   - Releases the mutex, toggles `LED1_PIN` high/low with the adjusted delay using `vTaskDelay`.
   - Runs indefinitely, updating the blink rate based on commands.

4. **led2PWMTask**:
   - Maintains a `localPWM` value (default 128, 50% brightness).
   - Acquires the mutex to check if `commandPacket` starts with "LED2"; if so, updates `localPWM` with the 5th byte (assumed as a PWM value, 0-255).
   - Releases the mutex, applies the PWM value to `LED2_PIN` with `analogWrite`, and delays 100ms to regulate task execution.
   - Runs continuously, adjusting brightness dynamically.

5. **Loop**: Left empty, as all functionality is handled by FreeRTOS tasks.

The code uses a shared `commandPacket` array, protected by the mutex, to pass commands between tasks. However, the current command parsing (e.g., `strncmp` with a single byte for delay/PWM) is simplistic and assumes the 5th byte directly represents the value, which may need refinement for robust integer parsing (e.g., multi-byte values).

## Usage

The system accepts commands via serial communication at 9600 baud rate. Two types of commands are supported:

1. **LED1 control**: Send "LED1" followed by a delay value (in milliseconds, currently interpreted as the 5th byte).
2. **LED2 control**: Send "LED2" followed by a PWM value (0-255, currently interpreted as the 5th byte).

### Example Commands:
- To set LED1 blink delay to 1 second: `LED1 1000` (only the 5th byte, e.g., `100`, will be used).
- To set LED2 brightness to 50%: `LED2 128` (only the 5th byte, e.g., `128`, will be used).
