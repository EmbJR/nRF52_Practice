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
#include <zephyr/random/random.h>


/* STEP 7 - Change the sleep time from 1000 ms to 100 ms */
#define SLEEP_TIME_MS 100

/* STEP 3.1 - Get the node identifier for button 1 through its alias sw0 */
#define SW0_NODE DT_ALIAS(sw0)

#define  PRODUCER_STACK_SIZE 1024
#define  PRODUCER_PRIOR		 12

#define  CONSUMER_STACK_SIZE 1024
#define  CONSUMER_PRIOR		 13

#define MAX_DATA_SIZE	14
#define MIN_DATA_ITEMS	4
#define MAX_DATA_ITEMS	14

struct data_item_t {
	void *fifo_reserved;
	uint8_t  data[MAX_DATA_SIZE];
	uint16_t len;
}; 

LOG_MODULE_REGISTER(Fifo_Test, LOG_LEVEL_DBG);

/* STEP 3.2 - Get the device pointer, pin number, and pin's configuration flags through gpio_dt_spec
 */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

/* LED0_NODE is the devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

K_FIFO_DEFINE(my_fifo);

#if 1

static void timer0_handler(struct k_timer *timer_id)
{
	gpio_pin_toggle_dt(&led);
}

void producer_fun(void)
{
	int ret;
	int bytes_written;
	uint32_t dataitem_count = 0;

	while (1) {
		
	/* Generate a random number between MIN_DATA_ITEMS & MAX_DATA_ITEMS to represent the number
	of data items to send every time the producer thread is scheduled */
	uint32_t data_number =
		MIN_DATA_ITEMS + sys_rand32_get() % (MAX_DATA_ITEMS - MIN_DATA_ITEMS + 1);
		for(int i = 0; i < data_number; i++) 
		{
			struct data_item_t *buff = k_malloc(sizeof(struct data_item_t));
			if(buff == NULL)
			{
				printf("Error creating buff\n");
				return;
			}
			bytes_written = snprintf(buff->data, MAX_DATA_SIZE, "Data seg %u:\t%u", dataitem_count, sys_rand32_get());
			buff->len = bytes_written;
			dataitem_count++;
			k_fifo_put(&my_fifo, buff);
		}
		k_sleep(K_SECONDS(1));
	}
}

void consumer_fun(void)
{
	int ret;
	struct data_item_t *Datarcv;
	while(1) {
		Datarcv = k_fifo_get(&my_fifo, K_FOREVER);
		if(Datarcv == NULL)
		{
			printf("Error creating buff\n");
			return;
		}
		//printf("Data received: %s len %u\n", Datarcv->data, Datarcv->len);
		LOG_INF("Consumer: %s\tSize: %u",Datarcv->data,Datarcv->len);
		k_free(Datarcv);
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

	return 0;
}
#endif