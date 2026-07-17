/*
 * Copyright (c) 2024
 * nRF52832 LED and UART Demo Application
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

/* Define LED device */
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* UART device */
const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));

/* Check if LED device exists */
#if !DT_NODE_EXISTS(LED0_NODE)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

/* UART callback for data reception */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	if (evt->type == UART_RX_RDY) {
		printk("UART RX Ready\n");
	}
}

void main(void)
{
	int ret;

	printk("nRF52832 LED and UART Demo Started\n");

	/* Verify LED GPIO device is ready */
	if (!gpio_is_ready_dt(&led)) {
		printk("LED GPIO device is not ready\n");
		return;
	}

	/* Configure LED GPIO pin as output */
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		printk("Failed to configure LED GPIO pin: %d\n", ret);
		return;
	}

	printk("LED configured on GPIO P0.13\n");

	/* Verify UART device is ready */
	if (!device_is_ready(uart_dev)) {
		printk("UART device is not ready\n");
		return;
	}

	printk("UART initialized on P0.24 (TX) and P0.25 (RX)\n");
	printk("UART Baudrate: 115200\n\n");

	/* Main loop: blink LED */
	int counter = 0;
	while (1) {
		/* Toggle LED */
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			printk("Failed to toggle LED: %d\n", ret);
			return;
		}

		counter++;
		printk("[%d] LED toggled. LED is now %s\n", 
		       counter, 
		       gpio_pin_get_dt(&led) ? "ON" : "OFF");

		/* Wait 1 second */
		k_msleep(1000);
	}
}
