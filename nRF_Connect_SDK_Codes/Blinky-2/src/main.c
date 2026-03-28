/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   500

/* The devicetree node identifier for the "led0" alias. */
#define LED2_NODE DT_ALIAS(led2)
#define LED1_NODE DT_ALIAS(led1)
#define SW1_NODE DT_ALIAS(sw0)
#define SW2_NODE DT_ALIAS(sw1)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

static const struct gpio_dt_spec sw1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
static const struct gpio_dt_spec sw2 = GPIO_DT_SPEC_GET(SW2_NODE, gpios);

static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_pin_toggle_dt(&led1);
}


int main(void)
{
	volatile int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	if (!gpio_is_ready_dt(&led1)) {
		return 0;
	}

	if (!gpio_is_ready_dt(&sw1)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&sw1, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&sw2, GPIO_INT_EDGE_TO_ACTIVE);

	gpio_init_callback(&button_cb_data, button_pressed, BIT(sw2.pin)); 	

	gpio_add_callback(sw2.port, &button_cb_data);

	while (1) {

		ret = gpio_pin_get_dt(&sw1);
		if (ret == 1) 
		{
			ret = gpio_pin_set_dt(&led, 1);
			if (ret < 0) {
				return 0;
			}
		}
		else
		{
			ret = gpio_pin_set_dt(&led, 0);
			if (ret < 0) {
				return 0;
			}
		}

		

		// ret = gpio_pin_toggle_dt(&led1);
		// if (ret < 0) {
		// 	return 0;
		// }

		led_state = !led_state;
		//printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
