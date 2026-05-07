#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <stdio.h>

/* Constants */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
#define RECEIVE_BUFF_SIZE 64
#define RECEIVE_TIMEOUT 1000 // Microseconds

static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static uint8_t rx_buf[RECEIVE_BUFF_SIZE];
static char msg_buf[RECEIVE_BUFF_SIZE];

/* State Variables */
static bool is_printing = false;

/* --- UART Callback Function --- */
static void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data)
{
    switch (evt->type) {
    case UART_RX_RDY:
        // Copy received data to a string buffer for processing
        if (evt->data.rx.len > 0) {
            memcpy(msg_buf, &evt->data.rx.buf[evt->data.rx.offset], evt->data.rx.len);
            msg_buf[evt->data.rx.len] = '\0'; // Null terminate

            // Logic for Start/Stop
            if (strncmp(msg_buf, "Start", 5) == 0) {
                is_printing = true;
            } else if (strncmp(msg_buf, "Stop", 4) == 0) {
                is_printing = false;
            }
        }
        break;
    case UART_RX_DISABLED:
        // Re-enable reception to keep the interrupt active
        uart_rx_enable(dev, rx_buf, sizeof(rx_buf), RECEIVE_TIMEOUT);
        break;
    default:
        break;
    }
}

/* --- Main Logic --- */
int main(void)
{
    int count = 1;
    char tx_buf[32];

    if (!device_is_ready(uart_dev)) {
        return 0;
    }

    // 1. Initialize Interrupt-based Receive
    uart_callback_set(uart_dev, uart_callback, NULL);
    uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), RECEIVE_TIMEOUT);

    printk("UART Controller Ready. Type 'Start' or 'Stop'\n");

    while (1) {
        if (is_printing) {
            // 2. Format and send the string
            snprintf(tx_buf, sizeof(tx_buf), "TestCnt%d\n", count);
            for (int i = 0; i < strlen(tx_buf); i++) {
                uart_poll_out(uart_dev, tx_buf[i]);
            }

            // 3. Counter Logic
            count++;
            if (count > 100) {
                count = 1;
            }

            // 4. One-second delay
            k_msleep(1000);
        } else {
            // If stopped, just wait for the interrupt to flip the flag
            k_msleep(100);
        }
    }
}