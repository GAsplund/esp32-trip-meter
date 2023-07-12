#ifndef TRIP_H_
#define TRIP_H_

#include <arduino.h>

class Trip {
  public:
    void begin(void (*injFunc)(void*));
    void injChange();

    uint16_t getRpm(void);
    float getLiters(void);
    float getKm(void);
    float getKmh(void);
    float getEfficiency(float km, float liters);

    volatile uint64_t latestInjectionTime;

  private:
    int16_t getVel();
    
    volatile uint32_t totalInjectionPulses;
    volatile uint64_t totalInjectionTime;
    volatile uint64_t injOpenTimestamp;

    volatile uint32_t totalRpmPulses;
    volatile uint32_t latestRpmTime;
}

#endif // TRIP_H_
