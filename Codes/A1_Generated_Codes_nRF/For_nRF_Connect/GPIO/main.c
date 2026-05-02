#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define GPIO_PIN 13

void main(void) {
    const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

    if (!device_is_ready(gpio_dev)) {
        return;
    }

    gpio_pin_configure(gpio_dev, GPIO_PIN, GPIO_OUTPUT);

    while (1) {
        gpio_pin_toggle(gpio_dev, GPIO_PIN);
        k_msleep(1000);
    }
}