/**
 * @file hardware_config.h
 * @brief Hardware Configuration Definitions for nRF52832
 *
 * This file contains easy-to-modify hardware pin and peripheral definitions.
 * Modify these values to change pin assignments without editing main.c
 */

#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

/* ============================================================================
 * LED Configuration
 * ========================================================================== */

/**
 * @defgroup LED_CONFIG LED Configuration
 * @brief LED hardware configuration
 * @{
 */

/** @brief LED GPIO Port (0 for GPIO Port 0) */
#define LED_PORT 0

/** @brief LED Pin Number (13 = P0.13) */
#define LED_PIN 13

/** @brief LED Active Level (1 = Active High, 0 = Active Low) */
#define LED_ACTIVE_HIGH 1

/** @brief Initial LED State (1 = ON, 0 = OFF) */
#define LED_INITIAL_STATE 0

/** @} */

/* ============================================================================
 * UART Configuration
 * ========================================================================== */

/**
 * @defgroup UART_CONFIG UART Configuration
 * @brief UART hardware configuration
 * @{
 */

/** @brief UART Instance (0 = UART0) */
#define UART_INSTANCE 0

/** @brief UART TX Pin (24 = P0.24) */
#define UART_TX_PIN 24

/** @brief UART RX Pin (25 = P0.25) */
#define UART_RX_PIN 25

/** @brief UART Baud Rate (bits per second) */
#define UART_BAUDRATE 115200

/** @brief UART Data Bits (typically 8) */
#define UART_DATA_BITS 8

/** @brief UART Stop Bits (typically 1) */
#define UART_STOP_BITS 1

/** @brief UART Parity (0 = None, 1 = Odd, 2 = Even) */
#define UART_PARITY 0

/** @} */

/* ============================================================================
 * Application Timing Configuration
 * ========================================================================== */

/**
 * @defgroup TIMING_CONFIG Timing Configuration
 * @brief Application timing parameters
 * @{
 */

/** @brief LED Blink Period in milliseconds */
#define LED_BLINK_PERIOD_MS 1000

/** @brief LED ON time in milliseconds (when period is 2000ms: 1000ms ON, 1000ms OFF) */
#define LED_ON_TIME_MS (LED_BLINK_PERIOD_MS / 2)

/** @brief LED OFF time in milliseconds */
#define LED_OFF_TIME_MS (LED_BLINK_PERIOD_MS / 2)

/** @} */

/* ============================================================================
 * Debug Configuration
 * ========================================================================== */

/**
 * @defgroup DEBUG_CONFIG Debug Configuration
 * @brief Debug and logging settings
 * @{
 */

/** @brief Enable verbose debug output (1 = enabled, 0 = disabled) */
#define DEBUG_VERBOSE 1

/** @brief Print LED toggle counter (1 = yes, 0 = no) */
#define PRINT_COUNTER 1

/** @brief Print GPIO state on each toggle (1 = yes, 0 = no) */
#define PRINT_GPIO_STATE 1

/** @} */

/* ============================================================================
 * Feature Configuration
 * ========================================================================== */

/**
 * @defgroup FEATURE_CONFIG Feature Configuration
 * @brief Feature enable/disable flags
 * @{
 */

/** @brief Enable LED functionality (1 = enabled, 0 = disabled) */
#define ENABLE_LED 1

/** @brief Enable UART logging (1 = enabled, 0 = disabled) */
#define ENABLE_UART 1

/** @brief Enable Button functionality (1 = enabled, 0 = disabled) */
#define ENABLE_BUTTON 0

/** @brief Enable PWM LED fading (1 = enabled, 0 = disabled) */
#define ENABLE_PWM_FADE 0

/** @} */

#endif /* HARDWARE_CONFIG_H */
