#include <Arduino.h>
#ifdef MOJEC
int hdlcopen_size = 9;
int dlmsreqopen_size = 45;
int hdlcclose_size = 9;
int dlmsreqclose_size = 9;
int obiscmd_size = 27;
float current_multipler = 0.001;
float volt_multipler = 0.01;
float power_multipler = 0.1;
int dataloc_start = 16;
int dataloc_end = 19;
int meterloc = 18;
 char HDLC_REQ_OPEN[] = { 0x7E, 0xA0, 0x07, 0x03, 0x21, 0x93, 0x0F, 0x01, 0x7E };
 char DLMS_REQ_OPEN[] = { 0x7E, 0xA0, 0x2B, 0x03, 0x21, 0x10, 0xFB, 0xAF, 0xE6, 0xE6, 0x00, 0x60, 0x1D, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xBE, 0x10, 0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x04, 0xB0, 0xCA, 0xEA, 0x7E };
 char HDLC_REQ_CLOSE[] = { 0x7E, 0xA0, 0x11, 0x03, 0x21, 0xDC, 0xC7, 0x33, 0xE6, 0xE6, 0x00, 0x62, 0x03, 0x80, 0x01, 0x00, 0xBD, 0x9B, 0x7E };
 char DLMS_REQ_CLOSE[] = { 0x7E, 0xA0, 0x07, 0x03, 0x21, 0xF1, 0x1B, 0x41, 0x7E };
 char OBIS_commands[14][27] = {
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x32, 0x6F, 0xD8, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0xC3, 0xAD, 0x7E}, 
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x54, 0x5F, 0xDE, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x1F, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x09, 0xE6, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x76, 0x4F, 0xDC, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x33, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x9D, 0x52, 0x7E}, 
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x98, 0x3F, 0xD2, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x47, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x30, 0x87, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0xBA, 0x2F, 0xD0, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x20, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x50, 0x1C, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0xDC, 0x1F, 0xD6, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x34, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x4C, 0x4E, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0xFE, 0x0F, 0xD4, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x48, 0x07, 0x00, 0xFF, 0x02, 0x00, 0xB9, 0xBA, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x10, 0x7F, 0xDA, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x15, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x07, 0xCF, 0x7E}, 
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x32, 0x6F, 0xD8, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x16, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x7A, 0xC3, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x54, 0x5F, 0xDE, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x29, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x23, 0x39, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x76, 0x4F, 0xDC, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x2A, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x5E, 0x35, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x98, 0x3F, 0xD2, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x3D, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x3F, 0x6B, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0xBA, 0x2F, 0xD0, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x3E, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x42, 0x67, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0xBA, 0x2F, 0xD0, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x3E, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x42, 0x67, 0x7E}
};
#endif

#ifdef HUAWEI
int hdlcopen_size =37;
int dlmsreqopen_size =48;
int hdlcclose_size =22;
int dlmsreqclose_size =12;
int obiscmd_size = 30;
float current_multipler = 0.001;
float volt_multipler = 0.001;
float power_multipler = 0.000001;
int dataloc_start = 19;
int dataloc_end = 22;
int meterloc = 20;
char HDLC_REQ_OPEN[] = {0x7E, 0xA0, 0x07, 0x03, 0x21, 0x93, 0x0F, 0x51, 0x7E};
 char DLMS_REQ_OPEN[] = {0x7E, 0xA0, 0x2B, 0x03, 0x21, 0x10, 0xFB, 0xAF, 0xE6, 0xE6, 0x00, 0x60, 0x1D, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xBE, 0x10, 0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x04, 0xB0, 0xCA, 0xEA, 0x7E};
 char HDLC_REQ_CLOSE[] = {0x7E, 0xA0, 0x11, 0x03, 0x21, 0xDC, 0xC7, 0x33, 0xE6, 0xE6, 0x00, 0x62, 0x03, 0x80, 0x01, 0x00, 0xBD, 0x9B, 0x7E};
 char DLMS_REQ_CLOSE[] ={0x7E, 0xA0, 0x07, 0x03, 0x21, 0xF1, 0x1B, 0x41, 0x7E};
char OBIS_commands[14][30] = {
{0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x04, 0x9D, 0x21, 0x32, 0x2A, 0xC0, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x01, 0x00, 0x00, 0x60, 0x01, 0x00, 0xFF, 0x02, 0x00, 0x89, 0xA0, 0x7E},
{0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x04, 0x9D, 0x21, 0x54, 0x1A, 0xC6, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x00, 0x1F, 0x07, 0x00, 0xFF, 0x02, 0x00, 0xDC, 0x79, 0x7E},
{0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x04, 0x9D, 0x21, 0x76, 0x0A, 0xC4, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x00, 0x33, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x48, 0xCD, 0x7E},
{0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x04, 0x9D, 0x21, 0x98, 0x7A, 0xCA, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x00, 0x47, 0x07, 0x00, 0xFF, 0x02, 0x00, 0xE5, 0x18, 0x7E},
{0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x04, 0x9D, 0x21, 0xBA, 0x6A, 0xC8, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x00, 0x20, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x85, 0x83, 0x7E},
{0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x04, 0x9D, 0x21, 0xDC, 0x5A, 0xCE, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x00, 0x34, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x99, 0xD1, 0x7E},
{0x7E, 0xA0, 0x19, 0x03, 0x21, 0xFE, 0x0F, 0xD4, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x48, 0x07, 0x00, 0xFF, 0x02, 0x00, 0xB9, 0xBA, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x10, 0x7F, 0xDA, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x15, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x07, 0xCF, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x10, 0x7F, 0xDA, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x15, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x07, 0xCF, 0x7E},
{0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x04, 0x9D, 0x21, 0x76, 0x0A, 0xC4, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x00, 0x33, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x48, 0xCD, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x10, 0x7F, 0xDA, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x15, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x07, 0xCF, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x10, 0x7F, 0xDA, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x15, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x07, 0xCF, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x10, 0x7F, 0xDA, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x15, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x07, 0xCF, 0x7E},
  {0x7E, 0xA0, 0x19, 0x03, 0x21, 0x10, 0x7F, 0xDA, 0xE6, 0xE6, 0x00, 0xC0, 0x01, 0xC1, 0x00, 0x03, 0x01, 0x01, 0x15, 0x07, 0x00, 0xFF, 0x02, 0x00, 0x07, 0xCF, 0x7E},
};

#endif
