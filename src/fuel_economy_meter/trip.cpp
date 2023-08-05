#include "trip.h"

#include <arduino.h>
#include "esp_timer.h"
#include "config.h"

Trip *Trip::sTrip = 0;

Trip::Trip()
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
}

/**
 * Starts the VSS counter and adds interrupts for the injector and VSS sensor
 */
void Trip::begin()
{
  pcnt_counter_resume(VSS_PCNT_UNIT);
  gpio_isr_handler_add(INJ_GPIO, Trip::updateTripInjISR, (void *)INJ_GPIO);
  gpio_isr_handler_add(VSS_GPIO, Trip::updateTripVssISR, (void *)VSS_GPIO);
}

void Trip::injChange()
{
  if (gpio_get_level(INJ_GPIO) == 0)
  {
    uint64_t openTimestamp = esp_timer_get_time();
    if (this->injOpenTimestamp > 0)
      this->latestInjectionPeriod = openTimestamp - this->injOpenTimestamp;
    this->injOpenTimestamp = openTimestamp;
    this->totalInjectionPulses += 1;
  }
  else
  {
    uint64_t delta = esp_timer_get_time() - this->injOpenTimestamp;

    if (delta < INJ_DELTA_MAX)
    {
      this->latestInjectionTime = delta;
      this->totalInjectionTime += this->latestInjectionTime;
    }
  }
}

void IRAM_ATTR Trip::updateTripInjISR()
{
  if (sTrip != 0)
    sTrip->injChange();
}

void Trip::vssPulse()
{
  uint64_t pulseTimestamp = esp_timer_get_time();
  uint64_t delta = pulseTimestamp - this->latestVssTime;
  if (delta < VSS_DELTA_MAX)
    this->latestVssPeriod = delta;
  this->totalVssPulses += 1;
  this->latestVssTime = esp_timer_get_time();
}

void IRAM_ATTR Trip::updateTripVssISR()
{
  if (sTrip != 0)
    sTrip->vssPulse();
}

uint16_t Trip::getRpm(void) { return (this->latestInjectionPeriod > 0) ? 60000000 / this->latestInjectionPeriod : 0; }

float Trip::getLiters(void) { return this->totalInjectionTime / INJ_USEC_LITER; }
float Trip::getKm(void) { return this->getVel() / VSS_PULSE_KM; }
float Trip::getKmh(void) { return this->getKm() * 3600; }
float Trip::getEfficiency(float km, float liters) { return (liters * 100) / km; }

int16_t Trip::getVel() { return (this->latestVssPeriod > 0) ? 1000000 / this->latestVssPeriod : 0; }
