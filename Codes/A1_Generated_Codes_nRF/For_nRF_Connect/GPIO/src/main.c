/*
 * GPIO P0.13 Toggle Application for nRF52832
 * Using nRF Connect SDK (based on Zephyr RTOS)
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* Define GPIO P0.13 (nRF52832 has GPIO0) */
#define GPIO_PORT_NAME		DT_NODELABEL(gpio0)
#define GPIO_PIN_NUMBER		13
#define TOGGLE_INTERVAL_MS	1000	/* Toggle every 1 second */

/* Get the GPIO device pointer */
static const struct device *gpio_dev;

/* GPIO configuration structure */
struct gpio_dt_spec gpio_p013 = GPIO_DT_SPEC_GET(DT_NODELABEL(gpio_p013), gpios);

void main(void)
{
	int ret;

	printk("nRF52832 GPIO P0.13 Toggle Application\n");
	printk("========================================\n\n");

	/* Get the GPIO device */
	gpio_dev = DEVICE_DT_GET(GPIO_PORT_NAME);
	if (!device_is_ready(gpio_dev)) {
		printk("GPIO device is not ready\n");
		return;
	}

	/* Configure GPIO P0.13 as output with active high */
	ret = gpio_pin_configure(gpio_dev, GPIO_PIN_NUMBER, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Failed to configure GPIO P0.13: %d\n", ret);
		return;
	}

	printk("GPIO P0.13 configured as output\n");
	printk("Starting toggle loop...\n\n");

	/* Toggle GPIO P0.13 in an infinite loop */
	while (1) {
		/* Toggle the GPIO pin */
		ret = gpio_pin_toggle(gpio_dev, GPIO_PIN_NUMBER);
		if (ret < 0) {
			printk("Failed to toggle GPIO P0.13: %d\n", ret);
			return;
		}

		/* Get current state and print */
		int pin_state = gpio_pin_get(gpio_dev, GPIO_PIN_NUMBER);
		printk("GPIO P0.13 toggled - State: %s\n", 
		       pin_state ? "HIGH" : "LOW");

		/* Wait before toggling again */
		k_msleep(TOGGLE_INTERVAL_MS);
	}
}
