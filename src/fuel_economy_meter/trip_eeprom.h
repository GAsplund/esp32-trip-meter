#ifndef TRIP_EEPROM_H_
#define TRIP_EEPROM_H_

#include <EEPROM.h>

class TripEeprom
{
public:
  void begin(void);

  uint32_t readOdo(void);
  uint64_t readInjectionTime(void);
  uint32_t readInjectionCount(void);

  void writeOdo(uint32_t);
  void writeInjectionTime(uint64_t);
  void writeInjectionCount(uint32_t);

};

#endif // TRIP_EEPROM_H_
