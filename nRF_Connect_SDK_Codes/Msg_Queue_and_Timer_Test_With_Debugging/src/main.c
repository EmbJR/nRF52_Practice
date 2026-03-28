/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>


/* STEP 7 - Change the sleep time from 1000 ms to 100 ms */
#define SLEEP_TIME_MS 100

/* STEP 3.1 - Get the node identifier for button 1 through its alias sw0 */
#define SW0_NODE DT_ALIAS(sw0)

#define  PRODUCER_STACK_SIZE 1024
#define  PRODUCER_PRIOR		 12

#define  CONSUMER_STACK_SIZE 1024
#define  CONSUMER_PRIOR		 13

/* STEP 3.2 - Get the device pointer, pin number, and pin's configuration flags through gpio_dt_spec
 */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

/* LED0_NODE is the devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#if 1
typedef struct {
	uint32_t x_reading;
	uint32_t y_reading;
	uint32_t z_reading;
}sensorreading;

K_MSGQ_DEFINE(device_msg_queue, sizeof(sensorreading), 16, 4);

static void timer0_handler(struct k_timer *timer_id)
{
	gpio_pin_toggle_dt(&led);
}

void producer_fun(void)
{
	int ret;
	sensorreading reading;
	reading.x_reading = 0;
	reading.y_reading = 0;
	reading.z_reading = 0;
	while (1) {
		
		reading.x_reading++;
		reading.y_reading++;
		reading.z_reading++;
		ret = k_msgq_put(&device_msg_queue, &reading, K_FOREVER);
		if(ret)
		{
			//LOG_ERR("Return value = %d", ret);
		}
		k_sleep(K_SECONDS(1));
	}
}

void consumer_fun(void)
{
	int ret;
	while(1) {
		sensorreading reading;
		ret = k_msgq_get(&device_msg_queue, &reading, K_FOREVER);
		if(ret)
		{
			//LOG_ERR("Return value = %d", ret);
		}
		printf("X = %d, Y = %d, Z = %d\n", reading.x_reading, reading.y_reading, reading.z_reading);
	}	
}

K_TIMER_DEFINE(timer0, timer0_handler, NULL);



K_THREAD_DEFINE(producer, PRODUCER_STACK_SIZE, producer_fun, NULL, NULL, NULL, PRODUCER_PRIOR, 0, 0);
K_THREAD_DEFINE(consumer, CONSUMER_STACK_SIZE, consumer_fun, NULL, NULL, NULL, CONSUMER_PRIOR, 0, 0);
#endif

#if 1


int main(void)
{
	int ret;

	if (!device_is_ready(led.port)) {
		return -1;
	}
	/* STEP 4 - Verify that the device is ready for use */
	if (!device_is_ready(button.port)) {
		return -1;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return -1;
	}

	/* STEP 5 - Configure the pin connected to the button to be an input pin and set its
	 * hardware specifications */
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		return -1;
	}

	k_timer_start(&timer0, K_MSEC(500), K_MSEC(500));

	while (1) {
		/* STEP 6.1 - Read the status of the button and store it */
		bool val = gpio_pin_get_dt(&button);


		k_msleep(SLEEP_TIME_MS); // Put the main thread to sleep for 100ms for power
					 // optimization
	}
}
	#endif