#include "trip.h"
#include "bt.h"
#include "elm327.h"
#include "log_stream.h"

static Trip trip;
static ELM327 elm327;
//static LogStream logStream;

void setup()
{
  Serial.begin(9600);
  Serial.println("BT");
  setupBT();
  Serial.println("Trip");
  trip.begin();
  elm327.begin(&SerialBT, &trip);
  //Serial.println("LogStream");
  //logStream.begin(&Serial, &trip);

  Serial.println("Setup done");
}

void loop()
{
  //delay(1000);
  //logStream.log();
  elm327.poll();
}
