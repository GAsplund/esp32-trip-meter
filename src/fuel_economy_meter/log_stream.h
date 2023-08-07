#ifndef LOG_STREAM_H_
#define LOG_STREAM_H_

#include "Stream.h"
#include "trip.h"

/**
 * @brief Class for logging trip status to a stream in readable format
 */
class LogStream
{
public:
  void begin(Stream*, Trip*);
  void log();

private:
  Stream *stream;
  Trip *trip;
};

#endif // LOG_STREAM_H_
