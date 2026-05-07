#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/logging/log.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <bluetooth/services/nus.h>
#include <zephyr/drivers/uart.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
#define RECEIVE_BUFF_SIZE 64
#define RECEIVE_TIMEOUT 1000 // Microseconds

LOG_MODULE_REGISTER(ai_adv, LOG_LEVEL_INF);

static K_SEM_DEFINE(ble_init_ok, 0, 1);
static K_FIFO_DEFINE(fifo_uart_rx_data);

/* Device name */
#define DEVICE_NAME "AI_Advert_01"
#define DEVICE_NAME_LEN	(sizeof(DEVICE_NAME) - 1)

struct uart_data_t {
	void *fifo_reserved;
	uint8_t data[RECEIVE_BUFF_SIZE];
	uint16_t len;
};

static struct k_work adv_work;
static struct bt_conn *current_conn;
static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static uint8_t rx_buf[RECEIVE_BUFF_SIZE];
static char msg_buf[RECEIVE_BUFF_SIZE];

static uint32_t adv_counter = 1;
char adv_str[16];
/* Initial manufacturer data (compile-time constant for global init) */
static const char adv_str_init[] = "Adv_01";

/* Advertising parameters: non-connectable, 1 second interval (1600 * 0.625ms) */
static const struct bt_le_adv_param adv_params = {
    .options = BT_LE_ADV_OPT_CONN,
    .interval_min = 1600,
    .interval_max = 1600,
};

// const struct bt_data init_ad[] = {
//     BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
//     BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, (uint8_t)(sizeof(DEVICE_NAME) - 1)),
//     /* Use compile-time constant for global initializer */
//     BT_DATA(BT_DATA_MANUFACTURER_DATA, adv_str_init, (uint8_t)(sizeof(adv_str_init) - 1)),
// };

const struct bt_data init_ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_VAL),
};

static void update_adv_string(char *buf, size_t buf_len, uint32_t cnt)
{
    /* Minimum 2 digits for small numbers, supports up to 65000 */
    snprintf(buf, buf_len, "Adv_%02u", (unsigned int)cnt);
}

static void adv_work_handler(struct k_work *work)
{
        /* Prepare initial advertising data */
    update_adv_string(adv_str, sizeof(adv_str), adv_counter);

    int err =  bt_le_adv_start(&adv_params, init_ad, ARRAY_SIZE(init_ad), sd, ARRAY_SIZE(sd));
    if (err) {
        LOG_ERR("Advertising start failed (err %d)", err);
        return;
    }
    LOG_INF("Advertising successfully started");
    /* This function can be used for additional periodic tasks if needed */
}
static void advertising_start(void)
{
	k_work_submit(&adv_work);
}


static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data,
			  uint16_t len)
{
    char recv_str[33]; /* 32 chars + null terminator */
    size_t copy_len = MIN(len, sizeof(recv_str) - 1);

    memcpy(recv_str, data, copy_len);
    recv_str[copy_len] = '\0'; /* Ensure null termination */

    LOG_INF("Received data over NUS: %s", recv_str);
}

static struct bt_nus_cb nus_cb = {
	.received = bt_receive_cb,
};


static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	if (err) {
		LOG_ERR("Connection failed, err 0x%02x %s", err, bt_hci_err_to_str(err));
		return;
	}

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	LOG_INF("Connected %s", addr);

	current_conn = bt_conn_ref(conn);

	//dk_set_led_on(CON_STATUS_LED);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Disconnected: %s, reason 0x%02x %s", addr, reason, bt_hci_err_to_str(reason));

	if (current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
		//dk_set_led_off(CON_STATUS_LED);
	}
}

static void recycled_cb(void)
{
	LOG_INF("Connection object available from previous conn. Disconnect is complete!");
	advertising_start();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
    .le_param_updated = NULL,
    .recycled = recycled_cb,
};

#if 1
/* --- UART Callback Function --- */
static void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data)
{
    struct uart_data_t *buf;
    int len_b, offset_b;

    switch (evt->type) {
    case UART_RX_RDY:

    len_b = evt->data.rx.len;
    offset_b = evt->data.rx.offset;
    sprintf(msg_buf, "1_Received len %d, Data: %.*s", len_b, len_b, evt->data.rx.buf + offset_b);
    //uart_tx(uart_dev, msg_buf, strlen(msg_buf), SYS_FOREVER_MS);
        // Copy received data to a string buffer for processing
        if (evt->data.rx.len > 0) {
            buf = k_malloc(sizeof(struct uart_data_t));
            if(buf)
            {
                buf->len = evt->data.rx.len;
                memcpy(buf->data, &evt->data.rx.buf[evt->data.rx.offset], evt->data.rx.len);
                k_fifo_put(&fifo_uart_rx_data, buf);
            }
            
        }
        break;
    case UART_RX_DISABLED:
        // Re-enable reception to keep the interrupt active
        uart_rx_enable(dev, rx_buf, sizeof(rx_buf), RECEIVE_TIMEOUT);
        break;
    case UART_RX_BUF_RELEASED:
		
	break;
    default:
        break;
    }
}
#endif

void main(void)
{
    int err;
    
    LOG_INF("Starting AI advertiser sample");

        if (!device_is_ready(uart_dev)) {
            return;
        }

    // 1. Initialize Interrupt-based Receive
    uart_callback_set(uart_dev, uart_callback, NULL);
    uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), RECEIVE_TIMEOUT);

    err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d)", err);
        return;
    }

    err = bt_nus_init(&nus_cb);
    if (err) {
        LOG_ERR("Failed to initialize UART service (err: %d)", err);
        return;
    }

    //bt_set_name(DEVICE_NAME);

    k_work_init(&adv_work, adv_work_handler);
    advertising_start();


    k_sem_give(&ble_init_ok);
    LOG_INF("Advertising started: name=%s, payload=%s", DEVICE_NAME, adv_str);

    while (1) {
        k_sleep(K_MSEC(1000));

    }
}


void ble_write_thread(void)
{
    k_sem_take(&ble_init_ok, K_FOREVER);
    while(1)
    {
        //uart_tx(uart_dev, "Hello From Custome Firmware\n", sizeof("Hello From Custome Firmware\n"), 
        //                   SYS_FOREVER_MS);
        struct uart_data_t *rx_data = k_fifo_get(&fifo_uart_rx_data, K_NO_WAIT);
        if(rx_data)
        {
            //uart_tx(uart_dev, rx_data->data, rx_data->len, SYS_FOREVER_MS);
            if (bt_nus_send(NULL, rx_data->data, rx_data->len)) {
					LOG_WRN("Failed to send data over BLE connection");
				}
            k_free(rx_data);    
        }
        k_sleep(K_MSEC(100));
    }
}

#define BLESEND_STACK_SIZE 1024
#define BLE_SEND_PRIORITY 5

K_THREAD_DEFINE(Ble_Send_Thread, BLESEND_STACK_SIZE, ble_write_thread, 
                NULL, NULL, NULL, 
                BLE_SEND_PRIORITY, 0, 0);

