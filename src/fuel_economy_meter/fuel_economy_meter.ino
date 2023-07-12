#include "trip.h"
#include "bt.h"
#include "elm327.h"

Trip trip;

void IRAM_ATTR injChange(void *args) {
  trip.injChange();
}

void setup()
{
  Serial.begin(9600);

  /*mcpwm_set_pin(VSS_MCPWM_UNIT, &vssMcpwmConfig);
  mcpwm_capture_enable(VSS_MCPWM_UNIT, VSS_MCPWM_CAP, VSS_MCPWM_EDGE, VSS_MCPWM_PNUM);*/
  setupBT();
  trip.begin(&injChange);

  Serial.println("Setup done");
}

void loop()
{
  delay(1000);

  float velKmh = trip.getKmh();
  float distKm = trip.getKm();
  float vol = trip.getLiters();

  float eff = trip.getEfficiency(distKm, vol);

  SerialBT.print(trip.latestInjectionTime);
  SerialBT.print(" uS/inj ");
  SerialBT.print(trip.getRpm());
  SerialBT.print(" RPM ");
  SerialBT.print(velKmh);
  SerialBT.print(" km/h ");
  SerialBT.print(vol * 60);
  SerialBT.print(" L/h ");

  if (velKmh > 1) SerialBT.print(eff);
  else SerialBT.print("N/A");

  SerialBT.println(" L/100km");
}
