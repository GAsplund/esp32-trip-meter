#include "trip.h"

#include <arduino.h>
#include "esp_timer.h"
#include "config.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "driver/mcpwm.h"
#include "driver/timer.h"

#define APB_TICK_US(ticks) ((ticks) / (APB_CLK_FREQ / 1000000))

Trip *sTrip = 0;

/**
 * Starts the VSS interrupts and adds interrupts for the injector and VSS sensor
 */
void Trip::begin(void)
{
  sTrip = this;

  Serial.println("Setting up timers");
  timer_init(TIMER_GROUP_0, TIMER_0, &timeout_timer_conf);
  timer_init(TIMER_GROUP_0, TIMER_1, &timeout_timer_conf);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, INJ_TICKS_TIMEOUT);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_1, VSS_TICKS_TIMEOUT);
  timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, &Trip::timeoutInjISR, NULL, 0);
  timer_isr_callback_add(TIMER_GROUP_0, TIMER_1, &Trip::timeoutVssISR, NULL, 0);

  Serial.println("Setting up injector ISR");
  gpio_set_pull_mode(INJ_GPIO, GPIO_PULLUP_ONLY);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, INJ_GPIO);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_1, INJ_GPIO);
  mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, MCPWM_NEG_EDGE, 0);
  mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP1, MCPWM_POS_EDGE, 0);
  mcpwm_isr_register(MCPWM_UNIT_0, Trip::tripInjISRHandler, NULL, ESP_INTR_FLAG_IRAM, NULL);

  Serial.println("Setting up VSS ISR");
  gpio_set_pull_mode(VSS_GPIO, GPIO_PULLUP_ONLY);
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM_CAP_0, VSS_GPIO);
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM_CAP_1, VSS_GPIO);
  mcpwm_capture_enable(MCPWM_UNIT_1, MCPWM_SELECT_CAP0, MCPWM_NEG_EDGE, 0);
  mcpwm_capture_enable(MCPWM_UNIT_1, MCPWM_SELECT_CAP1, MCPWM_POS_EDGE, 0);
  mcpwm_isr_register(MCPWM_UNIT_1, Trip::tripVssISRHandler, NULL, ESP_INTR_FLAG_IRAM, NULL);

  Serial.println("Starting timers");
  timer_start(TIMER_GROUP_0, TIMER_0);
  timer_start(TIMER_GROUP_0, TIMER_1);
}

/**
 * @brief Handles a pulse in the injector signal
 *
 * Used to calculate total and momentary injection time.
 * Injector is considered open when the signal is low due to MOSFET pulling it down.
 * 
 *     ╭―(+ Inj -)―╮
 *     +           o - pull-down signal
 *     |           |
 *     -           / - ECU MOSFET
 *     ╰―――――――――――╯
 */
void Trip::injPulse(uint32_t openCap, uint32_t closeCap)
{
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, INJ_TICKS_TIMEOUT);

  // Make sure that the injector has been open before
  // We also don't know how many times we could have 
  // overflowed before, so we discard any additional captures
  if (this->latestInjectionTimestamp == 0)
  {
    this->latestInjectionTimestamp = openCap;
    return;
  }

  uint32_t dutyTime = openCap > closeCap ?
    (0xFFFFFFFF - openCap) + closeCap + 1:
    closeCap - openCap;

  if (dutyTime <= INJ_DELTA_MAX)
  {
    this->latestInjectionDutyTime = APB_TICK_US(dutyTime);
    this->latestInjectionPeriod = APB_TICK_US(openCap - this->latestInjectionTimestamp);
  }
  
  this->latestInjectionTimestamp = openCap;
}

void IRAM_ATTR Trip::tripInjISRHandler(void* arg)
{
  if (MCPWM0.int_st.cap1_int_st && sTrip != 0)
    sTrip->injPulse(mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP0), mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP1));
  
  MCPWM0.int_clr.val = MCPWM0.int_st.val;
}

/**
 * @brief Handles a pulse in the VSS signal
 *
 * Uses the length of the pulse to calculate the velocity of the vehicle.
 * If the time since the last pulse isn't too long, the pulse is considered to be valid.
 */
void Trip::vssPulse(uint32_t upCap, uint32_t downCap)
{
  timer_set_counter_value(TIMER_GROUP_0, TIMER_1, VSS_TICKS_TIMEOUT);
  this->totalVssPulses++;

  if(isStopped)
  {
    isStopped = false;
    return;
  }

  // We assume that the HIGH time is always half a period
  uint32_t period = 2 * (upCap > downCap ?
    (0xFFFFFFFF - upCap) + downCap + 1:
    downCap - upCap);

  if (period < VSS_DELTA_MAX)
    this->latestVssPeriod = APB_TICK_US(period);
}

void IRAM_ATTR Trip::tripVssISRHandler(void* arg)
{
  if (MCPWM1.int_st.cap1_int_st && sTrip != 0)
    sTrip->vssPulse(mcpwm_capture_signal_get_value(MCPWM_UNIT_1, MCPWM_SELECT_CAP0), mcpwm_capture_signal_get_value(MCPWM_UNIT_1, MCPWM_SELECT_CAP1));
  
  MCPWM1.int_clr.val = MCPWM1.int_st.val;
}

/**
 * @brief Handles a timeout in the injector signal
 *
 * Used to calculate the total RPM of the engine.
 * If the injector has not opened for a long time, the engine is considered to be stopped.
 */
void Trip::timeoutInj()
{
  this->latestInjectionPeriod = 0;
  this->latestInjectionDutyTime = 0;
  this->latestInjectionTimestamp = 0;
}

bool IRAM_ATTR Trip::timeoutInjISR(void*)
{
  Serial.println("Inj timeout");
  if (sTrip != 0)
    sTrip->timeoutInj();
  return false;
}

/**
 * @brief Handles a timeout in the VSS signal
 *
 * Used to calculate the total velocity of the vehicle.
 * If the VSS signal has not pulsed for a long time, the vehicle is considered to be stopped.
 */
void Trip::timeoutVss()
{
  this->latestVssPeriod = 0;
  this->isStopped = true;
}

bool IRAM_ATTR Trip::timeoutVssISR(void*)
{
  Serial.println("VSS timeout");
  if (sTrip != 0)
    sTrip->timeoutVss();
  return false;
}

/**
 * @brief Calculates current engine RPM
 *
 * Uses injector pulse period to calculate the rotation speed of the engine in RPM
 *
 * Given that one single injector fires per engine rotation, the RPM is calculated
 * by dividing 60 seconds by the injector pulse period.
 *
 * Since f = 1/T, the formula is 60/T
 * This implicitly converts from rotations per second (Hz) to rotations per minute.
 * Since the injector pulse period is in microseconds, we multiply by 1e6 to get seconds.
 *
 * @return the current engine RPM
 */
//uint16_t Trip::getRpm(void) { return (this->latestInjectionPeriod > 0) ? (60 * 1000000) / this->latestInjectionPeriod : 0; }
uint_fast16_t Trip::getRpm(void) { return (this->latestInjectionPeriod > 0) ? 60000000 / this->latestInjectionPeriod : 0; }

/**
 * Calculates the total fuel spent
 */
float Trip::getLiters(void) { return this->totalInjectionTime * (1 / INJ_USEC_LITER); }

/**
 * @brief Calculates the duty cycle on the injector
 */
float Trip::getDuty(void) { return (this->latestInjectionPeriod > 0) ? (float) this->latestInjectionDutyTime / (float) this->latestInjectionPeriod : 0; }

/**
 * @brief Calculates momentary fuel consumption
 */
//float Trip::getLph(void) { return (3.6E9 * getDuty()) / INJ_USEC_LITER; }
float Trip::getLph(void) { return (3600000000.0f / INJ_USEC_LITER) * getDuty(); }

/**
 * Calculates the total distance traveled in km
 */
float Trip::getKm(void) { return this->totalVssPulses * (1 / VSS_PULSE_KM); }

/**
 * Calculates the velocity in km/h
 */
float Trip::getKmh(void) { return (this->latestVssPeriod > 0) ? (this->getVel() / VSS_PULSE_KM) * 3600 : 0; }
//float Trip::getKmh(void) { return (esp_timer_get_time() - this->latestVssTimestamp > VSS_DELTA_MAX) ? 0 : ( 3600/VSS_PULSE_KM ) * this->getVel(); }

/**
 * @brief Calculates the momentary fuel efficiency
 *
 * Calculates the fuel efficiency based on the velocity and fuel consumption rate.
 * Efficiency is calculated per km, which is multiplied by 100 to get a proper efficiency value.
 *
 * @return the momentary efficiency in L/100km
 */
float Trip::getEfficiency() { 
  float kmh = this->getKmh();
  return (kmh > 0) ? (this->getLph() * 100) / kmh : 0;
}

/**
 * @brief Gets the amount of VSS pulses per second
 *
 * Calculated using f = 1/T, where T is the latest VSS pulse period.
 * We also multiply by 1e6 to convert from seconds to microseconds.
 *
 * @return the amount of VSS pulses per second based on the latest period
 */
float Trip::getVel() { return this->isStopped ? 0 : 1000000.0 / this->latestVssPeriod; }
//float Trip::getVel() { return (this->latestVssPeriod > 0) ? 1000000.0f / this->latestVssPeriod : 0; }
