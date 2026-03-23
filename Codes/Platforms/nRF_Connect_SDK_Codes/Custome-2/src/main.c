#if 1
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#ifdef CONFIG_MYFUNCTION
#include "myfunction.h"
#endif
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/i2c.h>

#define I2C0_NODE DT_NODELABEL(mysensor)

static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);

int exercise_num = 2;
uint8_t data[] = {0x00, 0x01, 0x02, 0x03,
                  0x04, 0x05, 0x06, 0x07,
                  'H', 'e', 'l', 'l','o'};

static uint8_t rx_buf[10] = {0}; //A buffer to store incoming UART data 	
static uint8_t tx_buf[] =  {"nRF Connect SDK Fundamentals Course \n\r"};			  
		

LOG_MODULE_REGISTER(Less4_Exer2,LOG_LEVEL_DBG);

const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

const struct uart_config uart_cfg = {
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	switch (evt->type) {
	
	case UART_TX_DONE:
		// do something
		printk("Tx Done\n");
		break;

	case UART_TX_ABORTED:
		// do something
		break;
		
	case UART_RX_RDY:
		// do something
		break;

	case UART_RX_BUF_REQUEST:
		// do something
		break;

	case UART_RX_BUF_RELEASED:
		// do something
		break;
		
	case UART_RX_DISABLED:
		// do something
		break;

	case UART_RX_STOPPED:
		// do something
		break;
		
	default:
		break;
	}
}

int main(void)
{
	int ret;
	// Printf-like messages
	LOG_INF("nRF Connect SDK Fundamentals");
	LOG_INF("Exercise %d",exercise_num);    
	LOG_DBG("A log message in debug level");
	LOG_WRN("A log message in warning level!");
	LOG_ERR("A log message in Error level!");
	// Hexdump some data
	LOG_HEXDUMP_INF(data, sizeof(data),"Sample Data!");

	if (!device_is_ready(uart)) {
    return 0;
	}

	if (!device_is_ready(dev_i2c.bus)) {
		printk("I2C bus %s is not ready!\n\r",dev_i2c.bus->name);
		return 0;
	}

		int err = uart_configure(uart, &uart_cfg);
	if (err == -ENOSYS) {
		return -ENOSYS;
	}

	err = uart_callback_set(uart, uart_cb, NULL);
		if (err) {
			return err;
		}

	uart_rx_enable(uart, rx_buf, sizeof(rx_buf), 100);	

	uint8_t sensor_regs[2] ={0x00,0x00};
	uint8_t temp_reading[2]= {0,0};
	// ret =  i2c_write_dt(&dev_i2c,&sensor_regs[0], 1);
	// if(ret != 0){
	// 	printk("Failed to write/read I2C device address %x at Reg. %x \n\r", dev_i2c.addr,sensor_regs[0]);
	// }
	// ret =  i2c_read_dt(&dev_i2c,&temp_reading[0], 2);
	// if(ret != 0){
	// 	printk("Failed to write/read I2C device address %x at Reg. %x \n\r", dev_i2c.addr,sensor_regs[0]);
	// }

	ret = i2c_write_read_dt(&dev_i2c,&sensor_regs[0],1,&temp_reading[0],3);
	if(ret != 0){
		printk("Failed to write/read I2C device address %x at Reg. %x \n\r", dev_i2c.addr,sensor_regs[0]);
	}

	while (1) {
		err = uart_tx(uart, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
		if (err) {
			return err;
		}
#ifdef CONFIG_MYFUNCTION
		int a = 3, b = 4;
		printk("The sum of %d and %d is %d\n", a, b, sum(a, b));
#else
		printk("MYFUNCTION not enabled\n");
		return 0;
#endif
		k_msleep(1000);
	}
	return 0;
}
#endif

#if 1

#endif