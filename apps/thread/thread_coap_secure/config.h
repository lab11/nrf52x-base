#define GATEWAY_DEVICE_TYPE "epa-mote"

#define COAP_SERVER_HOSTNAME "coap-test.permamote.com"
#define NTP_SERVER_HOSTNAME "us.pool.ntp.org"
#define UPDATE_SERVER_HOSTNAME "dfu.permamote.com"
#define DNS_SERVER_ADDR "fdaa:bb:1::2"
#define GATEWAY_PARSE_ADDR "blueirislabs.github.io/epa-mote/gateway/"

#define ID_LOCATOR 0x1000

#define DEFAULT_CHILD_TIMEOUT    2*60  /**< Thread child timeout [s]. */
#define DEFAULT_POLL_PERIOD      1000  /**< Thread Sleepy End Device polling period when Asleep. [ms] */
#define DFU_POLL_PERIOD          15
#define RECV_POLL_PERIOD         15    /**< Thread Sleepy End Device polling period when expecting response. [ms] */
#define NUM_SLAAC_ADDRESSES      6     /**< Number of SLAAC addresses. */

#define VOLTAGE_PERIOD      5
#define TPH_PERIOD          5
#define COLOR_PERIOD        5
#define DISCOVER_PERIOD     15
#define SENSOR_PERIOD       APP_TIMER_TICKS(60*1000)
#define PIR_BACKOFF_PERIOD  APP_TIMER_TICKS(2*60*1000)
#define PIR_DELAY           APP_TIMER_TICKS(10*1000)
#define DFU_MONITOR_PERIOD  APP_TIMER_TICKS(5*1000)
#define COAP_TICK_TIME      APP_TIMER_TICKS(1*1000)
#define NTP_UPDATE_HOURS    1
#define NTP_UPDATE_MAX      5*1000
#define NTP_UPDATE_MIN      2*1000
#define DFU_CHECK_HOURS     24
#define MAX_OFFLINE_MINS    60*24 // one day

// Taken from google_iot_coap example in the nrf openthread/zigbee sdk

/**
 * CoAP transport configuration.
 * Must be configured by the user.
 */
#define GCP_COAP_SECURE_ENABLED               1
#define GCP_COAP_SECURE_PSK_SECRET            "blah"
#define GCP_COAP_SECURE_PSK_IDENTITY          "test"

/**
 * Google Cloud Platform CoAP server parameters.
 */
#define GCP_COAP_IOT_CORE_SERVER_PORT         5683
#define GCP_COAP_IOT_CORE_SERVER_SECURE_PORT  5684

/**
 * Google Cloud Platform project configuration.
 * Must be configured by the user.
 */
#define GCP_COAP_IOT_CORE_SERVER_ADDRESS      "64:ff9b::23ee:2ab0"
#define GCP_COAP_IOT_CORE_PATH                "gcp"
#define GCP_COAP_IOT_CORE_PROJECT_ID          "project-id"
#define GCP_COAP_IOT_CORE_REGISTRY_ID         "registry-id"
#define GCP_COAP_IOT_CORE_REGION              "us-central1"
#define GCP_COAP_IOT_CORE_PUBLISH             "publishEvent"
#define GCP_COAP_IOT_CORE_CONFIG              "config"

/**
 * Google Cloud Platform device configuration.
 * Must be configured by the user.
 */
#define GCP_COAP_IOT_CORE_DEVICE_ID          "example"
#define GCP_COAP_IOT_CORE_DEVICE_KEY         "blah add your own"

/**
 * The JSON representation of the header with ES256 algorithm.
 */
#define JWT_HEADER_TYPE_ES256 \
    "{\"alg\":\"ES256\",\"typ\":\"JWT\"}"

/**
 * The maximum size of the JWT signature.
 */
#define JWT_SIGNATURE_SIZE 64

/**
 * The size of key length for ES256.
 */
#define JWT_KEY_LENGTH_ES256 32

/**
 * The JWT delimeter used to separete header, claim and signature.
 *
 */
#define JWT_DELIMETER '.'
