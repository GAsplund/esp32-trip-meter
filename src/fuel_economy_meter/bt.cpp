#include "BluetoothSerial.h"

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234";

String device_name = "ESP32-BT-Slave";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setupBT() {
  SerialBT.begin(device_name);
}
