// http://elmelectronics.com/DSheets/ELM327DS.pdf
// https://en.wikipedia.org/wiki/OBD-II_PIDs
//  PID list:
//   PID  Name  Min  Max  Formula      Bytes
//   2110 a0    0    1024  256*A + B   2
//   2111 a1    0    1024  256*A + B   2
//   2112 a2    0    1024  256*A + B   2
//   2113 a3    0    1024  256*A + B   2
//   2114 a4    0    1024  256*A + B   2
//   2115 a5    0    1024  256*A + B   2

#include "elm327.h"
#include "trip.h"
#include "Stream.h"

/**
 * @brief Makes sure that the hex value is always `digits` long
 *
 * Torque needs leading zeroes in its values. This function takes care of this.
 *
 * @param a The value to convert to hex
 * @param digits The number of digits to use
 */
String ELM327::converttohex(int a, int digits)
{
  String temp;
  // if a is less than 2^(4*digits)-1, add leading zeroes
  if (a < (pow(16, digits) - 1))
  {
    temp = String(a, HEX);
    if (temp.length() < digits)
    {
      int nrzeroes = digits - temp.length();
      for (int a = 0; a < nrzeroes; a++)
      {
        temp = "0" + temp;
      }
    }
  }
  else
  {
    temp = "0";
  }
  return temp;
}

/**
 * @brief Initializes the ELM327 class
 *
 * @param inputStream a pointer to the data stream to read/write to, e.g. BluetoothSerial or Serial
 * @param inputTrip a pointer to a Trip class
 */
void ELM327::begin(Stream *inputStream, Trip *inputTrip)
{
  this->stream = inputStream;
  this->trip = inputTrip;
}

/**
 * @brief Polls for commands from the BT serial port
 *
 * Reads the BT serial port for commands from the Torque app.
 * Interprets any known commands and sends back data from the Trip class.
 *
 */
void ELM327::poll(void)
{
  String mode, OBDRequest, OBDReturn = "";
  while (stream->available())
  {           // Get data from stream
    delay(3); // wait for buffer to fill up
    if (stream->available() > 0)
    {
      char c = stream->read(); // write character to c
      OBDRequest += c;         // write c to OBDRequest variable
    }
    OBDRequest.trim(); // Remove line feeds
  }

  if (OBDRequest.length() == 0)
    return;

  mode = OBDRequest.substring(0, 2);

  if (mode == "AT")
  { // AT request - used to setup connection between Torque and Arduino
    String ATCommand = OBDRequest.substring(2);
    if (ATCommand == "@1")
    { // Device description
      stream->println("OBDII Arduino Simulator");
    }
    else if (ATCommand == "I")
    { // Torque asks for version ID
      stream->println("VPEE v1.0");
    }
    else if (ATCommand == "SP0")
    { // Torque says to go to auto protocol
      stream->println("OK");
    }
    else if (ATCommand == "DPN")
    { // Torque asks for device protocol by number
      stream->println("1"); // just say it is number 1.
    }
    else if (ATCommand == "RV")
    { // Torque asks for adapter voltage
      stream->println("12.5");
    }
  }
  else if (mode == "21")
  { // Mode 21 - Custom values
    String pid = OBDRequest.substring(2);
    if (pid == "11")
    {
      OBDReturn = "61" + pid + converttohex(trip->getLiters(), 4);
    }
    else if (pid == "12")
    {
      OBDReturn = "61" + pid + converttohex(trip->getKm(), 4);
    }
    else if (pid == "13")
    {
      OBDReturn = "61" + pid + converttohex(trip->getKmh(), 4);
    }
    else if (pid == "14")
    {
      OBDReturn = "61" + pid + converttohex(trip->latestInjectionTime, 4);
    }
    stream->println(OBDReturn);
  }
  else if (mode == "01")
  { // Mode 1 request - Current Data
    String pid = OBDRequest.substring(2, 4);
    if (pid == "00")
    { // PIDs supported (bit-encoded, big endian)
      OBDReturn = "41" + pid + "08180004";
    }
    else if (pid == "05")
    { // Engine coolant temperature
      OBDReturn = "41" + pid + "3C";
    }
    else if (pid == "0C")
    { // Engine speed
      OBDReturn = "41" + pid + converttohex(trip->getRpm() * 4, 4);
    }
    else if (pid == "0D")
    { // Vehicle speed
      OBDReturn = "41" + pid + converttohex(trip->getKmh(), 2);
    }
    else if (pid == "5E")
    { // Engine fuel rate
      OBDReturn = "41" + pid + converttohex(trip->getLph() * 20, 4);
    }
    stream->println(OBDReturn);
  }

  stream->println(">"); // Ready for another command
}
