//http://elmelectronics.com/DSheets/ELM327DS.pdf
//https://en.wikipedia.org/wiki/OBD-II_PIDs
// PID list:
//  PID  Name  Min  Max  Formula      Bytes
//  2110 a0    0    1024  256*A + B   2
//  2111 a1    0    1024  256*A + B   2
//  2112 a2    0    1024  256*A + B   2
//  2113 a3    0    1024  256*A + B   2
//  2114 a4    0    1024  256*A + B   2 
//  2115 a5    0    1024  256*A + B   2

#include "trip.h"
#include "bt.h"

String OBDRequest,ATCommand,OBDCommand, OBDReturn;
int a0, a1, a2, a3, a4, a5;

//Torque needs leading zero's in its values. This function takes care of this.
String converttohex (int a, int decimals) {
  String temp;
  if (a < (pow(16,decimals) - 1)) {  
    temp = String(a, HEX);
    if (temp.length() < decimals) {
      int nrzeroes = decimals - temp.length();
      for (int a = 0; a < nrzeroes; a++) {
        temp = "0" + temp;
      }  
    }
  } else {
    temp = "0";
  }
  return temp;
}

Trip *trip;

void setupELM327(Trip *inputTrip) {
  trip = inputTrip;
}

void ELM327_poll() {
  //delay(1000);                                       //Debug purposes
  a0 = trip->getRpm();                                 //Read analog pins
  a1 = trip->getLiters();
  a2 = trip->getKm();
  a3 = trip->getKmh();
  a4 = trip->latestInjectionTime;
  a5 = 0;
  while (SerialBT.available()){                        //Get data from BT serial port
    delay(3);                                          //wait for buffer to fill up
    if(SerialBT.available() >0){                       //Serial port ready? 
      char c = SerialBT.read();                        //write character to c
      OBDRequest += c;                                 //write c to OBDRequest variable 
    } 
    OBDRequest.trim();                                 //get rid of line feeds
  }
  if (OBDRequest.length() > 0) {                       //OBDRequest came in?
    if(OBDRequest.substring(0,2) == "AT") {            //Is it an AT request? used to setup connection between Torque and Arduino
      ATCommand = OBDRequest.substring(2);             //Get AT request from String
      Serial.println(ATCommand);                       //used for debugging to see what the Torque app is asking 
      if (ATCommand == "@1") {                         //Torque askes for device description
        SerialBT.println("OBDII Arduino Simulator");   //Give it a suitable name
      }
      if (ATCommand == "I") {                          //Torque askes for version ID
        SerialBT.println("VPEE v1.0");                 //
      }
      if (ATCommand == "SP0") {                        //Torque says to go to auto protocol
        SerialBT.println("OK");                        //Ok
      }
      if (ATCommand == "DPN") {                        //Torque askes for device protocol by number
        SerialBT.println("1");                         //just say it is number 1. 
      }
      if (ATCommand == "RV") {                         //Torque askes for adapter voltage
        SerialBT.println("12.5");  
      }
    }
    
    if(OBDRequest.substring(0,2) == "21") {                       //Mode 21 is used for custom values.
      OBDCommand = OBDRequest.substring(2);                       //Get PID request from Mode 21 
      if (OBDCommand == "10") {                                   //This can be any value. Make your own table with PID's, names, length and formulas
        OBDReturn = "61" + OBDCommand + converttohex(a0,4);       //61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque 
      }
      if (OBDCommand == "11") {                                   //This can be any value. Make your own table with PID's, names, length and formulas
        OBDReturn = "61" + OBDCommand + converttohex(a1,4);       //61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque 
      }
      if (OBDCommand == "12") {                                   //This can be any value. Make your own table with PID's, names, length and formulas
        OBDReturn = "61" + OBDCommand + converttohex(a2,4);       //61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque 
      }
      if (OBDCommand == "13") {                                   //This can be any value. Make your own table with PID's, names, length and formulas
        OBDReturn = "61" + OBDCommand + converttohex(a3,4);       //61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque 
      }
      if (OBDCommand == "14") {                                   //This can be any value. Make your own table with PID's, names, length and formulas
        OBDReturn = "61" + OBDCommand + converttohex(a4,4);       //61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque 
      }
      if (OBDCommand == "15") {                                   //This can be any value. Make your own table with PID's, names, length and formulas
        OBDReturn = "61" + OBDCommand + converttohex(a5,4);       //61 means return Mode 21 value + 40hex. Make sure length return conforms with formula in Torque 
      } 
      SerialBT.println(OBDReturn);                                 //Return value to Torque    
      
    }
    //This if statement is only used to prevent Torque mentioning no PID's are available at startup.
    if(OBDRequest.substring(0,2) == "01") {           //Mode 1 request
      OBDCommand = OBDRequest.substring(2);           //Get PID request from Mode 1
      if (OBDCommand == "00") {                       //Torque requests available PID's from Arduino
        OBDReturn = "41" + OBDCommand + "08000000";   //41 means return Mode 1 value + 40hex. 08000000hex = only coolant temp available (just to avoid error in Torque) 
      }
      if (OBDCommand == "05") {                       //Torque requests available PID's from Arduino
        OBDReturn = "41" + OBDCommand + "3C";         //41 means return Mode 1 value + 40hex. 08000000hex = only coolant temp available (just to avoid error in Torque) 
      }
      SerialBT.println(OBDReturn);                    //Return value to Torque
    }
    
    SerialBT.println(">");                            //Let Torque know Arduino is ready.
  }
  OBDRequest = "";                                    //clear String when ready.
}
