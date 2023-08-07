#include "trip.h"
#include "bt.h"
#include "elm327.h"
#include "log_stream.h"

static Trip trip;
//static ELM327 elm327;
static LogStream logStream;

void setup()
{
  Serial.begin(9600);
  setupBT();
  trip.begin();
  //elm327.begin(&SerialBT, &trip);
  logStream.begin(&Serial, &trip);

  Serial.println("Setup done");
}

void loop()
{
  delay(1000);
  logStream.log();
}
