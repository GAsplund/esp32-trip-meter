#include "log_stream.h"

void LogStream::begin(Stream *inputStream, Trip *inputTrip)
{
  this->stream = inputStream;
  this->trip = inputTrip;
}

/**
 * @brief Prints the current trip status to the stream
 */
void LogStream::log()
{
  float velKmh = trip->getKmh();
  float lph = trip->getLph();
  float eff = trip->getEfficiency();

  stream->print(trip->latestInjectionTime);
  stream->print(" uS/inj ");
  stream->print(trip->getRpm());
  stream->print(" RPM ");
  stream->print(velKmh);
  stream->print(" km/h ");
  stream->print(lph);
  stream->print(" L/h ");

  if (velKmh > 1)
    stream->print(eff);
  else
    stream->print("N/A");

  stream->println(" L/100km");
}
