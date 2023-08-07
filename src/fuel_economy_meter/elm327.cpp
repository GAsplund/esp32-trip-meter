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
  {           // Get data from BT serial port
    delay(3); // wait for buffer to fill up
    if (stream->available() > 0)
    {
      char c = stream->read(); // write character to c
      OBDRequest += c;          // write c to OBDRequest variable
    }
    OBDRequest.trim(); // get rid of line feeds
  }
  if (OBDRequest.length() == 0) return; // Got no data

  mode = OBDRequest.substring(0, 2);

  if (mode == "AT")
  {                                      // Is it an AT request? used to setup connection between Torque and Arduino
    String ATCommand = OBDRequest.substring(2); // Get AT request from String
    Serial.println(ATCommand);           // used for debugging to see what the Torque app is asking
    if (ATCommand == "@1")
    {                                              // Torque asks for device description
      stream->println("OBDII Arduino Simulator"); // Give it a suitable name
    }
    if (ATCommand == "I")
    {                                // Torque asks for version ID
      stream->println("VPEE v1.0"); //
    }
    if (ATCommand == "SP0")
    {                         // Torque says to go to auto protocol
      stream->println("OK"); // Ok
    }
    if (ATCommand == "DPN")
    {                        // Torque asks for device protocol by number
      stream->println("1"); // just say it is number 1.
    }
    if (ATCommand == "RV")
    { // Torque askes for adapter voltage
      stream->println("12.5");
    }
  } else if (mode == "21")
  {                                       // Mode 21 is used for custom values.
    String OBDCommand = OBDRequest.substring(2); // Get PID request from Mode 21
    if (OBDCommand == "10")
    {                                                      // This can be any value. Make your own table with PID's, names, length and formulas
      OBDReturn = "61" + OBDCommand + converttohex(trip->getRpm(), 4); // 61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque
    }
    if (OBDCommand == "11")
    {                                                      // This can be any value. Make your own table with PID's, names, length and formulas
      OBDReturn = "61" + OBDCommand + converttohex(trip->getLiters(), 4); // 61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque
    }
    if (OBDCommand == "12")
    {                                                      // This can be any value. Make your own table with PID's, names, length and formulas
      OBDReturn = "61" + OBDCommand + converttohex(trip->getKm(), 4); // 61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque
    }
    if (OBDCommand == "13")
    {                                                      // This can be any value. Make your own table with PID's, names, length and formulas
      OBDReturn = "61" + OBDCommand + converttohex(trip->getKmh(), 4); // 61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque
    }
    if (OBDCommand == "14")
    {                                                      // This can be any value. Make your own table with PID's, names, length and formulas
      OBDReturn = "61" + OBDCommand + converttohex(trip->latestInjectionTime, 4); // 61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque
    }
    if (OBDCommand == "15")
    {                                                      // This can be any value. Make your own table with PID's, names, length and formulas
      OBDReturn = "61" + OBDCommand + converttohex(0, 4); // 61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque
    }
    stream->println(OBDReturn); // Return value to Torque
  } else if (mode == "01") // This if statement is only used to prevent Torque mentioning no PID's are available at startup.
  {                                       // Mode 1 request
    String OBDCommand = OBDRequest.substring(2); // Get PID request from Mode 1
    if (OBDCommand == "00")
    {                                             // Torque requests available PID's from Arduino
      OBDReturn = "41" + OBDCommand + "08000000"; // 41 means return Mode 1 value + 40hex. 08000000hex = only coolant temp available (just to avoid error in Torque)
    }
    if (OBDCommand == "05")
    {                                       // Torque requests available PID's from Arduino
      OBDReturn = "41" + OBDCommand + "3C"; // 41 means return Mode 1 value + 40hex. 08000000hex = only coolant temp available (just to avoid error in Torque)
    }
    stream->println(OBDReturn); // Return value to Torque
  }

  stream->println(">"); // Let Torque know Arduino is ready.
}
