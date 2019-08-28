Simple BLE
==========

A simplified wrapper for Nordic's BLE Softdevice API.

## Initialization
Simple BLE relies on a library context, which must be initialized before use of
the library. To initialize this context, you must supply a configuration struct
`simple_ble_config_t` to define the device address, name, and advertising and
connection interval settings.

```c
simple_ble_config_t ble_config = {
    .platform_id        = 0x42,         // 4th octet to determine device type
    .device_id          = 0xAABB,       // Last two octets defines device ID
    .adv_name           = "BLE Name",   // Used as name in advertisements if room
    .adv_interval       = MSEC_TO_UNITS(1000, UNIT_0_625_MS), // Advertise every second
    .min_conn_interval  = MSEC_TO_UNITS(500, UNIT_1_25_MS),   // Minimum connection interval is 500ms
    .max_conn_interval  = MSEC_TO_UNITS(1000, UNIT_1_25_MS),  // Maximum connection interval is 1s
}
```

After defining a config, allocate space for a pointer to BLE library context:
```
simple_ble_app_t* simple_ble_app;
```

Next, initialize the Simple BLE library:
```
simple_ble_app = simple_ble_init(&ble_config);
```

From here, the library is initialized and can be used to send advertisements,
and manage connections.

## Advertising
In order to advertise, two things must be done:
1. Register new advertisement data with the softdevice.
2. Tell the softdevice to start advertising.

The function `simple_ble_set_adv` performs both steps for us:
```c
simple_ble_set_adv(ble_advdata_t* adv_data, ble_advdata_t* scan_rsp_data);
```
Where `ble_advdata_t` is a Nordic struct defined [here](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.3.0/structble__advdata__t.html).
Advertising can be controlled with the functions `advertising_start()` and
`advertising_stop()`, which begin and stop advertising, respectively.

In addition to these fine grained functions, Simple BLE also offers functions
that set advertising data to common values.  One of the simplest ways to start
advertising is to just advertise the device name:
```c
simple_ble_adv_only_name();
```
To advertise manufacturer specific data:
```c
simple_ble_adv_manuf_data(uint8_t* buf, size_t len);
```
Where `buf` points to an arbitrary buffer, and `len` specifies the length of
the buffer. Note that only 24 bytes can be sent as manufacturer specific data.

To advertise an Eddystone URL:
```c
simple_ble_es_adv(const char* url_str, const ble_advdata_t* scan_rsp_data);
```
Where the `url_str` is a URL string, and `scan_rsp_data` is any additional data
you want to include in the scan response. Provide `NULL` to just
advertise the URL.

There are additional eddystone advertisement functions that advertise a URL
with manufacturer data, or with a name. Find these functions in `simple_ble.h`

See the [advertisement
example](https://github.com/lab11/nrf52x-base/tree/master/apps/ble/ble_adv) and
[eddystone
example](https://github.com/lab11/nrf52x-base/tree/master/apps/ble/ble_eddystone)
to see advertising in action.

## Connections
In addition to advertisements, Simple BLE supports connections and establishing
a GATT by defining services and characteristics.

### Services
To define a service, declare a `simple_ble_service_t` struct and populate its
`.uuid128` field with a [UUID](https://www.uuidgenerator.net/). Note that it
must be defined in little-endian format.
```c
// UUID: 979b5a10-81e8-4531-89c9-90024f2984a2
static simple_ble_service_t my_service= {{
    .uuid128 = {0xa2,0x84,0x29,0x4f,0x02,0x90,0xc0,0x89,
                0x31,0x45,0xe8,0x81,0x10,0x5a,0x9b,0x97}
}};

```

To add this service to your device, simply:
```c
simple_ble_add_service(&my_service);
```

### Characteristics
A service is pretty much useless with characteristics to interact with. Luckily
characteristics are even easier to define. First determine the short UUID16
from the larger UUID128, which is the last two bytes in the first group. In
this example: `0x5a10`. The first characteristic in this service will have the
UUID16 `0x5a11`, and we can define it and a variable to store its data like so:
```c
static simple_ble_char_t my_char = {.uuid16 = 0x5a11};
static data_type my_char_data
```
Register the characteristic to add it to your device:
```c
  simple_ble_add_characteristic(1, 1, 0, 0, // read, write, notify, vlen
      sizeof(my_char_data), (uint8_t*)&my_char_data,
      &my_service, &my_characteristic);
```

Simple BLE supports user-defined callbacks for BLE events and statuses like
connections, disconnections, and characteristic writes. Simply define these
functions and they will be called after events are emitted by the BLE stack.
```c
// Callback for connected event
extern void ble_evt_connected(ble_evt_t const* p_ble_evt);
// Callback for disconnected event
extern void ble_evt_disconnected(ble_evt_t const* p_ble_evt);
// Callback for when perhiperal is written to
extern void ble_evt_write(ble_evt_t const* p_ble_evt);
```
The helper function `simple_ble_is_char_evt` returns true if the event
corresponds to a specific characteristic:
```c
bool simple_ble_is_char_event(ble_evt_t const* p_ble_evt, simple_ble_char_t* char_handle);
```
See the [service example](https://github.com/lab11/nrf52x-base/tree/master/apps/ble/ble_service) to see these functions in action.

## Listening
The `nRF52840` and `nRF52832` both support dual peripheral and central roles.
Simple BLE supports scanning for BLE peripherals with the function
`scanning_start()`. Users need to define the callback `ble_evt_adv_report` to
process incoming advertisements.
```c
extern void ble_evt_adv_report(ble_evt_t const* p_ble_evt);
```
Use Nordic's [`ble_gap_evt_adv_report_t`](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.s132.api.v6.1.1/structble__gap__evt__adv__report__t.html) to parse advertisement data like so:
```c
ble_gap_evt_adv_report_t const* adv_report = &(p_ble_evt->evt.gap_evt.params.adv_report);
```
From this report you can get data like the advertiser's address, its advertisement data, and advertisement data length.


