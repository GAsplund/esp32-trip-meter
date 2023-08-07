#include <BluetoothSerial.h>
#include "config.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

/**
 * @brief Starts Bluetooth serial with the given device name
 */
void setupBT(void)
{
  SerialBT.begin(BT_DEVICE_NAME);
}
