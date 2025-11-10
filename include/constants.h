#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

/**
 * Direction Constants
 */
const uint8_t DIRECTION_STOP = 0;      // Stop all movement
const uint8_t DIRECTION_FORWARD = 1;   // Move forward
const uint8_t DIRECTION_BACKWARD = 2;  // Move backward
const uint8_t DIRECTION_LEFT = 3;      // Turn left
const uint8_t DIRECTION_RIGHT = 4;     // Turn right

/**
 * Display configuration settings
 */
const uint8_t DISPLAY_BRIGHTNESS = 50;  // Display brightness level (0-255), set to 50 for power efficiency

/**
 * Touch area structure definition
 */
struct TouchArea {
  uint16_t x_min;      // Minimum X coordinate of touch area
  uint16_t x_max;      // Maximum X coordinate of touch area
  uint16_t y_min;      // Minimum Y coordinate of touch area
  uint16_t y_max;      // Maximum Y coordinate of touch area
  uint8_t direction;   // Direction command to send when this area is touched
  const char* label;   // Human-readable label for this area (used in debug output)
};

/**
 * Arrow coordinate namespace
 */
namespace Arrow {
  /**
   * Forward (upward) arrow coordinates
   */
  const uint16_t FORWARD_X1 = 160;
  const uint16_t FORWARD_Y1 = 5;
  const uint16_t FORWARD_X2 = 125;
  const uint16_t FORWARD_Y2 = 70;
  const uint16_t FORWARD_X3 = 195;
  const uint16_t FORWARD_Y3 = 70;
  
  /**
   * Backward (downward) arrow coordinates
   */
  const uint16_t BACKWARD_X1 = 160;
  const uint16_t BACKWARD_Y1 = 235;
  const uint16_t BACKWARD_X2 = 125;
  const uint16_t BACKWARD_Y2 = 170;
  const uint16_t BACKWARD_X3 = 195;
  const uint16_t BACKWARD_Y3 = 170;
  
  /**
   * Left arrow coordinates
   */
  const uint16_t LEFT_X1 = 10;
  const uint16_t LEFT_Y1 = 120;
  const uint16_t LEFT_X2 = 80;
  const uint16_t LEFT_Y2 = 85;
  const uint16_t LEFT_X3 = 80;
  const uint16_t LEFT_Y3 = 155;
  
  /**
   * Right arrow coordinates
   */
  const uint16_t RIGHT_X1 = 310;
  const uint16_t RIGHT_Y1 = 120;
  const uint16_t RIGHT_X2 = 240;
  const uint16_t RIGHT_Y2 = 85;
  const uint16_t RIGHT_X3 = 240;
  const uint16_t RIGHT_Y3 = 155;
}

/**
 * Touch area definitions array
 * 
 * Defines the rectangular regions on the screen that respond to touch input.
 * Each entry specifies:
 * - Boundaries of the touch-sensitive area
 * - Direction command to send when touched
 * - Label for identification in debug output
 */
const TouchArea TOUCH_AREAS[] = {
  // {x_min, x_max, y_min, y_max, direction, label}
  {100, 220, 0, 80, DIRECTION_FORWARD, "Forward"},
  {100, 220, 160, 240, DIRECTION_BACKWARD, "Backward"},
  {0, 100, 80, 160, DIRECTION_LEFT, "Left"},
  {220, 320, 80, 160, DIRECTION_RIGHT, "Right"}
};

/**
 * Number of defined touch areas
 */
const size_t TOUCH_AREAS_COUNT = sizeof(TOUCH_AREAS) / sizeof(TouchArea);

#endif // CONSTANTS_H
