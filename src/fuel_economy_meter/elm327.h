#ifndef ELM327_H_
#define ELM327_H_

#include "Stream.h"
#include "trip.h"

/**
 * @brief Class for emulating an ELM327 OBD-II adapter
 */
class ELM327
{
public:
  void begin(Stream*, Trip*);
  void poll();

  static String converttohex(int a, int decimals);

private:
  Stream *stream;
  Trip *trip;
};

#endif // ELM327_H_
