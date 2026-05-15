#ifndef CUST_CLIENT_H
#define CUST_CLIENT_H

#include <zephyr/types.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/conn.h>
#include <bluetooth/gatt_dm.h>

enum {
	NUS_C_INITIALIZED,
	NUS_C_TX_NOTIF_ENABLED,
	NUS_C_RX_WRITE_PENDING
};

/** @brief UUID of the NUS Service. **/
#define BT_UUID_NUS_VAL \
	BT_UUID_128_ENCODE(0x6e400001, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

/** @brief UUID of the TX Characteristic. **/
#define BT_UUID_NUS_TX_VAL \
	BT_UUID_128_ENCODE(0x6e400003, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

/** @brief UUID of the RX Characteristic. **/
#define BT_UUID_NUS_RX_VAL \
	BT_UUID_128_ENCODE(0x6e400002, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

#define BT_UUID_NUS_SERVICE   BT_UUID_DECLARE_128(BT_UUID_NUS_VAL)
#define BT_UUID_NUS_RX        BT_UUID_DECLARE_128(BT_UUID_NUS_RX_VAL)
#define BT_UUID_NUS_TX        BT_UUID_DECLARE_128(BT_UUID_NUS_TX_VAL)


/** @brief custom Client structure. */
struct bt_custom_client_cb;

struct bt_custom_client {

        /** Connection object. */
	struct bt_conn *conn;

         /** Application callbacks. */
	struct bt_custom_client_cb *cb;

            /** GATT subscribe parameters for NUS TX Characteristic. */
	struct bt_gatt_subscribe_params tx_notif_params;

        /** GATT write parameters for NUS RX Characteristic. */
	struct bt_gatt_write_params rx_write_params;
        /** Internal state. */
	atomic_t state;
    #if 0

        /** Handles on the connected peer device that are needed
         * to interact with the device.
         */
	struct bt_custom_client_handles handles;



   
    #endif
};

struct bt_custom_client_cb {
 void (*datasent_fun_t)(struct bt_custom_client *nus, uint8_t err,
					const uint8_t *const data, uint16_t len);

 uint8_t (*datarecv_fun_t)(struct bt_custom_client *nus,
					const uint8_t *const data, uint16_t len);
};


int bt_custom_handles_assign(struct bt_gatt_dm *dm,
			  struct bt_custom_client *nus_c);

int bt_custom_subscribe_receive(struct bt_custom_client *nus_c);

int bt_custom_client_send(struct bt_custom_client *cust, const uint8_t *data,
						  uint16_t len);

uint8_t ble_data_received(struct bt_custom_client *cust,
		const uint8_t *data, uint16_t len);

void ble_data_sent(struct bt_custom_client *cust, uint8_t err,
                          const uint8_t *const data, uint16_t len);

void testFunc(void);

#endif /* CUST_CLIENT_H */