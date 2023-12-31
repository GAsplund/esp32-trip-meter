#include "trip.h"

#include <arduino.h>
#include "esp_timer.h"
#include "config.h"

Trip *Trip::sTrip = 0;

/**
 * Starts the VSS interrupts and adds interrupts for the injector and VSS sensor
 */
void Trip::begin(void)
{
  sTrip = this;

  Serial.println("Setting up injector ISR");
  gpio_pad_select_gpio(INJ_GPIO);
  gpio_set_direction(INJ_GPIO, GPIO_MODE_INPUT);
  gpio_set_pull_mode(INJ_GPIO, GPIO_PULLUP_ONLY);

  Serial.println("Setting up VSS ISR");
  gpio_pad_select_gpio(VSS_GPIO);
  gpio_set_direction(VSS_GPIO, GPIO_MODE_INPUT);
  gpio_set_pull_mode(VSS_GPIO, GPIO_PULLUP_ONLY);

  Serial.println("Enabling ISR");
  gpio_install_isr_service(0);
  gpio_set_intr_type(INJ_GPIO, GPIO_INTR_ANYEDGE);
  gpio_set_intr_type(VSS_GPIO, GPIO_INTR_POSEDGE);

  gpio_isr_handler_add(INJ_GPIO, Trip::updateTripInjISR, (void *)INJ_GPIO);
  gpio_isr_handler_add(VSS_GPIO, Trip::updateTripVssISR, (void *)VSS_GPIO);
}

/**
 * @brief Handles a pulse change in the injector signal
 *
 * Used to calculate total and momentary injection time.
 * Injector is considered open when the signal is low due to MOSFET pulling it down.
 * 
 *     ╭―(+ Inj -)―╮
 *     +           o - pull-down signal
 *     -           / - MOSFET
 *     ╰―――――――――――╯
 */
void Trip::injChange()
{
  uint64_t changeTimestamp = esp_timer_get_time();
  if (gpio_get_level(INJ_GPIO) == 0)
  {
    if (this->injOpenTimestamp > 0)
      this->latestInjectionPeriod = changeTimestamp - this->injOpenTimestamp;
    this->injOpenTimestamp = changeTimestamp;
    this->totalInjectionPulses += 1;
  }
  else
  {
    uint32_t delta = changeTimestamp - this->injOpenTimestamp;

    if (delta < INJ_DELTA_MAX)
    {
      this->latestInjectionTime = delta;
      this->totalInjectionTime += this->latestInjectionTime;
    }
  }
}

void IRAM_ATTR Trip::updateTripInjISR(void*)
{
  if (sTrip != 0)
    sTrip->injChange();
}

/**
 * @brief Handles a pulse in the VSS signal
 *
 * Gets the timestamp of the pulse and calculates the period between the current and the last pulse.
 * If the period is not too long, it is stored as the latest period.
 *
 * Since the period is only used to calculate velocity, the delta is not needed when the period is
 * too long. (i.e. when the vehicle is stopped)
 */
void Trip::vssPulse()
{
  uint64_t pulseTimestamp = esp_timer_get_time();
  uint32_t delta = pulseTimestamp - this->latestVssTimestamp;
  if (delta < VSS_DELTA_MAX)
    this->latestVssPeriod = delta;
  this->totalVssPulses += 1;
  this->latestVssTimestamp = pulseTimestamp;
}

void IRAM_ATTR Trip::updateTripVssISR(void*)
{
  if (sTrip != 0)
    sTrip->vssPulse();
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
uint_fast16_t Trip::getRpm(void) { return (this->latestInjectionPeriod > 0) ? 60000000 / this->latestInjectionPeriod : 0; }

/**
 * Calculates the total fuel spent
 */
float Trip::getLiters(void) { return this->totalInjectionTime * (1 / INJ_USEC_LITER); }

/**
 * @brief Calculates the duty cycle on the injector
 */
float Trip::getDuty(void) { return (this->latestInjectionPeriod > 0) ? (float) this->latestInjectionTime / (float) this->latestInjectionPeriod : 0; }

/**
 * @brief Calculates momentary fuel consumption
 */
float Trip::getLph(void) { return (3600000000.0f / INJ_USEC_LITER) * getDuty(); }

/**
 * Calculates the total distance traveled in km
 */
float Trip::getKm(void) { return this->totalVssPulses * (1 / VSS_PULSE_KM); }

/**
 * Calculates the velocity in km/h
 */
float Trip::getKmh(void) { return (esp_timer_get_time() - this->latestVssTimestamp > VSS_DELTA_MAX) ? 0 : ( 3600/VSS_PULSE_KM ) * this->getVel(); }

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
float Trip::getVel() { return (this->latestVssPeriod > 0) ? 1000000.0f / this->latestVssPeriod : 0; }
