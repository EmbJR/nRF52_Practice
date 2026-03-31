#include <zephyr/types.h>
#include <zephyr/logging/log.h>

/* STEP 2.1 - Define the driver compatible from the custom binding */
#define DT_DRV_COMPAT zephyr_custom_bme280

LOG_MODULE_REGISTER(custom_bme280, CONFIG_SENSOR_LOG_LEVEL);

#if DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 0
#warning "Custom BME280 driver enabled without any devices"
#endif

struct custom_driver_data {
    /* Compensation parameters */
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
};

/* STEP 3.2 - Define structure to store sensor configuration */
struct custom_driver_config {
    struct spi_dt_spec spi;
};

static int custom_bme280_sample_fetch(const struct device *dev)
{
    const struct custom_bme280_config *bus = dev->config;
    // LOG_ERR("Custome driver call read \n
    //         reg %d data %d size %d", reg, data[0], size);
    LOG_ERR("Custome driver call read \n");

    return 0;
}

static int custom_bme280_channel_get(const struct device *dev)
{
    // LOG_ERR("Custome driver call write \n
    //         reg %d value %d", reg, value);
    LOG_ERR("Custome driver call read \n");

    return 0;
}

static int custom_bme280_init(const struct device *dev)
{
    LOG_ERR("Custome driver call init \n");

    return 0;
}

/* STEP 3.3 - Define the sensor driver API */
static const struct sensor_driver_api custom_bme280_api = {
    .sample_fetch = &custom_bme280_sample_fetch,
    .channel_get = &custom_bme280_channel_get,
};

/* STEP 5.1 - Define macro with device drivers structures */
#define CUSTOM_DRIVER_DEFINE(inst)                                              \
    static struct custom_driver_data custom_driver_data_##inst;                 \
    static const struct custom_driver_config custom_driver_config_##inst = {    \
        .spi = SPI_DT_SPEC_INST_GET(inst, SPIOP, 0),                            \
    };                                                                          \
    /* STEP 5.2 - Define a macro for the device driver instance */              \
    DEVICE_DT_INST_DEFINE(inst,                                                 \
                custom_bme280_init,                                             \
                NULL,                                                           \
                &custom_driver_data_##inst,                                     \
                &custom_driver_config_##inst,                                   \
                POST_KERNEL,                                                    \
                CONFIG_SENSOR_INIT_PRIORITY,                                    \
                &custom_bme280_api);

/* STEP 5.3 - Create the struct device for every status "okay" node in the devicetree */
DT_INST_FOREACH_STATUS_OKAY(CUSTOM_BME280_DEFINE)