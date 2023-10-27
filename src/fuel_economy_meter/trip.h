#ifndef TRIP_H_
#define TRIP_H_

#include <Arduino.h>

/**
 * @brief Class for handling fuel and velocity calculations
 */
class Trip
{
public:
  void begin(void);

  uint_fast16_t getRpm(void);
  float getLiters(void);
  float getKm(void);
  float getKmh(void);
  float getEfficiency();
  float getLph(void);

  volatile uint32_t latestInjectionTime = 0; /// The duty time of the last injection pulse

private:
  static Trip *sTrip;

  float getVel();
  float getDuty(void);

  static IRAM_ATTR void updateTripInjISR(void*);
  static IRAM_ATTR void updateTripVssISR(void*);
  void injChange();
  void vssPulse();

  volatile uint32_t totalInjectionPulses = 0;
  volatile uint64_t totalInjectionTime = 0;
  volatile uint32_t totalRpmPulses = 0;

  volatile uint32_t latestInjectionPeriod = 0;
  volatile uint32_t latestRpmTime = 0;

  volatile uint64_t injOpenTimestamp = 0;

  volatile uint32_t totalVssPulses = 0;
  volatile uint64_t latestVssTimestamp = 0;
  volatile uint32_t latestVssPeriod = 0;
};

#endif // TRIP_H_
