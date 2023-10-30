#include "trip_eeprom.h"
#include <EEPROM.h>

/**
 * @brief Initialize the EEPROM
 */
void TripEeprom::begin(void) {
  EEPROM.begin(64);
}

/**
 * @brief Get the distance traveled
 * 
 * @return uint32_t The distance traveled
 */
uint32_t TripEeprom::readOdo(void) {
  return EEPROM.readUInt(0);
}

/**
 * @brief Save the current distance traveled
 * 
 * @param odo The updated distance traveled
 */
void TripEeprom::writeOdo(uint32_t odo) {
  EEPROM.writeUInt(0, odo);
  EEPROM.commit();
}

/**
 * @brief Get the total number of injection pulses
 * 
 * @return uint32_t The total number of injection pulses
 */
uint32_t TripEeprom::readInjectionCount(void) {
  return EEPROM.readUInt(4);
}

/**
 * @brief Save the total number of injection pulses
 * 
 * @param cnt The updated number of injection pulses
 */
void TripEeprom::writeInjectionCount(uint32_t cnt) {
  EEPROM.writeUInt(4, cnt);
  EEPROM.commit();
}

/**
 * @brief Get the total injection time
 * 
 * @return uint64_t The injection time
 */
uint64_t readInjectionTime(void) {
  return EEPROM.readULong(8);
}

/**
 * @brief Save the total injection time
 * 
 * @param time The updated injection time
 */
void writeInjectionTime(uint64_t time) {
  EEPROM.writeULong(8, time);
  EEPROM.commit();
}
