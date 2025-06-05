#include <Arduino.h>
#include <FreeRTOS.h>

#define LED1_PIN  2   // Built-in LED for blinking
#define LED2_PIN  4   // Another LED for PWM control

// Global variables with mutex
uint8_t commandPacket[32] = {0};
SemaphoreHandle_t Key;

// Task handles
TaskHandle_t receiveTaskHandle = NULL;
TaskHandle_t led1TaskHandle = NULL;
TaskHandle_t led2TaskHandle = NULL;

// Function prototypes
void receiveTask(void *pvParameters);
void led1BlinkTask(void *pvParameters);
void led2PWMTask(void *pvParameters);

void setup() {
  // Initialize pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  // Initialize serial
  Serial.begin(9600);
  while(!Serial) { ; }  // Wait for serial to be ready

  // Create mutex
  Key = xSemaphoreCreateMutex();
  if (Key == NULL) {
    Serial.println("Mutex creation failed!");
    while(1); // Halt if mutex creation fails
  }

  // Create tasks
  xTaskCreate(
    receiveTask,
    "Receive Task",
    4096,  
    NULL,
    2,     
    &receiveTaskHandle
  );

  xTaskCreate(
    led1BlinkTask,
    "LED1 Blink Task", 
    4096, 
    NULL, 
    1, 
    &led1TaskHandle
  );

  xTaskCreate(
    led2PWMTask,
    "LED2 PWM Task", 
    4096, 
    NULL, 
    1, 
    &led2TaskHandle
  );
}

void loop() {

}

void receiveTask(void *pvParameters) {
  uint8_t localCommandPacket[32];

  for (;;) {
    // Check if serial data is available
    if (Serial.available()) {
      // Acquire mutex before modifying shared resource
      if (xSemaphoreTake(Key, portMAX_DELAY) == pdTRUE) {
        // Read serial data
        int bytesRead = Serial.read(commandPacket, 32);
        
        // Debug: Print received packet
        Serial.println("Received Packet:");
        for (int i = 0; i < bytesRead; i++) {
          Serial.print(commandPacket[i]);
          Serial.print(" ");
        }
        Serial.println();

        // Release mutex
        xSemaphoreGive(Key);
      }
    }

    // Small delay to prevent task from hogging CPU
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void led1BlinkTask(void *pvParameters) {
  uint32_t localDelay = 500;  // Default 500ms delay

  for (;;) {
    // Acquire mutex to safely read shared resource
    if (xSemaphoreTake(Key, portMAX_DELAY) == pdTRUE) {
      // Check for LED1 command (assuming first 4 bytes are "LED1")
      if (strncmp((char*)commandPacket, "LED1", 4) == 0) {
        // Extract delay value (assuming delay is stored as an integer after "LED1")
        localDelay = commandPacket[4];
      }

      // Release mutex
      xSemaphoreGive(Key);
    }

    // Blink LED
    digitalWrite(LED1_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(localDelay));
    digitalWrite(LED1_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(localDelay));
  }
}

void led2PWMTask(void *pvParameters) {
  uint8_t localPWM = 128;  // Default 50% brightness

  for (;;) {
    // Acquire mutex to safely read shared resource
    if (xSemaphoreTake(Key, portMAX_DELAY) == pdTRUE) {
      // Check for LED2 command (assuming first 4 bytes are "LED2")
      if (strncmp((char*)commandPacket, "LED2", 4) == 0) {
        // Extract PWM value (assuming PWM is stored as an integer after "LED2")
        localPWM = commandPacket[4];
      }

      // Release mutex
      xSemaphoreGive(Key);
    }

    // Set PWM
    analogWrite(LED2_PIN, localPWM);
    
    // Small delay to prevent task from spinning too fast
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
