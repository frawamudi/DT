#include <SPI.h>
#include <ArduinoJson.h>
#include <avr/wdt.h>



#include <math.h>
#include "SoftwareSerial.h"
#include "Flash_Data.h"
#include "max6675.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include "DLMSMeterCode.h"
#include "PARAMETERS.h"
#define RX  11
#define TX  10
#define NETLED  7
bool debug = true;
#define RS485 Serial2
SoftwareSerial    GSM(RX, TX); //RX, TX
const int RS485_control = 30; // A15;
String rs485Data[200] = {};
float RS485_Data_Array[12] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
int rs485Res;
byte resData[35] = {};
int thermoDO = 45;
int thermoCS = 46;
int thermoCLK  = 47;
byte state = LOW;
bool isDebug = true;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int simAPN = 4;
int netCheck = 0;
String response;
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)


#define usingmux4051
int pin_Out_S0 = 31;
int pin_Out_S1 = 32;
int pin_Out_S2 = 33;
int Mux_State1[8] = {0};
int Mux_State2[8] = {0};
int Mux_State3[8] = {0};
int Mux_State4[8] = {0};


int digital_Array[24] = {0};
#define usingmuxdig
int pin_In_Mux_dig_one = 43;
int pin_In_Mux_dig_two = 44;
int enable_dig_one = 34;
int enable_dig_two = 35;
#define usingmuxana
int pin_In_Mux_ana_one = A13;
int pin_In_Mux_ana_two = A14;
int enable_ana_one = 36;
int enable_ana_two = 4;
#include "mux4067.h"
multiplexer  mux;

#define READINGS_NUMBER 10


static uint32_t timer; //HTTPACTION timers
unsigned long SoftRead;

unsigned long ReadRS485;;
const long interval = 2000;
unsigned long LEDtimer;

String Data;
String networkAPN;
String pry = "";
String ser ;
String retVal = "";
String gpsResponse;

#define DEBUG true
String gpsState, timegps = "0";
String data[5];


int loopGSP = 0;
int address = 0;
boolean isLocateSet = false;
byte value;
char c;

char buffer[85];

char server[] = "www.susejgroup.net"; // "";//also change the Host line in httpRequest()  //.. configured in SGC Studio "www.susejtest.us"; //
char path[] =  "/api/connect?serial_number=";

bool errorState = true;

String dataRS485;

#define ONE_WIRE_BUS 14
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


//control pin declaration
#define relay_main_cba 22
#define relay_main_cbb 23

#define relay_a_cba    24
#define relay_a_cbb    25

#define relay_b_cba    26
#define relay_b_cbb    27

#define relay_c_cba    28
#define relay_c_cbb    29

#define relay_d_cba    37
#define relay_d_cbb    38


int smscount = 0;

StaticJsonDocument<200> doc;
DeserializationError error;
void setup()
{
  // Init serial communication
  RS485.begin(9600);
  Serial.begin(115200);
  Serial.println("System Startup");
  delay(2000);
  pinMode(relay_main_cba,  OUTPUT);
  pinMode(relay_a_cba,  OUTPUT);
  pinMode(relay_b_cba ,  OUTPUT);
  pinMode(relay_c_cba , OUTPUT);
  pinMode(relay_d_cba , OUTPUT);

  pinMode(relay_main_cbb,  OUTPUT);
  pinMode(relay_a_cbb,  OUTPUT);
  pinMode(relay_b_cbb ,  OUTPUT);
  pinMode(relay_c_cbb , OUTPUT);
  pinMode(relay_d_cbb , OUTPUT);
  GSM.begin(9600);
  wdt_disable();
  wdt_enable(WDTO_4S);// 4 sec
  // Set RS485 Control pin to OUTPUT
  pinMode(RS485_control, OUTPUT);
  mux.initial_digMux();
  mux.initial_anaMux();
  strcpy_P(buffer, (char*)pgm_read_word(&(serialNumber[0])));
  ser = (String)buffer;
  Debug("DT Monitoring Device Nov. 2019 \n");
  Debug("DT Monitoring Device Version: 1.2 \n");
  Debug("Device Serial Number: " + ser + "\n");
  Debug("DigitalChannel: " + (String)DigitalChannel + "\n");
  Debug("AnalogChannel: " + (String)AnalogChannel + "\n");
  Debug("Latitude:  " + (String)latitude + "\n");
  Debug("Longitude: " + (String)longitude + "\n");
  while (1) {
    //Print "AT" to check if GSM is ON
    wdt_reset();
    GSM.println(F("AT"));
    delay(100);
    if (GSM.available()) {
      pry = GSM.readString();
    }
    //If COntroller receives "OK", jump out of loop
    if (pry.indexOf("OK") != -1) break;
  }
  pry = "";
  Debug("Setting GSM HTTP connection:\n");
  setUpGSM();
  delay(500); //wait for stability
  while (simAPN == 4 && netCheck < 4) {
    wdt_reset();
    attachInternet();
    netCheck++;
    wdt_reset();
    delay(1500);
    wdt_reset();
  }
  Debug(networkAPN);
  wdt_reset();
  delay(500);
  setGSMHTTP(networkAPN);
  Debug(F("\nGSM Setup Complete\n"));
  delay(500);
  sendData("AT+CGNSPWR=1", 1000);
  delay(50);
  sendData("AT+CGNSSEQ=RMC", 1000);

  delay(500);
  sendData("AT+CMGF=1", 1000);
  delay(50);
}
void loop()
{
  response = readSms();
  error = deserializeJson(doc, response);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
  } else {
    controlRelay(doc);
  }
  wdt_reset();
  getRS485Data();
  String address, url;
  unsigned long StateMillis = millis();
  address = "AT+HTTPPARA=\"URL\",\"http://" + String(server) + String(path);
  address.concat(linkGenerate());
  address.concat("\"");
  address.replace(" ", "");
  response = SendData(address); //A function to send GPS coordinates to server
  if (response.indexOf("ERROR") != -1) {
    software_Reset();
  }
  blinkNETLED();
  Debug(response + "\n");
  response = response.substring(response.indexOf(F("{")), response.indexOf(F("}")) + 1);
  error = deserializeJson(doc, response);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  controlRelay(doc);

  if (!isLocateSet)
  {
    getGPSData("AT+CGNSINF", 1000, DEBUG);
  }
  GSM.flush();
  Serial.flush();
}


void controlRelay(StaticJsonDocument<200> doc) {
  const char* c1 = doc["c1"]; // "1"
  const char* c2 = doc["c2"]; // "0"
  const char* c3 = doc["c3"]; // "1"
  const char* c4 = doc["c4"]; // "1"
  const char* c5 = doc["c5"]; // "0"
  Serial.println(String(c1));
  Serial.println(String(c2));
  if (String(c1) == "1") {
    Serial.println(F("c1 on"));
    digitalWrite(relay_main_cba,  1);
    digitalWrite(relay_main_cbb,  0);
    
  } else if (String(c1) == "0") {
    Serial.println(F("c1 off"));
    digitalWrite(relay_main_cba,  0);
    digitalWrite(relay_main_cbb,  1);
    
  }
  if (String(c2) == "1") {
    Serial.println(F("c2 on"));
    digitalWrite(relay_a_cba ,  1);
    digitalWrite(relay_a_cbb ,  0);

  }  else if (String(c2) == "0") {
    Serial.println(F("c2 off"));
    digitalWrite(relay_a_cba ,  0);
    digitalWrite(relay_a_cbb , 1);
  
  }
  if (String(c3) == "1") {
    Serial.println(F("c3 on"));
     digitalWrite(relay_b_cba ,  1);
    digitalWrite(relay_b_cbb ,  0);
  
  }  else if (String(c3) == "0") {
    Serial.println(F("c3 off"));
    digitalWrite(relay_b_cba ,  0);
    digitalWrite(relay_b_cbb ,  1);

  }
  if (String(c4) == "1") {
    Serial.println(F("c4 on"));
    digitalWrite(relay_c_cba ,  1);
    digitalWrite(relay_c_cbb ,  0);
  
  }  else if (String(c4) == "0") {
    Serial.println(F("c4 off"));
    digitalWrite(relay_c_cba ,  0);
    digitalWrite(relay_c_cbb ,  1);
   
  }
  if (String(c5) == "1") {
    Serial.println(F("c5 on"));
    digitalWrite(relay_d_cba ,  1);
    digitalWrite(relay_b_cbb , 0);
   
  }  else if (String(c5) == "0") {
    Serial.println(F("c5 off"));
    digitalWrite(relay_d_cba ,  0);
    digitalWrite(relay_b_cbb , 1);
    
  }
}
