void setUpGSM() {
  String response;
  while (true) {
    wdt_reset();
    response = "";
    GSM.println(F("AT"));
    response = GSM_Data();
    if (response.indexOf(F("OK")) != -1) {
      wdt_reset();
      Debug(F("GSM Found!\n"));
      response = "";
      GSM.println(F("AT+CPIN?"));
      delay(1000);
      response = GSM_Data();
      Debug(response + "\n");
      if (response.indexOf(F("READY")) != -1) {
        GSM.println(F("AT+CSQ"));
        GSM_Data();
        GSM.println(F("AT+CREG?"));
        GSM_Data();
        errorState = false;
        break;
      }
    }
  }
}//End void setUpGSM()
void setGSMHTTP(String networkAPN) {
  wdt_reset();
  GSM.println("AT+CGATT?"); //Attach or Detach from GPRS Support
  delay(200);
  GSM.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
  delay(200);
  GSM.println("AT+SAPBR=3,1,\"APN\"," + networkAPN); //setting the APN, Access point name string
  delay(200);
  wdt_reset();
  GSM.println("AT+SAPBR=1,1");//setting the SAPBR
  delay(200);
  GSM.println("AT+CSQ"); // Signal quality check
  delay(500);
  GSM.println("AT+HTTPTERM"); //init the HTTP request
  delay(500);
  GSM.println("AT+HTTPINIT"); //init the HTTP request
}

int IDSIM() {
  String response = "";
  unsigned int net = 4;
  Debug(F("Checking SIM Network!\n"));
  if (!errorState) {
    errorState = true;
    while (true) {
      wdt_reset();
      GSM.println(F("AT+COPS?"));
      response = GSM_Data();
      Debug(response);
      if (response.indexOf("Glo") != -1)
        net = 0;
      else if (response.indexOf("MTN") != -1)
        net = 1;
      else if (response.indexOf("Airtel") != -1 || response.indexOf("Econet") != -1)
        net = 2;
      else if (response.indexOf("9mobile") != -1 || response.indexOf("62160") != -1)
        net = 3;
      else {
        net = 4;
        Debug(F("Network Not defined:  Using default APN\n"));
      }
      if (net <= 4) {
        errorState = false;
        break;
      }

    }
  } else {
    Debug(F("Error state is active!\n"));
  }
  return net;
}//End int IDSIM()

String GSM_Data() {
  String _data;
  unsigned long GSM_data_timeout = millis();
  while (GSM.available() == 0)
    if (millis() > (GSM_data_timeout + 1000)) break;
  delay(50);
  while (GSM.available()) {
    char c = GSM.read();
    _data += c;
  }
  return _data;
}//End String GSM_Data()

bool attachInternet() {
  wdt_reset();
  String response;
  bool state = false;
  unsigned int simID = IDSIM();
  Debug("\n" + (String)simID);
  simAPN = simID;
  if (simID <= 4) {
    char buffer[75];
    strcpy_P(buffer, (char*)pgm_read_word(&(APN[simID])));
    Debug(F("APN: "));
    Debug((String)buffer);
    Debug(F("\n"));
    Debug(F("simID: "));
    Debug((String)simID);
    Debug(F("\n"));
    networkAPN = (String)buffer;
    GSM.println(F("AT+CGATT?"));
    GSM_Data();
    GSM.println(F("AT+CSTT?"));
    response = GSM_Data();
    if (response.indexOf(buffer) < 0) {
      String apn = F("AT+CSTT=\"");
      apn += (String)buffer;
      apn += F("\"");
      apn += F(",\"\",\"\"");
      GSM.println(apn);
    }
    GSM_Data();
    GSM.println(F("AT+CIICR"));
    GSM_Data();
    GSM.println(F("AT+CIFSR"));
    response = "";
    response = GSM_Data();
    if (response.indexOf(F("10.")) != -1) {
      wdt_reset();
      GSM_Data();
      GSM.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
      delay(500);
      GSM_Data();
      GSM.println("AT+SAPBR=3,1,\"APN\"," + networkAPN);//setting the APN, Access point name string
      delay(500);
      wdt_reset();
      GSM_Data();
      GSM.println("AT+SAPBR=1,1");//setting the SAPBR
      delay(500);
      GSM_Data();
      state = true;
    }

  } else {
    Debug(F("Unknown Network, Attach Internet failed!\n"));
  }
  return state;
}//End bool attachInternet()

void Debug(String msg) {
  if (isDebug) {
    Serial.print(msg);
  }
}//End Debug()

//Function to send IoT data to server
String SendData(String url) {
  wdt_reset();
  String response;
  Debug(url + "\n");
  Debug(F("opening server request!\n"));
  GSM.println(url);
  delay(200);
  GSM.println("AT+HTTPACTION=0");
  wdt_reset();
  delay(1000);
  GSM_Data();
  wdt_reset();
  GSM.println("AT+HTTPREAD");// read the data from the website you access
  delay(1500);
  wdt_reset();
  response = GSM_Data();
  GSM.println("  ");
  GSM.flush();
  return response;
}

void getGPSData(String command , const int timeout , boolean debug) {
  wdt_reset();
  Serial.println("Getting GPS Info.");
  GSM.println(command);
  String lastestLat;
  String lastestLng;
  gpsState = "";
  timegps  = "";
  long int time = millis();
  int i = 0;

  while ((time + timeout) > millis()) {
    wdt_reset();
    while (GSM.available()) {
      char c = GSM.read();
      if (c != ',') {
        data[i] += c;
        delay(50);
      } else {
        i++;
      }
      if (i == 5) {
        delay(50);
        goto exitL;
      }
    }
} exitL:
  if (debug) {
    gpsState = data[1];
    timegps = data[2];
    lastestLat = data[3];
    lastestLng = data[4];
  }
  Serial.println("State  :" + gpsState);
  Serial.println("Time  :" + timegps);
  Serial.println("latitude  :" + lastestLat);
  Serial.println("Longitude  :" + lastestLng);
  if (gpsState == "1") {
    latitude = lastestLat;
    longitude = lastestLng;
    isLocateSet = true;
  }
}

String sendData(String command, const int timeout)
{
  String response = "";
  GSM.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    wdt_reset();
    while (GSM.available())
    {
      char c = GSM.read();
      response += c;
    }
  }
  Debug(response);
  return response;
}
// digital input function to collect number of avaiable channel and used channel

String  digitalSignal(int usedChannels, int totalChannels) {
  String paramStr = "&d=";
  for (int i = 0; i < totalChannels; i ++) {
    wdt_reset();
    int signalSample[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (i < 8) {
      //      Serial.print("Sampling signal ");
      //      Serial.println(i);
      for (int j = 0; j < 20; j++) {
        wdt_reset();
        signalSample[j] = mux.digitalMux1(i);
        // Serial.println(mux.digitalMux1(i));
        delay(10);
        wdt_reset();
      }
      for (int m = 0; m < 20; m++) {
        wdt_reset();
        if (signalSample[m] == 1) {
          digital_Array[i] = signalSample[m];
          break;
        } else {
          digital_Array[i] = 0;
        }
      }
    }
    else if (i >= 8 && i < totalChannels ) {
      for (int j = 0; j < 20; j++) {
        wdt_reset();
        signalSample[j] = mux.digitalMux2(i - 8);
        delay(5);
      }
      for (int m = 0; m < 20; m++) {
        wdt_reset();
        if (signalSample[m] == 1) {
          digital_Array[i] = signalSample[m];
          break;
        } else {
          digital_Array[i] = 0;
        }
      }
    }
  }
  for (int i = 0; i < totalChannels; i ++) {
    int sensorVal; float av_val;
    if (i < usedChannels) {
      int buttonState = 0;
      buttonState = digital_Array[i];
      paramStr += String(buttonState);
      paramStr += ">";
    } else {
      if (i == (totalChannels - 1)) {
        paramStr += "R";
      }
      else if (i < (totalChannels - 1)) {
        paramStr += "R";
        paramStr += ">";
      }

    }
  }
  return paramStr;
}

String analogSignal(int usedChannels, int totalChannels) {
  String paramStr = "&a=";
  for (int i = 0; i < totalChannels; i++) {
    wdt_reset();
    if (i < usedChannels) {
      if (i == 0) { // environ temperature read
        sensors.requestTemperatures();
        double celsiusevn = sensors.getTempCByIndex(0);
        // Debug((String)celsiusevn + "\n");
        if (isnan(celsiusevn) || celsiusevn < 10 ) {
          celsiusevn = 25;
        }
        paramStr += (String)celsiusevn;
        paramStr += ">";
      } else if (i == 1) { // oil temperature
        double celsius = thermocouple.readCelsius();
        if (isnan(celsius) || celsius < 0 || celsius > 200) {
          celsius = 0.0;
        }

        paramStr += (String)celsius;
        paramStr += ">";
      }
      else if (i == 2) { // oil level
        float sensorValueraw = mux.analogMux1(i - 2);
        if (sensorValueraw < 0) {
          sensorValueraw = 0.0;
        }
        int  sensorValue = map(sensorValueraw, 180, 800, 0, 100);
        paramStr += (String)sensorValue;
        paramStr += ">";
      } else {
        if (RS485_Data_Array[i - 3] < 1) {
          paramStr += "0";
        } else {
          paramStr += (String)RS485_Data_Array[i - 3];
        }

        paramStr += ">";
      }
    } else {
      if (i < totalChannels - 1) {
        paramStr += "R";
        paramStr += ">";
      }
      else if (i == totalChannels - 1) {
        paramStr += "R";
      }
    }
  }
  return paramStr;
}

String  feedbackSignalfromAmux() {
  String paramStr = "&z=";
  for (int i = 0; i < 8; i ++) {
    int fbState = 0;
    wdt_reset();
    fbState =  mux.analogMux2(i);
    paramStr += String(fbState);
    if (i < 7) {
      paramStr += ">";
    }

  }
  return paramStr;
}
void blinkNETLED() {
  int blinkInterval = 150;
  if (millis() > (LEDtimer + blinkInterval)) {
    LEDtimer = millis();
    digitalWrite(NETLED, !digitalRead(NETLED));
  }
}//End void blinkNETLED()


String linkGenerate() {
  wdt_reset();
  String address = "";
  address.concat(ser);
  address.concat(digitalSignal(DigitalChannel, 13));
  address.concat(analogSignal(AnalogChannel, 13));
  address.concat(feedbackSignalfromAmux());
  address.concat("&l=");
  address.concat(latitude);
  address.concat(">");
  address.concat(longitude);
  for (int j = 0; j < 5; j++) {
    data[j] = "";
  }
  return address;
}
void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");
}


String readSms() {
  String ctrlText;
  smscount++;
  wdt_reset();
  String response = sendData("AT+CMGR=" + String(smscount), 500); //read message at index i
  if (response.indexOf("+CMGR:") != -1) {
    if (response.indexOf("{") != -1) {
      ctrlText = response.substring(response.indexOf(F("{")), response.indexOf(F("}")) + 1);//extracts text frm JSON in the sms
      Serial.println(ctrlText);
    }
    sendData("AT+CMGD=" + String(smscount), 50);
    wdt_reset();
  }
  smscount = 0;
  return ctrlText;
}



void getRS485Res(int k) {
  wdt_reset();
  delay(50);
  int l = 0;
  digitalWrite(RS485_control, LOW);
  if (RS485.available()) {
    Serial.println();
    while (RS485.available() > 0) {
      String rs485Res = String(RS485.read(), HEX);
      wdt_reset();
      if (rs485Res.length() == 1) {
        rs485Res = "0" + rs485Res;
      }
      if (k > 0) {
        Serial.print(rs485Res); Serial.print(" ");
        rs485Data[l] = rs485Res;
      }
      l++;

    }
  }
  RS485.flush();
  Serial.println();
  if (k == 1) {
    Serial.println("Meter ID No: ");
    for (int k = 0; k < 12; k++) {
      wdt_reset();
      String wh_volt =  rs485Data[meterloc + k];
      int str_len = wh_volt.length() + 1;
      char char_array[str_len];
      wh_volt.toCharArray(char_array, str_len);
      long decimal_answer = strtol(char_array, NULL, 16) - 48;
      Serial.print(decimal_answer);  Serial.print(" ");
    }
  }
  if (k == 2) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * current_multipler;
    if (total_val > 0) {
      RS485_Data_Array[3] = total_val;
    }

    Serial.println("Total Current A");
    Serial.print( RS485_Data_Array[3]); Serial.println();
  }
  if (k == 3) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * current_multipler;
    if (total_val > 0) {
      RS485_Data_Array[4] = total_val;
    }
    Serial.println("Total Current B");
    Serial.print(  RS485_Data_Array[4]); Serial.println();
  }
  if (k == 4) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * current_multipler;
    if (total_val > 0) {
      RS485_Data_Array[5] = total_val;
    }
    Serial.println("Total Current C");
    Serial.print( RS485_Data_Array[5]); Serial.println();

  }
  if (k == 5) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * volt_multipler;
    if (total_val > 0) {
      RS485_Data_Array[0] = total_val;
    }
    Serial.println("Total Voltage A");
    Serial.print(  RS485_Data_Array[0]); Serial.println();
  }
  if (k == 6) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * volt_multipler;
    if (total_val > 0) {
      RS485_Data_Array[1] = total_val;
    }
    Serial.println("Total Voltage B");
    Serial.print(  RS485_Data_Array[1]); Serial.println();
  }
  if (k == 7) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * volt_multipler;
    if (total_val > 0) {
      RS485_Data_Array[2] = total_val;
    }
    Serial.println("Total Voltage C");
    Serial.print(RS485_Data_Array[2] ); Serial.println();
  }
  if (k == 8) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * power_multipler;
    RS485_Data_Array[6] = total_val;
    Serial.println("Total Power A imp");
    Serial.print(total_val); Serial.println();
  }
  if (k == 9) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * power_multipler;
    RS485_Data_Array[7] = total_val;
    Serial.println("Total Power A exp");
    Serial.print(total_val); Serial.println();
  }
  if (k == 10) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * power_multipler;
    RS485_Data_Array[8] = total_val;
    Serial.println("Total Power B imp");
    Serial.print(total_val); Serial.println();
  }
  if (k == 11) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * power_multipler;
    RS485_Data_Array[9] = total_val;
    Serial.println("Total Power B exp");
    Serial.print(total_val); Serial.println();
  }
  if (k == 12) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * power_multipler;
    RS485_Data_Array[10] = total_val;
    Serial.println("Total Power C imp");
    Serial.print(total_val); Serial.println();
  }
  if (k == 13) {
    String wh_volt =  rs485Data[dataloc_start] + rs485Data[dataloc_start + 1] + rs485Data[dataloc_start + 2] + rs485Data[dataloc_start + 3];
    Serial.println(wh_volt);
    int str_len = wh_volt.length() + 1;
    char char_array[str_len];
    wh_volt.toCharArray(char_array, str_len);
    long decimal_answer = strtol(char_array, NULL, 16);
    float total_val = decimal_answer * power_multipler;
    RS485_Data_Array[11] = total_val;
    Serial.println("Total Power C ex");
    Serial.print(total_val); Serial.println();
  }
  Serial.println();
  RS485.flush();
  Serial.flush();

}
void getRS485Data() {
  Serial.println("Taking Meter Data");
  digitalWrite(RS485_control, HIGH);
  Serial.println();
  for (int i = 0; i < hdlcopen_size; i++) {
    wdt_reset();
    char cmd = (HDLC_REQ_OPEN[i]);//please check "hdlcopen_size" i find something wrong we(over indexing)
    RS485.write(cmd);
  }
  delay(300);
  digitalWrite(RS485_control, HIGH);
  for (int i = 0; i < dlmsreqopen_size; i++) {
    wdt_reset();
    char cmd = (DLMS_REQ_OPEN[i]);
    RS485.write(cmd);
  }
  delay(300);
  wdt_reset();
  for (int j = 0; j < 9; j++) {
    digitalWrite(RS485_control, HIGH);
    for (int i = 0; i < obiscmd_size; i++) {
      wdt_reset();
      char cmd = (OBIS_commands[j][i]);
      RS485.write(cmd);
    }
    Serial.println();
    getRS485Res(j);
    delay(300);
    wdt_reset();
  }
  digitalWrite(RS485_control, HIGH);
  for (int i = 0; i < hdlcclose_size; i++) {
    wdt_reset();
    char cmd = (DLMS_REQ_CLOSE[i]);
    RS485.write(cmd);
  }
  delay(300);
  wdt_reset();
  digitalWrite(RS485_control, HIGH);
  for (int i = 0; i < dlmsreqclose_size; i++) {
    wdt_reset();
    char cmd = (HDLC_REQ_CLOSE[i]);
    RS485.write(cmd);
  }
  Serial.println();
  RS485.flush();
  Serial.flush();
  delay(300);
  wdt_reset();
}
