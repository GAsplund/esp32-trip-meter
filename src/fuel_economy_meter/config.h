#ifndef ESP32_TRIP_CONFIG_H_
#define ESP32_TRIP_CONFIG_H_

/*
 * Trip parameters
 */

// GPIO Definitions
#define INJ_GPIO GPIO_NUM_4
#define VSS_GPIO GPIO_NUM_19

// The maximum allowed time between opening and closing the injector
#define INJ_DELTA_MAX 500000
// Amount of time for an injector to have injected 1 liter of fuel
#define INJ_USEC_LITER 53454766.0f

// The maximum allowed time between VSS pulses to calculate velocity
#define VSS_DELTA_MAX 5000000

// Amount of VSS pulses to have travelled 1 Km
#define VSS_PULSE_KM 6840.0f

/*
 * Bluetooth parameters
 */

// Uncomment this to use PIN during pairing. The pin is specified on the line below
//#define USE_PIN
#define BT_PIN "1234"

#define BT_DEVICE_NAME "ESP32-BT-Slave"

#endif // ESP32_TRIP_CONFIG_H_
