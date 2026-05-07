# AI Advertiser (nRF Connect SDK / Zephyr)

This sample app advertises non-connectable BLE packets on an nRF52832 board.

- Device name: AI_Advert_01
- Advertising interval: 1 second
- Advertising payload: a manufacturer-data field with string `Adv_xx` where `xx` increments from 01 up to 65000 and wraps back to 1.

Build & flash (example using `west`):

1. Configure the nRF Connect SDK environment and a board, for example `nrf52832_pca10040`.
2. Build:

```bash
west build -b nrf52832_pca10040 -s .
```

3. Flash:

```bash
west flash
```

If your board differs, replace the `-b` argument with your board's name.
