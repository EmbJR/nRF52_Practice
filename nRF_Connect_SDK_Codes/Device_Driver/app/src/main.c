/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if 1
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(PWM_Test, LOG_LEVEL_INF);
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   500

/* The devicetree node identifier for the "led0" alias. */
// #define LED2_NODE DT_ALIAS(led2)
// #define LED1_NODE DT_ALIAS(led1)
#define SW1_NODE DT_ALIAS(sw0)
#define SW2_NODE DT_ALIAS(sw1)

#define PWM_PERIOD_NS 20000000
#define PWM_PULSE_WIDTH 100000

#define PWM_LED0    DT_ALIAS(pwm_led0)

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(PWM_LED0);

 /* STEP 9.1 - Get the device structure from the node label */
 const struct device * dev = DEVICE_DT_GET(DT_NODELABEL(custom));

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
// static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

static const struct gpio_dt_spec sw1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
static const struct gpio_dt_spec sw2 = GPIO_DT_SPEC_GET(SW2_NODE, gpios);

static struct gpio_callback button_cb_data;



int main(void)
{
	volatile int ret;
	bool led_state = true;
	struct sensor_value temp_val;


	if (!gpio_is_ready_dt(&sw1)) {
		return 0;
	}

	if (!pwm_is_ready_dt(&pwm_led0)) {
		LOG_ERR("Error: PWM device %s is not ready\n", pwm_led0.dev->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&sw1, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	int err = pwm_set_dt(&pwm_led0, PWM_PERIOD_NS, PWM_PULSE_WIDTH);
	if (err) {
		LOG_ERR("Error in pwm_set_dt(), err: %d", err);
		return 0;
	}

	err = device_is_ready(dev);
     if (!err) {
         LOG_INF("Error: SPI device is not ready, err: %d", err);
         return 0;
     }

	err = sensor_sample_fetch(dev);
	if (err < 0) {
		LOG_ERR("Could not fetch sample (%d)", err);
		return 0;
	}

	if (sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp_val)) {
		LOG_ERR("Could not get sample");
		return 0;
	}



	while (1) {

		ret = gpio_pin_get_dt(&sw1);
		if (ret == 1) 
		{
			// ret = gpio_pin_set_dt(&led, 1);
			// if (ret < 0) {
			// 	return 0;
			// }
		}
		else
		{
			// ret = gpio_pin_set_dt(&led, 0);
			// if (ret < 0) {
			// 	return 0;
			// }
		}

		// ret = gpio_pin_toggle_dt(&led1);
		// if (ret < 0) {
		// 	return 0;
		// }

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
#endif

#if 0
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#define SLEEP_TIME_MS   500
int main(void)
{
	while (1) {

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
#endif
