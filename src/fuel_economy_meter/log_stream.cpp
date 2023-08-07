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
  float distKm = trip->getKm();
  float vol = trip->getLiters();

  float eff = trip->getEfficiency(distKm, vol);

  stream->print(trip->latestInjectionTime);
  stream->print(" uS/inj ");
  stream->print(trip->getRpm());
  stream->print(" RPM ");
  stream->print(velKmh);
  stream->print(" km/h ");
  stream->print(vol * 60);
  stream->print(" L/h ");

  if (velKmh > 1)
    stream->print(eff);
  else
    stream->print("N/A");

  stream->println(" L/100km");
}
