
/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   500

/* The devicetree node identifier for the "led0" alias. */
#define LED2_NODE DT_ALIAS(led2)
#define LED1_NODE DT_ALIAS(led1)

#define THREAD_STACK_SIZE 1024

#define THREAD0_PRIORITY 7
#define THREAD1_PRIORITY 7

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);


void thread0(void)
{
	while (1) {
		/* STEP 3 - Call printk() to display a simple string "Hello, I am thread0" */
		printk("Hello, I am thread0\n");
		/* STEP 6 - Make the thread yield */
		// k_yield();
		/* STEP 10 - Put the thread to sleep */
		 //k_msleep(50);
		 //k_yield();
		k_busy_wait(1000000);
		/* Remember to comment out the line from STEP 6 */
	}
}

void thread1(void)
{
	while (1) {
		/* STEP 3 - Call printk() to display a simple string "Hello, I am thread1" */
		printk("Hello, I am thread1\n");
		/* STEP 8 - Make the thread yield */
		// k_yield();
		/* STEP 10 - Put the thread to sleep */
		//k_msleep(50);
		//k_yield();
		k_busy_wait(1000000);
		/* Remember to comment out the line from STEP 8 */
	}
}

K_THREAD_DEFINE(thread0_id, THREAD_STACK_SIZE, thread0, NULL, NULL, NULL, THREAD0_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread1_id, THREAD_STACK_SIZE, thread1, NULL, NULL, NULL, THREAD1_PRIORITY, 0, 0);

#if 0
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


	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}




	while (1) {
		//printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
#endif