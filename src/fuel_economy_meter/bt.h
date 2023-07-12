#ifndef BT_H_
#define BT_H_

#include <BluetoothSerial.h>

const char *pin;
String device_name;

BluetoothSerial SerialBT;
void setupBT();

#endif // BT_H_
