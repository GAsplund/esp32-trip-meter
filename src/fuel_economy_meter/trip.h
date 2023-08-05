#ifndef TRIP_H_
#define TRIP_H_

#include <Arduino.h>

/**
 * Class for handling fuel and velocity calculations
 */
class Trip
{
public:
  Trip(void);

  void begin();
  void injChange();

  uint16_t getRpm(void);
  float getLiters(void);
  float getKm(void);
  float getKmh(void);
  float getEfficiency(float km, float liters);

  volatile uint64_t latestInjectionTime = 0;

private:
  static Trip *sTrip;

  int16_t getVel();
  static IRAM_ATTR void updateTripInjISR();
  static IRAM_ATTR void updateTripVssISR();

  volatile uint32_t totalInjectionPulses = 0;
  volatile uint64_t totalInjectionTime = 0;
  volatile uint64_t totalInjectionTime = 0;
  volatile uint32_t totalRpmPulses = 0;

  volatile uint32_t latestInjectionPeriod = 0;
  volatile uint32_t latestRpmTime = 0;

  volatile uint64_t injOpenTimestamp = 0;

  volatile uint32_t totalVssPulses = 0;
  volatile uint64_t latestVssTime = 0;
  volatile uint32_t latestVssPeriod = 0;
};

#endif // TRIP_H_
