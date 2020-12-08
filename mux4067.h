
#ifdef usingmux4067
// this must be place inside the code for it to work fine
// I.E the declearation below

/*

  #define usingmux4067
  #define AINMUX  A12      // Analog In Pin 0
  #define DIGMUX A13      // DIGITAL In Pin 0


  #define muxPin0 33  // Digital Pin 2
  #define muxPin1 32 // Digital Pin 3
  #define muxPin2 31 // Digital Pin 4
  #define muxPin3 30 // Digital Pin 5
  int ANEN = A14;
  int DIGEN = A15;
  int muxValue;
*/


#include <Arduino.h>


class multiplexer {
  public:
    void initial();
    String updateMux1 (int k);
    String  updateMux2 (int k);

};


multiplexer::initial() {
  pinMode(muxPin0, OUTPUT);
  pinMode(muxPin1, OUTPUT);
  pinMode(muxPin2, OUTPUT);
  pinMode(muxPin3, OUTPUT);
  pinMode(ANEN, OUTPUT);
  pinMode(DIGEN, OUTPUT);
  pinMode(DIGMUX, INPUT);
}


String multiplexer::analogMux(int channel)
{
  digitalWrite(DIGEN, HIGH);
  digitalWrite(ANEN, LOW);
  for (int i = 0; i < 16; i++)
  {
    digitalWrite(muxPin0, bitRead(channel, 0));
    digitalWrite(muxPin1, bitRead(channel, 1));
    digitalWrite(muxPin2, bitRead(channel, 2));
    digitalWrite(muxPin3, bitRead(channel, 3));

    // read from the selected  channel (A0)
    muxValue = analogRead(AINMUX);
  }
  // return the  analog value
  return String(muxValue);
}


String multiplexer::digitalMux(int channel)
{
  digitalWrite(DIGEN, LOW);
  digitalWrite(ANEN, HIGH);
  for (int i = 0; i < 16; i++)
  {
    digitalWrite(muxPin0, bitRead(channel, 0));
    digitalWrite(muxPin1, bitRead(channel, 1));
    digitalWrite(muxPin2, bitRead(channel, 2));
    digitalWrite(muxPin3, bitRead(channel, 3));

    // read from the selected  channel (A0)
    muxValue = digitalRead(DIGMUX);
  }
  // return the  analog value
  return String(muxValue);
}

#endif //usingmux4067



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




#ifdef usingmux4051
/*
  #define usingmux4051
  int pin_Out_S0 = 41;
  int pin_Out_S1 = 40;
  int pin_Out_S2 = 39;
  int pin_In_Mux1 = A1;
  int pin_In_Mux2 = A0;
  int Mux1_State[8] = {0};
  int Mux2_State[8] = {0};
  int Enable_1 = 15;
  int Enable_2 = 14;
*/

#include <Arduino.h>
class multiplexer {
  public:
    initial();
    initial_digMux();
    initial_anaMux();
    int analogMux1(int k);
    int analogMux2(int k);
    int digitalMux1 (int k);
    int digitalMux2 (int k);
};


#ifdef usingmuxdig
multiplexer::initial_digMux() {
  pinMode(pin_Out_S0, OUTPUT);
  pinMode(pin_Out_S1, OUTPUT);
  pinMode(pin_Out_S2, OUTPUT);
  pinMode(enable_dig_one, OUTPUT);
  pinMode(enable_dig_two, OUTPUT);
  pinMode(pin_In_Mux_dig_one, INPUT);
  pinMode(pin_In_Mux_dig_two, INPUT);
  int Mux_State1[8] = {0};
  int Mux_State2[8] = {0};
}

int multiplexer::digitalMux1 (int k) {
  digitalWrite(enable_dig_one, LOW);
  digitalWrite(enable_dig_two, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(pin_Out_S0, HIGH && (i & B00000001));
    digitalWrite(pin_Out_S1, HIGH && (i & B00000010));
    digitalWrite(pin_Out_S2, HIGH && (i & B00000100));
    Mux_State1[i] = digitalRead(pin_In_Mux_dig_one);
  }
  return  Mux_State1[k];
}

int multiplexer::digitalMux2(int k) {
  digitalWrite(enable_dig_one, HIGH);
  digitalWrite(enable_dig_two, LOW);
  for (int i = 0; i < 8; i++) {
    digitalWrite(pin_Out_S0, HIGH && (i & B00000001));
    digitalWrite(pin_Out_S1, HIGH && (i & B00000010));
    digitalWrite(pin_Out_S2, HIGH && (i & B00000100));
    Mux_State2[i] = digitalRead(pin_In_Mux_dig_two);
  }
  return  Mux_State2[k];
}
#endif

#ifdef usingmuxana
multiplexer::initial_anaMux() {
  pinMode(pin_Out_S0, OUTPUT);
  pinMode(pin_Out_S1, OUTPUT);
  pinMode(pin_Out_S2, OUTPUT);
  pinMode(enable_ana_one, OUTPUT);
  pinMode(enable_ana_two, OUTPUT);
  int Mux_State3[8] = {0};
  int Mux_State4[8] = {0};
}

int multiplexer::analogMux1(int k) {
  digitalWrite(enable_ana_one, LOW);
  digitalWrite(enable_ana_two, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(pin_Out_S0, HIGH && (i & B00000001));
    digitalWrite(pin_Out_S1, HIGH && (i & B00000010));
    digitalWrite(pin_Out_S2, HIGH && (i & B00000100));
    Mux_State3[i] = analogRead(pin_In_Mux_ana_one);
  }
  return Mux_State3[k];
}
int multiplexer::analogMux2(int k) {
  digitalWrite(enable_ana_one, HIGH);
  digitalWrite(enable_ana_two, LOW);
  for (int i = 0; i < 8; i++) {
    digitalWrite(pin_Out_S0, HIGH && (i & B00000001));
    digitalWrite(pin_Out_S1, HIGH && (i & B00000010));
    digitalWrite(pin_Out_S2, HIGH && (i & B00000100));
    Mux_State4[i] = analogRead(pin_In_Mux_ana_two);
  }
  return Mux_State4[k];
}

#endif

#endif //usingmux4051

#ifdef usingmcp3208

/*#define SELPIN0 3 //Selection Pin
  #define SELPIN1 16 //Selection Pin
  #define DATAOUT 51//MOSI
  #define DATAIN  50//MISO
  #define SPICLOCK  52//Clock */
int readvalue;

#include <Arduino.h>
class multiplexer {
  public:
    initial();
    String updateMux1 (int k);
    String  updateMux2 (int k);

};



multiplexer::initial() {
  //set pin modes
  pinMode(SELPIN0, OUTPUT);
  pinMode(SELPIN1, OUTPUT);
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK, OUTPUT);
  //disable device to start with
  digitalWrite(SELPIN0, HIGH);
  digitalWrite(SELPIN1, HIGH);
  digitalWrite(DATAOUT, LOW);
  digitalWrite(SPICLOCK, LOW);

  int multiplexer::read_adc(int channel) {
    int adcvalue = 0;
    byte commandbits = B11000000; //command bits - start, mode, chn (3), dont care (3)

    //allow channel selection
    commandbits |= ((channel - 1) << 3);

    digitalWrite(SELPIN0, HIGH); //Select adc
    digitalWrite(SELPIN1, LOW); //Select adc
    // setup bits to be written
    for (int i = 7; i >= 3; i--) {
      digitalWrite(DATAOUT, commandbits & 1 << i);
      //cycle clock
      digitalWrite(SPICLOCK, HIGH);
      digitalWrite(SPICLOCK, LOW);
    }

    digitalWrite(SPICLOCK, HIGH);   //ignores 2 null bits
    digitalWrite(SPICLOCK, LOW);
    digitalWrite(SPICLOCK, HIGH);
    digitalWrite(SPICLOCK, LOW);

    //read bits from adc
    for (int i = 11; i >= 0; i--) {
      adcvalue += digitalRead(DATAIN) << i;
      //cycle clock
      digitalWrite(SPICLOCK, HIGH);
      digitalWrite(SPICLOCK, LOW);
    }
    digitalWrite(SELPIN1, HIGH); //turn off device
    return adcvalue;
  }


  int multiplexer::read_adc2 (int channel) {
    int adcvalue = 0;
    byte commandbits = B11000000; //command bits - start, mode, chn (3), dont care (3)

    //allow channel selection
    commandbits |= ((channel - 1) << 3);

    digitalWrite(SELPIN0, LOW); //Select adc
    digitalWrite(SELPIN1, HIGH); //Select adc
    // setup bits to be written
    for (int i = 7; i >= 3; i--) {
      digitalWrite(DATAOUT, commandbits & 1 << i);
      //cycle clock
      digitalWrite(SPICLOCK, HIGH);
      digitalWrite(SPICLOCK, LOW);
    }

    digitalWrite(SPICLOCK, HIGH);   //ignores 2 null bits
    digitalWrite(SPICLOCK, LOW);
    digitalWrite(SPICLOCK, HIGH);
    digitalWrite(SPICLOCK, LOW);

    //read bits from adc
    for (int i = 11; i >= 0; i--) {
      adcvalue += digitalRead(DATAIN) << i;
      //cycle clock
      digitalWrite(SPICLOCK, HIGH);
      digitalWrite(SPICLOCK, LOW);
    }
    digitalWrite(SELPIN0, HIGH); //turn off device
    return adcvalue;
  }
#endif
