#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "custom_client.h"

void testFunc(void)
{
	printk("This is a test function in Custom_client.c\n");
}

int bt_custom_handles_assign(struct bt_gatt_dm *dm,
							 struct bt_custom_client *nus_c)
{
#if 1
	const struct bt_gatt_dm_attr *gatt_service_attr =
		bt_gatt_dm_service_get(dm);
	const struct bt_gatt_service_val *gatt_service =
		bt_gatt_dm_attr_service_val(gatt_service_attr);
	const struct bt_gatt_dm_attr *gatt_chrc;
	const struct bt_gatt_dm_attr *gatt_desc;

	if (bt_uuid_cmp(gatt_service->uuid, BT_UUID_NUS_SERVICE))
	{
		return -ENOTSUP;
	}
	printk("Getting handles from NUS service.\n");

	/* NUS TX Characteristic */
	gatt_chrc = bt_gatt_dm_char_by_uuid(dm, BT_UUID_NUS_TX);
	if (!gatt_chrc)
	{
		printk("Missing NUS TX characteristic.\n");
		return -EINVAL;
	}
	/* NUS TX */
	gatt_desc = bt_gatt_dm_desc_by_uuid(dm, gatt_chrc, BT_UUID_NUS_TX);
	if (!gatt_desc)
	{
		printk("Missing NUS TX value descriptor in characteristic.\n");
		return -EINVAL;
	}
	printk("Found handle for NUS TX characteristic.\n");
	nus_c->tx_notif_params.value_handle = gatt_desc->handle;
	/* NUS TX CCC */
	gatt_desc = bt_gatt_dm_desc_by_uuid(dm, gatt_chrc, BT_UUID_GATT_CCC);
	if (!gatt_desc)
	{
		printk("Missing NUS TX CCC in characteristic.\n");
		return -EINVAL;
	}
	printk("Found handle for CCC of NUS TX characteristic.\n");
	nus_c->tx_notif_params.ccc_handle = gatt_desc->handle;

	/* NUS RX Characteristic */
	gatt_chrc = bt_gatt_dm_char_by_uuid(dm, BT_UUID_NUS_RX);
	if (!gatt_chrc)
	{
		printk("Missing NUS RX characteristic.\n");
		return -EINVAL;
	}
	/* NUS RX */
	gatt_desc = bt_gatt_dm_desc_by_uuid(dm, gatt_chrc, BT_UUID_NUS_RX);
	if (!gatt_desc)
	{
		printk("Missing NUS RX value descriptor in characteristic.\n");
		return -EINVAL;
	}
	printk("Found handle for NUS RX characteristic.\n");
	nus_c->rx_write_params.handle = gatt_desc->handle;

	/* Assign connection instance. */
	nus_c->conn = bt_gatt_dm_conn_get(dm);
#endif
	return 0;
}

//--------------------- receive handling start ----------------------//
static uint8_t on_received(struct bt_conn *conn,
						   struct bt_gatt_subscribe_params *params,
						   const void *data, uint16_t length)
{
	struct bt_custom_client *cust;

	ARG_UNUSED(conn);

	/* Retrieve NUS Client module context. */
	cust = CONTAINER_OF(params, struct bt_custom_client, tx_notif_params);

	if (!data)
	{
		printk("[UNSUBSCRIBED]\n");
		params->value_handle = 0;
		atomic_clear_bit(&cust->state, NUS_C_TX_NOTIF_ENABLED);
		return BT_GATT_ITER_STOP;
	}

	printk("[NOTIFICATION] data %p length %u\n", data, length);
	if (cust->cb->datarecv_fun_t)
	{
		return cust->cb->datarecv_fun_t(cust, data, length);
	}
	else
	{
		printk("Callback error..\n");
	}

	return BT_GATT_ITER_CONTINUE;
}

int bt_custom_subscribe_receive(struct bt_custom_client *nus_c)
{
	int err;
#if 1
	if (atomic_test_and_set_bit(&nus_c->state, NUS_C_TX_NOTIF_ENABLED))
	{
		return -EALREADY;
	}

	nus_c->tx_notif_params.notify = on_received;
	nus_c->tx_notif_params.value = BT_GATT_CCC_NOTIFY;
	atomic_set_bit(nus_c->tx_notif_params.flags,
				   BT_GATT_SUBSCRIBE_FLAG_VOLATILE);

	err = bt_gatt_subscribe(nus_c->conn, &nus_c->tx_notif_params);
	if (err)
	{
		printk("Subscribe failed (err %d)\n", err);
		atomic_clear_bit(&nus_c->state, NUS_C_TX_NOTIF_ENABLED);
	}
	else
	{
		printk("[SUBSCRIBED]\n");
	}
#endif
	return err;
}
//--------------------- receive handling ends ----------------------//

//---------------------- transmitt handling start -------------------------//
static void on_sent(struct bt_conn *conn, uint8_t err,
					struct bt_gatt_write_params *params)
{
	struct bt_custom_client *cust_c;
	const void *data;
	uint16_t length;

	/* Retrieve NUS Client module context. */
	cust_c = CONTAINER_OF(params, struct bt_custom_client, rx_write_params);

	/* Make a copy of volatile data that is required by the callback. */
	data = params->data;
	length = params->length;

	atomic_clear_bit(&cust_c->state, NUS_C_RX_WRITE_PENDING);
	if (cust_c->cb->datasent_fun_t)
	{
		cust_c->cb->datasent_fun_t(cust_c, err, data, length);
	}
	else
	{
		printk("Callback error..\n");
	}
}

int bt_custom_client_send(struct bt_custom_client *cust, const uint8_t *data,
						  uint16_t len)
{
	int err;

	if (!cust->conn)
	{
		return -ENOTCONN;
	}

	if (atomic_test_and_set_bit(&cust->state, NUS_C_RX_WRITE_PENDING))
	{
		return -EALREADY;
	}

	cust->rx_write_params.func = on_sent;
	cust->rx_write_params.offset = 0;
	cust->rx_write_params.data = data;
	cust->rx_write_params.length = len;

	err = bt_gatt_write(cust->conn, &cust->rx_write_params);
	if (err)
	{
		atomic_clear_bit(&cust->state, NUS_C_RX_WRITE_PENDING);
	}

	return err;
}
//---------------------- transmitt handling ends -------------------------//