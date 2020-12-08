#pragma once
#include <avr/pgmspace.h>
#include "PARAMETERS.h"
const char Glo[]                PROGMEM = "gloflat";
const char MTN[]                PROGMEM = "web.gprs.mtnnigeria.net";
const char Airtel[]             PROGMEM = "internet.ng.airtel.com";
const char Ninemobile[]         PROGMEM = "9mobile";
const char Default[]            PROGMEM = "web.gprs.mtnnigeria.net";

const char* const comm_Data[]  PROGMEM =
{
  serialNumber
};

const char * const APN[]        PROGMEM =
{
  Glo,
  MTN,
  Airtel,
  Ninemobile,
  Default
};
