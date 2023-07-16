#include <arduino.h>
//#include <driver/mcpwm.h>
#include <driver/pcnt.h>
#include "esp_timer.h"
#include "config.h"

// GPIO Definitions
#define INJ_GPIO GPIO_NUM_4
#define INJ_PIN 4
#define VSS_PIN 19

/*
 * VSS
 */

#define VSS_PCNT_UNIT PCNT_UNIT_0
#define VSS_PCNT_CHAN PCNT_CHANNEL_0
pcnt_config_t vssPcntConfig = {
    .pulse_gpio_num = VSS_PIN,
    .ctrl_gpio_num = -1,
    .pos_mode = PCNT_CHANNEL_EDGE_ACTION_INCREASE,
    .neg_mode = PCNT_CHANNEL_EDGE_ACTION_HOLD,
    .counter_h_lim = 32767,
    .counter_l_lim = 0,
    .unit = VSS_PCNT_UNIT,
    .channel = VSS_PCNT_CHAN
};

class Trip {
  public:
    Trip ();

    void begin(void (*injFunc)(void*));

    volatile uint64_t latestInjectionTime = 0;
    void injChange();

    uint16_t getRpm(void);
    float getLiters(void);
    float getKm(void);
    float getKmh(void);
    float getEfficiency(float km, float liters);

  private:
    int16_t getVel();

    volatile uint32_t totalInjectionPulses = 0;
    volatile uint64_t totalInjectionTime = 0;
    volatile uint64_t injOpenTimestamp = 0;

    volatile uint32_t totalRpmPulses = 0;
    volatile uint32_t latestRpmTime = 0;
};

Trip::Trip() {
  Serial.println("Setting up PCNT");
  pcnt_unit_config(&vssPcntConfig);
  pcnt_counter_pause(VSS_PCNT_UNIT);
  pcnt_counter_clear(VSS_PCNT_UNIT);
  pcnt_event_enable(VSS_PCNT_UNIT, PCNT_EVT_H_LIM);  // Interrupt on high limit.
  //pcnt_isr_handle_t isrHandle;
  //pcnt_isr_register(onHLim, (void *)backupCounter, 0, &isrHandle);
  pcnt_intr_enable(VSS_PCNT_UNIT);
  
  Serial.println("Setting up ISR");
  gpio_pad_select_gpio(INJ_GPIO);
  gpio_set_direction(INJ_GPIO, GPIO_MODE_INPUT);
  gpio_set_pull_mode(INJ_GPIO, GPIO_PULLUP_ONLY);
  gpio_install_isr_service(0);
  gpio_set_intr_type(INJ_GPIO, GPIO_INTR_ANYEDGE);
}

void Trip::begin(void (*injFunc)(void*)) {
  pcnt_counter_resume(VSS_PCNT_UNIT);
  gpio_isr_handler_add(INJ_GPIO, injFunc, (void*)INJ_GPIO);
}

void Trip::injChange() {
  if (gpio_get_level(INJ_GPIO) == 0) {
    this->injOpenTimestamp = esp_timer_get_time();
    this->totalInjectionPulses += 1;
  } else {
    uint64_t delta = esp_timer_get_time() - this->injOpenTimestamp;
    if (delta < INJ_DELTA_MAX) {
      this->latestInjectionTime = delta;
      this->totalInjectionTime += this->latestInjectionTime;
    }
  }
}

uint16_t Trip::getRpm(void) { return this->totalInjectionPulses * 60; }
float Trip::getLiters(void) { return this->totalInjectionTime / INJ_USEC_LITER; }
float Trip::getKm(void) { return this->getVel() / VSS_PULSE_KM; }
float Trip::getKmh(void) { return this->getKm() * 3600; }
float Trip::getEfficiency(float km, float liters) { return (liters * 100) / km; }

int16_t Trip::getVel() {
  int16_t frequency = 0;
  pcnt_get_counter_value(VSS_PCNT_UNIT, &frequency);
  pcnt_counter_clear(VSS_PCNT_UNIT);
  return frequency;
}
