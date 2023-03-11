#pragma once
#define EEPROM_SIZE 26
#define MAX_TOTAL_LEDS 300

TaskHandle_t Task1; // task for core 1

const char* ssid = "get off its mine 2";
const char* password = "home123456";

int width;
int height;
framesize_t ImageResolution;
int Brightness;  
int Saturation; 
int Contrast; 
int Mode; 
int ISOValue;  
int Clock2x;
int pixDepth=8;

int webServerState;

//--------------- keystone points------------------
                  //                 (0,0)  -------> +X
int X1=0,Y1=0;    // point 1           |  P1----------------------------P2
                  //                   |  |                             |
int X2=240,Y2=0;  // point 2          \|/ |                             |
                  //                   +y |                             |
int X3=240,Y3=240;// point 3              |                             |
                  //                      |                             |
int X4=0,Y4=240;  // point 4              P4----------------------------P3

//--------------------------------------------------

int max_loop_distance=0;
int P1P2_dis=0;
int P2P3_dis=0;
int P4P3_dis=0;
int P1P4_dis=0;

//// Array of Size [EEPROM][2][2] dyanmaic
//int ***P1P2;     //width
//int ***P2P3;     //height
//int ***P4P3;     //width
//int ***P1P4;     //height

int P1P2[240][8][2];     //width
int P2P3[240][8][2];     //height
int P4P3[240][8][2];     //width
int P1P4[240][8][2];     //height


//---------------------core-1 variables-----------------------------------------

//------------Led counts------------------

int max_1D_led_count=86;
int P1P2_count=86;
int P2P3_count=47;
int P4P3_count=86;
int P1P4_count=47;

//----------------------------------------

//---------multpler factor----------------

float P1P2_fact=0;
float P2P3_fact=0;
float P4P3_fact=0;
float P1P4_fact=0;

//---------------------------------------

typedef struct RGB_avg{
   uint32_t R;
   uint32_t G;
   uint32_t B;
}RGB_avg;

RGB_avg RGB_1;
//----------------------------------------




//--------------MAC Address---------

uint8_t SlaveAddress[]={0xEC,0xFA,0xBC,0xBC,0x9F,0x18};
//uint8_t SlaveAddress[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
//----------------------------------

bool ESPNowMsgSent=true;

typedef struct Led_RGB{
  uint8_t Led_R[MAX_TOTAL_LEDS];
  uint8_t Led_G[MAX_TOTAL_LEDS];
  uint8_t Led_B[MAX_TOTAL_LEDS];
  uint16_t Actaully_total_length;
  uint16_t Remaing_counts;
}Led_RGB;

Led_RGB LED;

typedef struct struct_message{
  uint8_t Led_R[82];
  uint8_t Led_G[82];
  uint8_t Led_B[82];
  uint16_t start_pos;
  uint16_t len;    
  
}struct_message;

struct_message dataPacket;

esp_now_peer_info_t peerInfo;

bool debugFlag=true;
int previous_Frame_Time=millis();;
unsigned int Frame_count=0;

const char* JPG_CONTENT_TYPE = "image/jpeg";
uint8_t * out=NULL;
size_t out_len;

bool captureFlag= true;
AsyncWebServer server(80);
