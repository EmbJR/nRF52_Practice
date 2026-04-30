/*
 * Advanced GPIO P0.13 Toggle Application for nRF52832
 * Features: GPIO toggle with button control
 * 
 * NOTE: This is an alternative version with button input.
 * Replace src/main.c with this file and update the device tree overlay
 * to add button pin configuration.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define GPIO_PORT_NAME		DT_NODELABEL(gpio0)
#define GPIO_PIN_NUMBER		13		/* GPIO P0.13 for toggle */
#define BUTTON_PIN_NUMBER	11		/* GPIO P0.11 for button input (example) */
#define TOGGLE_INTERVAL_MS	500		/* Default toggle interval */

static const struct device *gpio_dev;
static uint32_t toggle_interval = TOGGLE_INTERVAL_MS;
static volatile bool button_pressed = false;

/* Button interrupt handler */
void button_pressed_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	button_pressed = true;
	printk("Button pressed!\n");
	
	/* Change toggle interval on button press */
	toggle_interval = (toggle_interval == 500) ? 1000 : 500;
	printk("Toggle interval changed to: %u ms\n", toggle_interval);
}

static struct gpio_callback button_cb_data;

void main(void)
{
	int ret;

	printk("\n=== nRF52832 Advanced GPIO P0.13 Toggle ===\n");
	printk("Features: GPIO toggle with button control\n\n");

	/* Get the GPIO device */
	gpio_dev = DEVICE_DT_GET(GPIO_PORT_NAME);
	if (!device_is_ready(gpio_dev)) {
		printk("GPIO device is not ready\n");
		return;
	}

	/* Configure GPIO P0.13 as output */
	ret = gpio_pin_configure(gpio_dev, GPIO_PIN_NUMBER, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Failed to configure GPIO P0.13 as output: %d\n", ret);
		return;
	}
	printk("GPIO P0.13 configured as output\n");

	/* Configure button pin (P0.11) as input with pull-up */
	ret = gpio_pin_configure(gpio_dev, BUTTON_PIN_NUMBER, 
	                         GPIO_INPUT | GPIO_PULL_UP);
	if (ret < 0) {
		printk("Failed to configure button pin: %d\n", ret);
		return;
	}
	printk("GPIO P0.11 (button) configured as input with pull-up\n");

	/* Configure interrupt for button */
	ret = gpio_pin_interrupt_configure(gpio_dev, BUTTON_PIN_NUMBER,
	                                   GPIO_INT_EDGE_FALLING);
	if (ret < 0) {
		printk("Failed to configure button interrupt: %d\n", ret);
		return;
	}

	/* Register button callback */
	gpio_init_callback(&button_cb_data, button_pressed_handler,
	                   BIT(BUTTON_PIN_NUMBER));
	ret = gpio_add_callback(gpio_dev, &button_cb_data);
	if (ret < 0) {
		printk("Failed to add callback: %d\n", ret);
		return;
	}

	printk("\nInitialization complete!\n");
	printk("- GPIO P0.13 will toggle every %u ms\n", toggle_interval);
	printk("- Press button on P0.11 to change toggle interval\n\n");

	/* Main toggle loop */
	while (1) {
		/* Toggle GPIO P0.13 */
		ret = gpio_pin_toggle(gpio_dev, GPIO_PIN_NUMBER);
		if (ret < 0) {
			printk("Failed to toggle GPIO P0.13: %d\n", ret);
			return;
		}

		/* Get and print current state */
		int pin_state = gpio_pin_get(gpio_dev, GPIO_PIN_NUMBER);
		printk("[%u ms] GPIO P0.13 state: %s\n", k_uptime_get_32(), 
		       pin_state ? "HIGH" : "LOW");

		/* Wait before next toggle */
		k_msleep(toggle_interval);
	}
}
