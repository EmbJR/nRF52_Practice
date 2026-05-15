#include <zephyr/kernel.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <bluetooth/gatt_dm.h>
#include <bluetooth/scan.h>

#include <zephyr/settings/settings.h>
#include <zephyr/drivers/uart.h>

#include "Custom_client.h"

#define NUS_WRITE_TIMEOUT K_MSEC(150)
#define UART_WAIT_FOR_BUF_DELAY K_MSEC(50)
#define UART_RX_TIMEOUT 50000 /* Wait for RX complete event time in microseconds. */

#define UART_BUFF_SIZE 20

struct uart_data_t
{
        void *fito_reserved;
        uint8_t data[UART_BUFF_SIZE];
        uint16_t len;
};

static int scan_start(void);

static K_FIFO_DEFINE(fifo_uart_tx_data);
static K_FIFO_DEFINE(fifo_uart_rx_data);

static struct k_work scan_work;
static struct k_work_delayable uart_work;

static struct bt_conn *default_conn;
static struct bt_custom_client cust_client = {0};

K_SEM_DEFINE(cust_write_sem, 0, 1);

#define UART_EN

#if defined UART_EN
static const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
#endif

//--------------------------- uart handling starts ---------------------------//
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
        #if defined UART_EN
        ARG_UNUSED(dev);
        ARG_UNUSED(user_data);

        static size_t aborted_len;
        struct uart_data_t *buf;
        static uint8_t *aborted_buf;
        static bool disable_req;

        switch (evt->type)
        {
        case UART_TX_DONE:
                printk("UART_TX_DONE\n");
                if ((evt->data.tx.len == 0) ||
                    (!evt->data.tx.buf))
                {
                        return;
                }

                if (aborted_buf)
                {
                        buf = CONTAINER_OF(aborted_buf, struct uart_data_t,
                                           data[0]);
                        aborted_buf = NULL;
                        aborted_len = 0;
                }
                else
                {
                        buf = CONTAINER_OF(evt->data.tx.buf,
                                           struct uart_data_t,
                                           data[0]);
                }

                k_free(buf);

                buf = k_fifo_get(&fifo_uart_tx_data, K_NO_WAIT);
                if (!buf)
                {
                        return;
                }

                if (uart_tx(uart, buf->data, buf->len, SYS_FOREVER_MS))
                {
                        printk("Failed to send data over UART\n");
                }

                break;

        case UART_RX_RDY:
                printk("UART_RX_RDY\n");
                buf = CONTAINER_OF(evt->data.rx.buf, struct uart_data_t, data[0]);
                buf->len += evt->data.rx.len;

                if (disable_req)
                {
                        return;
                }

                if ((evt->data.rx.buf[buf->len - 1] == '\n') ||
                    (evt->data.rx.buf[buf->len - 1] == '\r'))
                {
                        disable_req = true;
                        uart_rx_disable(uart);
                }

                break;

        case UART_RX_DISABLED:
                printk("UART_RX_DISABLED\n");
                disable_req = false;

                buf = k_malloc(sizeof(*buf));
                if (buf)
                {
                        buf->len = 0;
                }
                else
                {
                        printk("Not able to allocate UART receive buffer\n");
                        k_work_reschedule(&uart_work, UART_WAIT_FOR_BUF_DELAY);
                        return;
                }

                uart_rx_enable(uart, buf->data, sizeof(buf->data),
                               UART_RX_TIMEOUT);

                break;

        case UART_RX_BUF_REQUEST:
                printk("UART_RX_BUF_REQUEST\n");
                buf = k_malloc(sizeof(*buf));
                if (buf)
                {
                        buf->len = 0;
                        uart_rx_buf_rsp(uart, buf->data, sizeof(buf->data));
                }
                else
                {
                        printk("Not able to allocate UART receive buffer\n");
                }

                break;

        case UART_RX_BUF_RELEASED:
                printk("UART_RX_BUF_RELEASED\n");
                buf = CONTAINER_OF(evt->data.rx_buf.buf, struct uart_data_t,
                                   data[0]);

                if (buf->len > 0)
                {
                        k_fifo_put(&fifo_uart_rx_data, buf);
                }
                else
                {
                        k_free(buf);
                }

                break;

        case UART_TX_ABORTED:
                printk("UART_TX_ABORTED\n");
                if (!aborted_buf)
                {
                        aborted_buf = (uint8_t *)evt->data.tx.buf;
                }

                aborted_len += evt->data.tx.len;
                buf = CONTAINER_OF(aborted_buf, struct uart_data_t,
                                   data[0]);

                uart_tx(uart, &buf->data[aborted_len],
                        buf->len - aborted_len, SYS_FOREVER_MS);

                break;

        default:
                break;
        }
        #endif
}

static void uart_work_handler(struct k_work *item)
{
        #if defined UART_EN
        ARG_UNUSED(item);

        struct uart_data_t *buf;

        buf = k_malloc(sizeof(*buf));
        if (buf)
        {
                buf->len = 0;
        }
        else
        {
                printk("Not able to allocate UART receive buffer\n");
                k_work_reschedule(&uart_work, UART_WAIT_FOR_BUF_DELAY);
                return;
        }

        uart_rx_enable(uart, buf->data, sizeof(buf->data), UART_RX_TIMEOUT);
        #endif
}

static int uart_init(void)
{
        #if defined UART_EN
        int err;
        struct uart_data_t *rx;

        if (!device_is_ready(uart))
        {
                printk("UART device not ready\n");
                return -ENODEV;
        }

        rx = k_malloc(sizeof(struct uart_data_t));
        if (rx)
        {
                rx->len = 0;
        }
        else
        {
                return -ENOMEM;
        }

        k_work_init_delayable(&uart_work, uart_work_handler);

        err = uart_callback_set(uart, uart_cb, NULL);
        if (err)
        {
                return err;
        }

        return uart_rx_enable(uart, rx->data, sizeof(rx->data),
                              UART_RX_TIMEOUT);
        // TBD
        #endif
}
//---------------------- uart handling ends
//--------------- Discovery retaled starts ---------------------------//

static void discovery_complete(struct bt_gatt_dm *dm,
                               void *context)
{
        struct bt_custom_client *nus = context;
        printk("Service discovery completed\n");

        bt_gatt_dm_data_print(dm);

        bt_custom_handles_assign(dm, nus);
        bt_custom_subscribe_receive(nus);

        bt_gatt_dm_data_release(dm);
}

static void discovery_service_not_found(struct bt_conn *conn,
                                        void *context)
{
        ARG_UNUSED(conn);
        ARG_UNUSED(context);
        printk("Service not found\n");
}

static void discovery_error(struct bt_conn *conn,
                            int err,
                            void *context)
{
        ARG_UNUSED(conn);
        ARG_UNUSED(context);
        printk("Error while discovering GATT database: (%d)\n", err);
}

struct bt_gatt_dm_cb discovery_cb = {
    .completed = discovery_complete,
    .service_not_found = discovery_service_not_found,
    .error_found = discovery_error,
};

static void gatt_discover(struct bt_conn *conn)
{
#if 1
        int err;

        if (conn != default_conn)
        {
                return;
        }

        err = bt_gatt_dm_start(conn,
                               BT_UUID_NUS_SERVICE,
                               &discovery_cb,
                               &cust_client);
        if (err)
        {
                printk("could not start the discovery procedure, error code: %d\n",
                       err);
        }
#endif
}
//--------------- Discovery retaled ends ---------------------------//

//--------------- connection start -----------------------------//
static void exchange_func(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
        ARG_UNUSED(conn);
        ARG_UNUSED(params);

        if (!err)
        {
                printk("MTU exchange done\n");
        }
        else
        {
                printk("MTU exchange failed (err %" PRIu8 ")\n", err);
        }
}

static void connected(struct bt_conn *conn, uint8_t conn_err)
{
#if 1
        char addr[BT_ADDR_LE_STR_LEN];
        int err;

        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

        if (conn_err)
        {
                printk("Failed to connect to %s, 0x%02x %s\n", addr, conn_err,
                       bt_hci_err_to_str(conn_err));

                if (default_conn == conn)
                {
                        bt_conn_unref(default_conn);
                        default_conn = NULL;

                        (void)k_work_submit(&scan_work);
                }

                return;
        }

        printk("Connected: %s\n", addr);

        static struct bt_gatt_exchange_params exchange_params;

        exchange_params.func = exchange_func;
        err = bt_gatt_exchange_mtu(conn, &exchange_params);
        if (err)
        {
                printk("MTU exchange failed (err %d)\n", err);
        }

        gatt_discover(conn);

        err = bt_scan_stop();
        if (err)
        {
                printk("Stop LE scan failed (err %d)\n", err);
        }
#endif
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
#if 1
        char addr[BT_ADDR_LE_STR_LEN];

        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

        printk("Disconnected: %s, reason 0x%02x %s\n", addr, reason,
               bt_hci_err_to_str(reason));

        if (default_conn != conn)
        {
                return;
        }

        bt_conn_unref(default_conn);
        default_conn = NULL;

        (void)k_work_submit(&scan_work);
#endif
}

#if 0
static void security_changed(struct bt_conn *conn, bt_security_t level,
			     enum bt_security_err err)
{
        
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		printk("Security changed: %s level %u\n", addr, level);
	} else {
		printk("Security failed: %s level %u err %d %s\n", addr, level, err,
			bt_security_err_to_str(err));
	}

	gatt_discover(conn);
        
}
#endif

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
    //.security_changed = security_changed
};
//--------------- connection ends ------------------------------------//

static void scan_filter_match(struct bt_scan_device_info *device_info,
                              struct bt_scan_filter_match *filter_match,
                              bool connectable)
{
        ARG_UNUSED(filter_match);

        char addr[BT_ADDR_LE_STR_LEN];

        bt_addr_le_to_str(device_info->recv_info->addr, addr, sizeof(addr));

        printk("Filters matched. Address: %s connectable: %d\n",
               addr, connectable);
}

static void scan_connecting_error(struct bt_scan_device_info *device_info)
{
        ARG_UNUSED(device_info);
        printk("Connecting failed\n");
}

static void scan_connecting(struct bt_scan_device_info *device_info,
                            struct bt_conn *conn)
{
        ARG_UNUSED(device_info);
        default_conn = bt_conn_ref(conn);
}

BT_SCAN_CB_INIT(scan_cb, scan_filter_match, NULL,
                scan_connecting_error, scan_connecting);

static void scan_work_handler(struct k_work *item)
{
        ARG_UNUSED(item);

        (void)scan_start();
}

static void scan_init(void)
{
        struct bt_scan_init_param scan_init = {
            .connect_if_match = true,
        };

        bt_scan_init(&scan_init);
        bt_scan_cb_register(&scan_cb);

        k_work_init(&scan_work, scan_work_handler);
        printk("Scan module initialized\n");
}

static int scan_start(void)
{
        int err;
        uint8_t filter_mode = 0;

        err = bt_scan_stop();
        if (err)
        {
                printk("Failed to stop scanning (err %d)\n", err);
                return err;
        }

        bt_scan_filter_remove_all();

        err = bt_scan_filter_add(BT_SCAN_FILTER_TYPE_UUID, BT_UUID_NUS_SERVICE);
        if (err)
        {
                printk("UUID filter cannot be added (err %d\n", err);
                return err;
        }
        filter_mode |= BT_SCAN_UUID_FILTER;

        err = bt_scan_filter_enable(filter_mode, false);
        if (err)
        {
                printk("Filters cannot be turned on (err %d)\n", err);
                return err;
        }

        err = bt_scan_start(BT_SCAN_TYPE_SCAN_ACTIVE);
        if (err)
        {
                printk("Scanning failed to start (err %d)\n", err);
                return err;
        }

        printk("Scan started\n");
        return 0;
}

//-------------------------- Data transmition CBs ------------------------//
uint8_t ble_data_received(struct bt_custom_client *cust,
                                 const uint8_t *data, uint16_t len)
{

        ARG_UNUSED(cust);

        int err;
#if defined UART_EN
        for (uint16_t pos = 0; pos != len;)
        {
                struct uart_data_t *tx = k_malloc(sizeof(*tx));

                if (!tx)
                {
                        printk("Not able to allocate UART send data buffer\n");
                        return BT_GATT_ITER_CONTINUE;
                }

                /* Keep the last byte of TX buffer for potential LF char. */
                size_t tx_data_size = sizeof(tx->data) - 1;

                if ((len - pos) > tx_data_size)
                {
                        tx->len = tx_data_size;
                }
                else
                {
                        tx->len = (len - pos);
                }

                memcpy(tx->data, &data[pos], tx->len);

                pos += tx->len;

                /* Append the LF character when the CR character triggered
                 * transmission from the peer.
                 */
                if ((pos == len) && (data[len - 1] == '\r'))
                {
                        tx->data[tx->len] = '\n';
                        tx->len++;
                }

                 printk("Received data is %.*s\n",tx->len, tx->data);
                // err = uart_tx(uart, tx->data, tx->len, SYS_FOREVER_MS);
                // if (err)
                // {
                //         k_fifo_put(&fifo_uart_tx_data, tx);
                // }
        }
#endif
        return BT_GATT_ITER_CONTINUE;
}

void ble_data_sent(struct bt_custom_client *cust, uint8_t err,
                          const uint8_t *const data, uint16_t len)
{
#if 1
	ARG_UNUSED(cust);
	ARG_UNUSED(data);
	ARG_UNUSED(len);

	k_sem_give(&cust_write_sem);

	if (err) {
		printk("ATT error code: 0x%02X\n", err);
	}
#endif
}

static int custom_client_init(void)
{
        int err = 0;
#if 1
        struct bt_custom_client_cb init = {
            .datarecv_fun_t = ble_data_received,
            .datasent_fun_t = ble_data_sent,
        };

        cust_client.cb = &init;

        printk("NUS Client module initialized\n");
#endif
        return err;
}
//-------------------------- Data transmission CBs ends ------------------------//

int main(void)
{
        int err;

        testFunc();

#if 1
        err = bt_enable(NULL);
        if (err)
        {
                printk("Bluetooth init failed (err %d)\n", err);
                return 0;
        }
        printk("Bluetooth initialized\n");

        err = uart_init();
        if (err != 0)
        {
                printk("uart_init failed (err %d)\n", err);
                return 0;
        }

        err = custom_client_init();
        if (err != 0)
        {
                printk("custom_client_init failed (err %d)\n", err);
                return 0;
        }

        scan_init();
        err = scan_start();
        if (err)
        {
                return 0;
        }
#endif

        printk("Starting Bluetooth Central UART sample\n");

        struct uart_data_t nus_data = {
            .len = 0,
        };

        for (;;)
        {
                /* Wait indefinitely for data to be sent over Bluetooth */
                struct uart_data_t *buf = k_fifo_get(&fifo_uart_rx_data,
                                                     K_FOREVER);

                int plen = MIN(sizeof(nus_data.data) - nus_data.len, buf->len);
                int loc = 0;

                while (plen > 0)
                {
                        memcpy(&nus_data.data[nus_data.len], &buf->data[loc], plen);
                        nus_data.len += plen;
                        loc += plen;
                        if (nus_data.len >= sizeof(nus_data.data) ||
                            (nus_data.data[nus_data.len - 1] == '\n') ||
                            (nus_data.data[nus_data.len - 1] == '\r'))
                        {
                                err = bt_custom_client_send(&cust_client, nus_data.data, nus_data.len);
                                if (err)
                                {
                                        printk("Failed to send data over BLE connection(err %d)\n",
                                               err);
                                }

                                err = k_sem_take(&cust_write_sem, NUS_WRITE_TIMEOUT);
                                if (err)
                                {
                                        printk("NUS send timeout\n");
                                }

                                nus_data.len = 0;
                        }

                        plen = MIN(sizeof(nus_data.data), buf->len - loc);
                }

                k_free(buf);
        }

        return 0;
}

