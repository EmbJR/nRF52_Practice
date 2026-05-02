#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#define GPIO_PIN 13
//#define GPIO_PIN_17 17
#define GPIO_PIN_18 18
#define GPIO_PIN_19 19
#define GPIO_PIN_20 20

LOG_MODULE_REGISTER(Custome_Jaydeep, LOG_LEVEL_INF);

int main(void) {
    const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

    if (!device_is_ready(gpio_dev)) {
        return 0;
    }

    gpio_pin_configure(gpio_dev, GPIO_PIN, GPIO_OUTPUT);
    //gpio_pin_configure(gpio_dev, GPIO_PIN_17, GPIO_OUTPUT);
    gpio_pin_configure(gpio_dev, GPIO_PIN_18, GPIO_OUTPUT);
    gpio_pin_configure(gpio_dev, GPIO_PIN_19, GPIO_OUTPUT);
    gpio_pin_configure(gpio_dev, GPIO_PIN_20, GPIO_OUTPUT);


    while (1) {
        gpio_pin_toggle(gpio_dev, GPIO_PIN_20);
        LOG_INF("Hello, Zephyr!");
        k_msleep(1000);
    }
    return 0;
}