#ifndef TRIP_H_
#define TRIP_H_

#include <Arduino.h>
#include "driver/mcpwm.h"
#include "driver/timer.h"

/**
 * @brief Class for handling fuel and velocity calculations
 */
class Trip
{
public:
  void begin(void);

  uint16_t getRpm(void);
  float getLiters(void);
  float getKm(void);
  float getKmh(void);
  float getEfficiency();
  float getLph(void);

  volatile uint64_t latestInjectionDutyTime = 0; /// The duty time of the last injection pulse

private:
  float getVel();
  float getDuty(void);

  //static IRAM_ATTR void tripInjISR(void*);
  //static IRAM_ATTR void tripVssISR(void*);
  static IRAM_ATTR bool tripInjISR(mcpwm_unit_t, mcpwm_capture_channel_id_t, const cap_event_data_t *, void *);
  static IRAM_ATTR bool tripVssISR(mcpwm_unit_t, mcpwm_capture_channel_id_t, const cap_event_data_t *, void *);

  static IRAM_ATTR bool timeoutInjISR(void*);
  static IRAM_ATTR bool timeoutVssISR(void*);

  void injPulse(uint32_t, uint32_t);
  void vssPulse(uint32_t, uint32_t);
  void timeoutInj();
  void timeoutVss();

  volatile uint32_t totalInjectionPulses = 0;
  volatile uint64_t totalInjectionTime = 0;
  volatile uint32_t totalRpmPulses = 0;

  volatile uint32_t latestInjectionTimestamp = 0;
  volatile uint32_t latestInjectionPeriod = 0;
  volatile uint32_t latestRpmTime = 0;

  volatile uint32_t totalVssPulses = 0;
  volatile uint32_t latestVssPeriod = 0;
  volatile bool isStopped = true;

  const mcpwm_capture_config_t inj_cap0_conf = {
    .cap_edge = MCPWM_NEG_EDGE,
    .capture_cb = NULL
  };

  const mcpwm_capture_config_t inj_cap1_conf = {
    .cap_edge = MCPWM_POS_EDGE,
    .capture_cb = Trip::tripInjISR
  };

  const mcpwm_capture_config_t vss_cap0_conf = {
    .cap_edge = MCPWM_POS_EDGE,
    .capture_cb = NULL
  };

  const mcpwm_capture_config_t vss_cap1_conf = {
    .cap_edge = MCPWM_NEG_EDGE,
    .capture_cb = Trip::tripVssISR
  };

  const timer_config_t timeout_timer_conf = {
    .alarm_en = TIMER_ALARM_EN,
    .counter_en = TIMER_PAUSE,
    .intr_type = TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_DOWN,
    .auto_reload = TIMER_AUTORELOAD_DIS,
    .divider = 2
  };

};

#endif // TRIP_H_
