/**
 * Remote Controller for a Crawler Robot Equipped with a Dot-Matrix Display  
 * 
 * This application controls a crawler robot using ESP-NOW wireless communication.
 * The M5Core2 device serves as a remote controller with a touch-based directional
 * interface displayed on the screen. Touch inputs are converted to direction commands
 * and transmitted to the crawler via ESP-NOW protocol.
 * 
 * Features:
 * - Touch-based directional control (Forward, Backward, Left, Right)
 * 
 * Hardware Requirements:
 * - M5Stack Core2
 */

#include <M5Unified.h>
#include <esp_now.h>
#include <WiFi.h>
#include "constants.h"

/**
 * MAC address of the receiver device (crawler robot)
 */
uint8_t broadcastAddress[] = {0x98, 0xA3, 0x16, 0x61, 0x19, 0xA8};

/**
 * ESP-NOW Message Structure
 */
typedef struct MessageStruct {
  uint8_t direction;
} MessageStruct;

/**
 * ESP-NOW peer information structure
 */
esp_now_peer_info_t peerInfo;

/**
 * Callback function invoked when ESP-NOW data transmission completes
 * 
 * @param mac_addr MAC address of the receiver device (6 bytes)
 * @param status Transmission status (ESP_NOW_SEND_SUCCESS or ESP_NOW_SEND_FAIL)
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("ESP-NOW delivery success");
  } else {
    Serial.println("ESP-NOW delivery failed");
  }
}

/**
 * Sends direction command to the crawler via ESP-NOW
 * 
 * @param direction Direction command value (use DIRECTION_* constants)
 *                  0 = Stop, 1 = Forward, 2 = Backward, 3 = Left, 4 = Right
 */
void sendData(uint8_t direction) {
  // Prepare message structure with direction command
  MessageStruct messageData = {
      .direction = direction,
  };

  // Attempt to send data via ESP-NOW to the broadcast address
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&messageData, sizeof(messageData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Sent with failed");
  }
}

/**
 * Renders the touch control interface on the display
 */
void drawUI() {
  // Clear display with black background
  M5.Display.clear();
  
  // Draw forward arrow
  M5.Display.fillTriangle(
    Arrow::FORWARD_X1, Arrow::FORWARD_Y1,
    Arrow::FORWARD_X2, Arrow::FORWARD_Y2,
    Arrow::FORWARD_X3, Arrow::FORWARD_Y3,
    TFT_WHITE
  );
  
  // Draw backward arrow
  M5.Display.fillTriangle(
    Arrow::BACKWARD_X1, Arrow::BACKWARD_Y1,
    Arrow::BACKWARD_X2, Arrow::BACKWARD_Y2,
    Arrow::BACKWARD_X3, Arrow::BACKWARD_Y3,
    TFT_WHITE
  );
  
  // Draw left arrow
  M5.Display.fillTriangle(
    Arrow::LEFT_X1, Arrow::LEFT_Y1,
    Arrow::LEFT_X2, Arrow::LEFT_Y2,
    Arrow::LEFT_X3, Arrow::LEFT_Y3,
    TFT_WHITE
  );
  
  // Draw right arrow
  M5.Display.fillTriangle(
    Arrow::RIGHT_X1, Arrow::RIGHT_Y1,
    Arrow::RIGHT_X2, Arrow::RIGHT_Y2,
    Arrow::RIGHT_X3, Arrow::RIGHT_Y3,
    TFT_WHITE
  );
}

/**
 * Displays an error message on the screen
 * 
 * @param message Error message string to display
 */
void displayError(const char* message) {
  M5.Display.clear();
  M5.Display.setTextColor(TFT_RED);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(10, 100);
  M5.Display.println("Error:");
  M5.Display.setCursor(10, 130);
  M5.Display.println(message);
}

/**
 * Initialize ESP-NOW Communication
 * 
 * @return true if initialization successful, false if any step fails
 */
bool initESPNow() {
  // Set WiFi to station mode (required for ESP-NOW)
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW protocol stack
  if (esp_now_init() != ESP_OK) {
    M5.Log.println("Error initializing ESP-NOW");
    displayError("ESP-NOW init failed");
    return false;
  }

  // Register callback function to monitor transmission status
  esp_now_register_send_cb(OnDataSent);

  // Configure peer information with receiver's MAC address
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);  // Copy MAC address
  peerInfo.channel = 0;                              // Use default WiFi channel
  peerInfo.encrypt = false;                          // Disable encryption for lower latency

  // Add configured peer to ESP-NOW peer list
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    M5.Log.println("Failed to add peer");
    displayError("Failed to add peer");
    return false;
  }

  return true;
}

/**
 * Determines which directional area was touched
 * 
 * @param x X-coordinate of touch point (0-319 for M5Core2)
 * @param y Y-coordinate of touch point (0-239 for M5Core2)
 * @return Index of the touched area (0-3), or -1 if no area was touched
 *         0=Forward, 1=Backward, 2=Left, 3=Right
 */
int checkTouchArea(int16_t x, int16_t y) {
  // Check each defined touch area
  for (size_t i = 0; i < TOUCH_AREAS_COUNT; i++) {
    const TouchArea& area = TOUCH_AREAS[i];
    // Check if coordinates fall within this area's boundaries
    if (x >= area.x_min && x < area.x_max && 
        y >= area.y_min && y < area.y_max) {
      return i;  // Return the index of the matched area
    }
  }
  return -1;  // No matching area found
}

/**
 * Processes touch screen events and sends appropriate commands
 * 
 * @param touch Touch event details including state, coordinates, and gesture info
 */
void handleTouch(const m5::touch_detail_t& touch) {
  switch (touch.state) {
    case m5::touch_state_t::touch_begin: {
      // User has just touched the screen - determine which area and send command
      int areaIndex = checkTouchArea(touch.x, touch.y);
      if (areaIndex >= 0) {
        const TouchArea& area = TOUCH_AREAS[areaIndex];
        Serial.printf("%s arrow was pressed.\n", area.label);
        sendData(area.direction);  // Send direction command to crawler
      }
      break;
    }
    
    // Handle all touch release events - send stop command
    case m5::touch_state_t::touch_end:
    case m5::touch_state_t::hold_end:
    case m5::touch_state_t::drag_end:
      Serial.println("Touch ended.");
      sendData(DIRECTION_STOP);  // Stop the crawler when touch is released
      break;
      
    default:
      // Ignore other touch states (touch_move, etc.)
      break;
  }
}

/**
 * Setup Function
 */
void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize M5Stack hardware with default configuration
  auto cfg = M5.config();
  M5.begin(cfg);

  // Reduce display brightness to conserve battery power
  M5.Display.setBrightness(DISPLAY_BRIGHTNESS);

  // Render the directional control interface
  drawUI();

  // Initialize ESP-NOW wireless communication
  if (!initESPNow()) {
    // Critical failure: stop execution to prevent unsafe operation
    while (true) {
      delay(1000);
    }
  }

  Serial.println("Setup complete - Remote Controller ready");
}

/**
 * Main Loop Function
 */
void loop() {
  // Update M5Stack internal state (must be called every loop iteration)
  M5.update();
  
  // Get detailed information about current touch state
  auto touch = M5.Touch.getDetail();
  
  // Process touch events and send appropriate commands
  handleTouch(touch);
}
