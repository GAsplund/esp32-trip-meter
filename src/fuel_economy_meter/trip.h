#ifndef TRIP_H_
#define TRIP_H_

#include <Arduino.h>

class Trip {
  public:
    Trip(void);
    
    void begin(void (*injFunc)(void*));
    void injChange();

    uint16_t getRpm(void);
    float getLiters(void);
    float getKm(void);
    float getKmh(void);
    float getEfficiency(float km, float liters);

    volatile uint64_t latestInjectionTime = 0;

  private:
    int16_t getVel();
    
    volatile uint32_t totalInjectionPulses = 0;
    volatile uint64_t totalInjectionTime = 0;
    volatile uint64_t injOpenTimestamp = 0;

    volatile uint32_t totalRpmPulses = 0;
    volatile uint32_t latestRpmTime = 0;
};

#endif // TRIP_H_
