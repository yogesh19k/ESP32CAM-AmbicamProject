

#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <math.h>
//For OTA when in server mode
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//---------------------------

#include "indexPage.h"
#include "esp_now.h"
#include "variables.h"'
#include "testimage.h"



// for full dynamic allocation
//void allocate3DArrays(int ****arrayName,int Max_length){
//  *arrayName = (int***)malloc(Max_length * sizeof(int**));
//  for (int i = 0; i < Max_length; i++) {
//      (*arrayName)[i] = (int**)malloc(pixDepth * sizeof(int*));
//      for (int j = 0; j < pixDepth; j++) {
//          (*arrayName)[i][j] = (int*)malloc(2 * sizeof(int));
//      }
//  }
//}

//2 level dynamic allocation
// void allocate3DArrays(int** (*arrayName)[240],int Max_length){  // 
//   for (int i = 0; i < Max_length; i++) {
//       (*arrayName)[i] = (int**)malloc(2 * sizeof(int*));
//       for (int j = 0; j < 2; j++) {
//          (*arrayName)[i][j] = (int*)malloc(2 * sizeof(int));
//       }
//   }
// }

//1 level dynamic allocation
//void allocate3DArrays(int* (*arrayName)[240][2],int Max_length){
//  for (int i = 0; i < Max_length; i++) {
//      for (int j = 0; j < 2; j++) {
//         (*arrayName)[i][j] = (int*)malloc(8 * sizeof(int));
//      }
//  }
//}



//void allocMemeForVar(){
//  allocate3DArrays(&P1P2,width);
//  allocate3DArrays(&P2P3,height);
//  allocate3DArrays(&P4P3,width);
//  allocate3DArrays(&P1P4,height);
//  
//}

void writeIntIntoEEPROM(int Address, int number)
{ 
  if(EEPROM.read(Address) != (number >> 8))
     EEPROM.write(Address, number >> 8);
  if(EEPROM.read(Address + 1) != (number & 0xFF))
     EEPROM.write(Address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
    byte byte1 = EEPROM.read(address);
    byte byte2 = EEPROM.read(address + 1);
    return (byte1 << 8) + byte2;
}
void ReadSettingsFromEEPROM(){
  // wirte all settigs which required 2 bytes
  EEPROM.begin(EEPROM_SIZE);
  ISOValue = readIntFromEEPROM(0);
  X1 = readIntFromEEPROM(2);
  Y1 = readIntFromEEPROM(4);
  X2 = readIntFromEEPROM(6);
  Y2 = readIntFromEEPROM(8);
  X3 = readIntFromEEPROM(10);
  Y3 = readIntFromEEPROM(12);
  X4 = readIntFromEEPROM(14);
  Y4 = readIntFromEEPROM(16);
  
  // 1 byte settings
  ImageResolution = (framesize_t)EEPROM.read(20);
  Brightness      =  EEPROM.read(21)-2;
  Saturation      =  EEPROM.read(22)-2;
  Contrast        =  EEPROM.read(23)-2;   
  Mode            =  EEPROM.read(24);   
  Clock2x         =  EEPROM.read(25);   
  webServerState=EEPROM.read(18);
}


void WriteSettingsToEEPROM(){
  // wirte all settigs which required 2 bytes

  writeIntIntoEEPROM(0, ISOValue);
  writeIntIntoEEPROM(2, X1);
  writeIntIntoEEPROM(4, Y1);
  writeIntIntoEEPROM(6, X2);
  writeIntIntoEEPROM(8, Y2);
  writeIntIntoEEPROM(10, X3);
  writeIntIntoEEPROM(12, Y3);
  writeIntIntoEEPROM(14, X4);
  writeIntIntoEEPROM(16, Y4);
  
  // 1 byte settings
  if(EEPROM.read(20) != String(ImageResolution).toInt())
      EEPROM.write(20, String(ImageResolution).toInt());
  if(EEPROM.read(21) != (Brightness+2))
      EEPROM.write(21, Brightness+2);
  if(EEPROM.read(22) != (Saturation+2))
      EEPROM.write(22, Saturation+2);
  if(EEPROM.read(23) != (Contrast+2))
      EEPROM.write(23, Contrast+2);
  if(EEPROM.read(24) != Mode)
      EEPROM.write(24, Mode);
  if(EEPROM.read(25) != Clock2x)
      EEPROM.write(25, Clock2x);  

}

void getWidthHightFromResolution(){
  switch(String(ImageResolution).toInt()){
    case 0:
          width  = 96;
          height = 96;
          break;
    case 1:
          width  = 160;
          height = 120;
          break;
    case 2:
          width  = 176;
          height = 144;
          break;
    case 3:
          width  = 240;
          height = 176;
          break;
    case 4:
          width  = 240;
          height = 240;
          break;
    case 5:
          width  = 320;
          height = 240;
          break;
    case 6:
          width  = 400;
          height = 296;
          break;
    case 7:
          width  = 480;
          height = 320;
          break;
    case 8:
          width  = 640;
          height = 480;
          break;
  }
  
}





void frame_buff_debug(){
//  if (flag1){
//    Serial.printf("Buffer Size: \nbuffer height: %d \nbuffer width: %d \nbuffer len: %d",
//                 frame_buffer->height,
//                 frame_buffer->width,
//                 frame_buffer->len);
//    flag1=false;
//    }
//    if (count%60 == 0){
//      Serial.printf("\nFPS :%f", 60000.0/(millis()-previous_Frame_Time));
//      previous_Frame_Time=millis(); 
//    }
//    count++;

}

float lineEquation(int XY,int X1,int X2,int Y1,int Y2,bool X_direc){
  float slope=(float)(Y2-Y1)/(X2-X1);
  if (X_direc)
    return (slope*(XY-X1))+Y1;
  else
    return ((XY-Y1)/slope)+X1;
}

void pre_process(const unsigned char *image){

  for(int i=0;i<max_loop_distance;i++){
    for(int k=0;k<pixDepth;k++){ 
        if(i<P1P2_dis){
          int T1=ceil(lineEquation(X1+i,X1,X2,Y1,Y2,true));       //int tx=lineEquation(X1+i,X1,X2,Y1,Y2,true);
          P1P2[i][k][0]=image[(((T1+k)*width)+X1+i)*2];           //P1P2[i][0][0]=image[((T1*width)+X1+i)*2];
          P1P2[i][k][1]=image[(((T1+k)*width)+X1+i)*2+1];         //P1P2[i][0][1]=image[((T1*width)+X1+i)*2+1];
    //      Serial.printf(" %d-%d ",X1+i,T1);
    //      Serial.printf("Pos of %d 0 : %d = %x, Pos of %d 1 : %d = %x\n",i,((T1*width)+X1+i)*2,image[((T1*width)+X1+i)*2],i,((T1*width)+X1+i)*2+1,image[((T1*width)+X1+i)*2+1]);
        }
        if(i<P4P3_dis){
          int T2=floor(lineEquation(X4+i,X4,X3,Y4,Y3,true));
          P4P3[i][k][0]=image[(((T2-k)*width)+X4+i)*2];           //P4P3[i][0][0]=image[((T2*width)+X4+i)*2];
          P4P3[i][k][1]=image[(((T2-k)*width)+X4+i)*2+1];         //P4P3[i][0][1]=image[((T2*width)+X4+i)*2+1];
    //      Serial.printf(" %d-%d",X4+i,T2);
        }
        if(i<P2P3_dis){
          int T3=floor(lineEquation(Y2+i,X2,X3,Y2,Y3,false));
          P2P3[i][k][0]=image[(((Y2+i)*width)+T3-k)*2];           //P2P3[i][0][0]=image[(((Y2+i)*width)+T3)*2];
          P2P3[i][k][1]=image[(((Y2+i)*width)+T3-k)*2+1];         //P2P3[i][0][1]=image[(((Y2+i)*width)+T3)*2+1];
    //      Serial.printf(" %d-%d",T3,Y2+i);
        }
        if(i<P1P4_dis){
          int T4=ceil(lineEquation(Y1+i,X1,X4,Y1,Y4,false));
          P1P4[i][k][0]=image[(((Y1+i)*width)+T4+k)*2];           //P1P4[i][0][0]=image[(((Y1+i)*width)+T4)*2];
          P1P4[i][k][1]=image[(((Y1+i)*width)+T4+k)*2+1];         //P1P4[i][0][1]=image[(((Y1+i)*width)+T4)*2+1];
//          Serial.printf(" %d: %x ",i,P1P4[i][k][0]);
        }
     }
//  flag1=false;
  }
}

void keystone_setup(){
//  max_loop_distance= (X2-X1) > (X3-X4) ? (X2-X1):(X3-X4);

  
  P1P2_dis=X2-X1;
  P2P3_dis=Y3-Y2;
  P4P3_dis=X3-X4;
  P1P4_dis=Y4-Y1;
  max_loop_distance= (P1P2_dis > P2P3_dis && P1P2_dis > P4P3_dis && P1P2_dis > P1P4_dis) ?
               P1P2_dis : ((P2P3_dis > P4P3_dis && P2P3_dis > P1P4_dis) ?
               P2P3_dis : (P4P3_dis > P1P4_dis ? P4P3_dis : P1P4_dis));
  Serial.printf("P1: %d,%d\nP2: %d,%d\nP3: %d,%d\nP4: %d,%d\nP1P2 Dis: %d\nP2P3 Dis: %d\nP4P3 Dis: %d\nP1P4 Dis: %d\nMax loop Distance: {%d}"
                ,X1,Y1,X2,Y2,X3,Y3,X4,Y4,P1P2_dis,P2P3_dis,P4P3_dis,P1P4_dis,max_loop_distance);
  
}



//----------------------------------------------------------------------------------------------------------
//Web server related class and funcations  ; Last chnage: Added class and function to get grb565 stream page
//----------------------------------------------------------------------------------------------------------
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* STREAM_PART = "Content-Type: %s\r\nContent-Length: %u\r\n\r\n";
typedef struct {
        camera_fb_t * fb;
        size_t index;
} camera_frame_t;

class AsyncJpegStreamResponse: public AsyncAbstractResponse {
    private:
        camera_frame_t _frame;
        size_t _index;
        size_t _jpg_buf_len;
        uint8_t * _jpg_buf;
        uint64_t lastAsyncRequest;
    public:
        AsyncJpegStreamResponse(){
            _callback = nullptr;
            _code = 200;
            _contentLength = 0;
            _contentType = STREAM_CONTENT_TYPE;
            _sendContentLength = false;
            _chunked = true;
            _index = 0;
            _jpg_buf_len = 0;
            _jpg_buf = NULL;
            lastAsyncRequest = 0;
            memset(&_frame, 0, sizeof(camera_frame_t));
        }
        ~AsyncJpegStreamResponse(){
            if(_frame.fb){
                if(_frame.fb->format != PIXFORMAT_JPEG){
                    free(_jpg_buf);
                }
                esp_camera_fb_return(_frame.fb);
            }
        }
        bool _sourceValid() const {
            return true;
        }
        virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override {
            size_t ret = _content(buf, maxLen, _index);
            if(ret != RESPONSE_TRY_AGAIN){
                _index += ret;
            }
            return ret;
        }
        size_t _content(uint8_t *buffer, size_t maxLen, size_t index){
            if(!_frame.fb || _frame.index == _jpg_buf_len){
                if(index && _frame.fb){
                    uint64_t end = (uint64_t)micros();
                    int fp = (end - lastAsyncRequest) / 1000;
                    log_printf("Size: %uKB, Time: %ums (%.1ffps)\n", _jpg_buf_len/1024, fp);
                    lastAsyncRequest = end;
                    if(_frame.fb->format != PIXFORMAT_JPEG){
                        free(_jpg_buf);
                    }
                    esp_camera_fb_return(_frame.fb);
                    _frame.fb = NULL;
                    _jpg_buf_len = 0;
                    _jpg_buf = NULL;
                }
                if(maxLen < (strlen(STREAM_BOUNDARY) + strlen(STREAM_PART) + strlen(JPG_CONTENT_TYPE) + 8)){
                    //log_w("Not enough space for headers");
                    return RESPONSE_TRY_AGAIN;
                }
                //get frame
                _frame.index = 0;

                _frame.fb = esp_camera_fb_get();
                if (_frame.fb == NULL) {
                    log_e("Camera frame failed");
                    return 0;
                }

                if(_frame.fb->format != PIXFORMAT_JPEG){
                    unsigned long st = millis();
                    bool jpeg_converted = frame2jpg(_frame.fb, 80, &_jpg_buf, &_jpg_buf_len);
                    if(!jpeg_converted){
                        log_e("JPEG compression failed");
                        esp_camera_fb_return(_frame.fb);
                        _frame.fb = NULL;
                        _jpg_buf_len = 0;
                        _jpg_buf = NULL;
                        return 0;
                    }
                    log_i("JPEG: %lums, %uB", millis() - st, _jpg_buf_len);
                } else {
                    _jpg_buf_len = _frame.fb->len;
                    _jpg_buf = _frame.fb->buf;
                }

                //send boundary
                size_t blen = 0;
                if(index){
                    blen = strlen(STREAM_BOUNDARY);
                    memcpy(buffer, STREAM_BOUNDARY, blen);
                    buffer += blen;
                }
                //send header
                size_t hlen = sprintf((char *)buffer, STREAM_PART, JPG_CONTENT_TYPE, _jpg_buf_len);
                buffer += hlen;
                //send frame
                hlen = maxLen - hlen - blen;
                if(hlen > _jpg_buf_len){
                    maxLen -= hlen - _jpg_buf_len;
                    hlen = _jpg_buf_len;
                }
                memcpy(buffer, _jpg_buf, hlen);
                _frame.index += hlen;
                return maxLen;
            }

            size_t available = _jpg_buf_len - _frame.index;
            if(maxLen > available){
                maxLen = available;
            }
            memcpy(buffer, _jpg_buf+_frame.index, maxLen);
            _frame.index += maxLen;

            return maxLen;
        }
};

void getKeystonePointCords(){
  X1=0;
  Y1=0;  
  X2=width;  
  Y2=0;  
  X3=width;  
  Y3=height; 
  X4=0;
  Y4=height; 
}
void SendVraiblesToWeb(AsyncWebServerRequest* request){
    String json = "{\"width\":\"" + String(width) + 
                     "\",\"height\":\"" + String(height) +
                     "\",\"X1\":\"" + String(X1) + 
                     "\",\"Y1\":\"" + String(Y1) +  
                     "\",\"X2\":\"" + String(X2) +  
                     "\",\"Y2\":\"" + String(Y2) +  
                     "\",\"X3\":\"" + String(X3) +  
                     "\",\"Y3\":\"" + String(Y3) + 
                     "\",\"X4\":\"" + String(X4) +
                     "\",\"Y4\":\"" + String(Y4) + 
                     "\",\"ImageResolution\":\"" + String(ImageResolution) + 
                     "\",\"Brightness\":\"" + String(Brightness) +  
                     "\",\"Saturation\":\"" + String(Saturation) + 
                     "\",\"Contrast\":\"" + String(Contrast) + 
                     "\",\"Mode\":\"" + String(Mode) + 
                     "\",\"ISOValue\":\"" + String(ISOValue) +  
                     "\",\"Clock2x\":\"" + String(Clock2x) +"\"}";
                     
    request->send(200, "application/json", json);

}

void SendDeafultVarToWeb(AsyncWebServerRequest* request){
    String json = "{\"width\":\"" + String(240) + 
                     "\",\"height\":\"" + String(240) +
                     "\",\"X1\":\"" + String(0) + 
                     "\",\"Y1\":\"" + String(0) +  
                     "\",\"X2\":\"" + String(240) +  
                     "\",\"Y2\":\"" + String(0) +  
                     "\",\"X3\":\"" + String(240) +  
                     "\",\"Y3\":\"" + String(240) + 
                     "\",\"X4\":\"" + String(0) +
                     "\",\"Y4\":\"" + String(240) + 
                     "\",\"ImageResolution\":\"" + "4" + 
                     "\",\"Brightness\":\"" + String(-2) +  
                     "\",\"Saturation\":\"" + String(0) + 
                     "\",\"Contrast\":\"" + String(2) + 
                     "\",\"Mode\":\"" + String(0) + 
                     "\",\"ISOValue\":\"" + String(145) +  
                     "\",\"Clock2x\":\"" + String(1) +"\"}";
    request->send(200, "application/json", json);
    //-------------Default Settings-----------------
    if(String(ImageResolution)!="4"){
      width  = 240;
      height = 240;
      X1=0;
      Y1=0;  
      X2=240;  
      Y2=0;  
      X3=240;  
      Y3=240; 
      X4=0;
      Y4=240; 
    }
    ImageResolution=(framesize_t)4;
    Brightness=-2;  
    Saturation=0; 
    Contrast=2; 
    Mode=0; 
    ISOValue=145;  
    Clock2x=1;
    //--------------------------------------------

    
    
}


void GetVraiblesFromeWeb(AsyncWebServerRequest* request){
    if(request->getParam("request")->value()=="imageSettings"){
      String Settings_name=request->getParam("setting")->value();
      if(Settings_name=="Image-resolution"){
        ImageResolution=(framesize_t)(request->getParam("value")->value().toInt());
        getWidthHightFromResolution();
        getKeystonePointCords();
        WriteSettingsToEEPROM();
        EEPROM.commit();   
        ESP.restart();
      }
      if(Settings_name=="Brightness")
        Brightness=request->getParam("value")->value().toInt();
      if(Settings_name=="Saturation")
        Saturation=request->getParam("value")->value().toInt();
      if(Settings_name=="Contrast")
        Contrast=request->getParam("value")->value().toInt();
      if(Settings_name=="Mode")
        Mode=request->getParam("value")->value().toInt();
      if(Settings_name=="ISO-Value-slider" || Settings_name=="ISO-Value-txt" )
        ISOValue=request->getParam("value")->value().toInt();
      if(Settings_name=="Clock-2x")
        Clock2x=request->getParam("value")->value().toInt();     

      Serial.println("ImageResolution: "+ String(ImageResolution));
      
    }
    else if(request->getParam("request")->value()=="cordSettings"){
      String point = request->getParam("Point")->value();
      if( point=="1"){
        X1=request->getParam("X")->value().toInt();
        Y1=request->getParam("Y")->value().toInt();
      }
      else if( point=="2"){
        X2=request->getParam("X")->value().toInt();
        Y2=request->getParam("Y")->value().toInt();
      }
      else if( point=="3"){
        X3=request->getParam("X")->value().toInt();
        Y3=request->getParam("Y")->value().toInt();
      }
      else if( point=="4"){
        X4=request->getParam("X")->value().toInt();
        Y4=request->getParam("Y")->value().toInt();
      }
    }
    else if(request->getParam("request")->value()=="cordReset"){
      X1=0;
      Y1=0;  
      X2=width;  
      Y2=0;  
      X3=width;  
      Y3=height; 
      X4=0;
      Y4=height; 
    }
    else if(request->getParam("request")->value()=="allDone"){
      request->send(200, "text/plain", "OK");
      WriteSettingsToEEPROM();
      EEPROM.write(18, 0);
      EEPROM.commit(); 
      Serial.println("Restarting the board..");  
      ESP.restart();
      
    }
  request->send(200, "text/plain", "OK");
}

void CaptureImageAndSendToWebPage(AsyncWebServerRequest* request){
    Serial.println("Resoulution: "+String(ImageResolution));
    if(captureFlag){
      captureFlag= false;
      setCameraParams(Brightness,Contrast,Saturation,Mode,ISOValue,Clock2x);
      camera_fb_t * fb = esp_camera_fb_get(); // gets the frame buffer 
      if (fb){
        if (out != NULL)
          free(out);
        bool RESULT= fmt2jpg(fb->buf,fb->len,fb->width,fb->height,fb->format,50,&out,&out_len);
//        bool RESULT= fmt2jpg((uint8_t*)tmep_buff,115200,240,240,PIXFORMAT_RGB565,50,&out,&out_len);
        if(RESULT)
          Serial.println("converted");
        else
          Serial.println("failed to convert");
        esp_camera_fb_return(fb);  
        
      }
      captureFlag=true;
    }
    Serial.println("Image requesed");
    AsyncWebServerResponse *response = request->beginResponse_P(200, JPG_CONTENT_TYPE,out,out_len);
    request->send(response);
}

void streamJpg(AsyncWebServerRequest *request){
    AsyncJpegStreamResponse *response = new AsyncJpegStreamResponse();
    if(!response){
        request->send(501);
        return;
    }
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}


void ConnetToWiFiAndStartServer(){
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected");
  Serial.println("IP address: " + WiFi.localIP().toString()); 
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  
//   Initialize the web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
                     request->send_P(200, "text/html", html);});
  server.on("/image.jpg", HTTP_GET, CaptureImageAndSendToWebPage);
  server.on("/stream", HTTP_GET, streamJpg);
  server.on("/load-variables",HTTP_GET,SendVraiblesToWeb);
  server.on("/default-variables",HTTP_GET,SendDeafultVarToWeb);
  server.on("/imageset", HTTP_GET,GetVraiblesFromeWeb); 
  server.begin(); 
}
//-----------------------------------------------------------------------------------------------------------------------------
void OTASetup(){
  Serial.println("Starting OTA");
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
   ArduinoOTA.setHostname("ESP32CAM");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void enableServerMode(){
    webServerState=1;
    EEPROM.write(18, 1);
    EEPROM.commit();
    Serial.println("Restarting the board..");
    ESP.restart();
  
}


////////////////////////////////////////////////////////////////////////////////////////////////
// CORE 1 Functions
///////////////////////////////////////////////////////////////////////////////


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
//    if(status != ESP_NOW_SEND_SUCCESS){
//      Serial.println("Failed to send");
//      
//    }
    ESPNowMsgSent=true;
}

void calculate_factors(){
  P1P2_fact=(float)(P1P2_dis-1)/(P1P2_count-1);
  P2P3_fact=(float)(P2P3_dis-1)/(P2P3_count-1);
  P4P3_fact=(float)(P4P3_dis-1)/(P4P3_count-1);
  P1P4_fact=(float)(P1P4_dis-1)/(P1P4_count-1);
}
RGB_avg Pixel_to_RGB(int A , int B){
//  int p = A | (B << 8);
  int p = B | (A << 8);
  RGB_avg RGB_temp;

  RGB_temp.R=(p >> 11) << 3;
  RGB_temp.G=((p >> 5) & 0b111111) << 2;
  RGB_temp.B=(p & 0b11111) << 3;
//    Serial.println("Pixel_to_RGB")
    return RGB_temp;
   
  
}
RGB_avg RGB_struct_copy_pow(RGB_avg A,RGB_avg B){
    RGB_avg C;
//    C.R=A.R+pow(B.R,2);
//    C.G=A.G+pow(B.G,2);
//    C.B=A.B+pow(B.B,2);
    C.R=A.R+B.R;
    C.G=A.G+B.G;
    C.B=A.B+B.B;
    return C;
}
void get_Pixel_RGB_avg(int side_index,int (*side)[8][2],int LED_index,int max_dis){
//void get_Pixel_RGB_avg(int side_index,int ***side,int LED_index,int max_dis){
  RGB_avg RGB_temp;
  int total_pix_count=0;
  RGB_1.R=0;
  RGB_1.G=0;
  RGB_1.B=0;
  for(int k=0;k<pixDepth;k++){
      
      RGB_temp = Pixel_to_RGB(side[side_index][k][0],side[side_index][k][1]);// o
    //RGB_temp = Pixel_to_RGB(side[side_index][1][0],side[side_index][1][1]);//d+o
      RGB_1=RGB_struct_copy_pow(RGB_1,RGB_temp);
      total_pix_count+=1;
      
      if (side_index+1<max_dis){
        RGB_temp = Pixel_to_RGB(side[side_index+1][k][0],side[side_index+1][k][1]);//+1
      //RGB_temp = Pixel_to_RGB(side[side_index+1][1][0],side[side_index+1][1][1]);//d+1
        RGB_1=RGB_struct_copy_pow(RGB_1,RGB_temp);
        total_pix_count+=1;
      }
      
      if (side_index-1>=0){
        RGB_temp = Pixel_to_RGB(side[side_index-1][k][0],side[side_index-1][k][1]);//-1
      //RGB_temp = Pixel_to_RGB(side[side_index-1][1][0],side[side_index-1][1][1]);//d-1
        RGB_1=RGB_struct_copy_pow(RGB_1,RGB_temp);
        total_pix_count+=1;
      }    
  }
    
//  LED.Led_R[LED_index]=round(sqrt(((float)RGB_1.R)/total_pix_count));
//  LED.Led_G[LED_index]=round(sqrt(((float)RGB_1.G)/total_pix_count));
//  LED.Led_B[LED_index]=round(sqrt(((float)RGB_1.B)/total_pix_count));
  LED.Led_R[LED_index]=round(((float)RGB_1.R)/total_pix_count);
  LED.Led_G[LED_index]=round(((float)RGB_1.G)/total_pix_count);
  LED.Led_B[LED_index]=round(((float)RGB_1.B)/total_pix_count);

//  Serial.printf("%u %u %u \n",LED.Led_R[LED_index],LED.Led_G[LED_index],LED.Led_B[LED_index]);
  
//  Serial.printf("side:%d ",LED_index);
//  Serial.printf("led_index:%d side_index:%d Pix:%x %x \n",LED_index,side_index, side[side_index][0][0],side[side_index][0][1]);
//  Serial.printf(" R:%u ",RGB_temp.R);
//  Serial.printf(" G:%u ",RGB_temp.G);
//  Serial.printf(" B:%u \n",RGB_temp.B);  
}

void Send_RGB_data(){
  LED.Remaing_counts=LED.Actaully_total_length;
  while(LED.Remaing_counts!=0){
    memcpy(dataPacket.Led_R,LED.Led_R+(LED.Actaully_total_length-LED.Remaing_counts),LED.Remaing_counts>82?82:LED.Remaing_counts);
    memcpy(dataPacket.Led_G,LED.Led_G+(LED.Actaully_total_length-LED.Remaing_counts),LED.Remaing_counts>82?82:LED.Remaing_counts);
    memcpy(dataPacket.Led_B,LED.Led_B+(LED.Actaully_total_length-LED.Remaing_counts),LED.Remaing_counts>82?82:LED.Remaing_counts);  
    dataPacket.start_pos=LED.Actaully_total_length-LED.Remaing_counts;
    dataPacket.len=LED.Remaing_counts>82?82:LED.Remaing_counts;
//    Serial.printf("Total length:{%d} Remaing counts:{%d} len :{%d}\n",LED.Actaully_total_length,LED.Remaing_counts,LED.Remaing_counts>82?82:LED.Remaing_counts);
    if(ESPNowMsgSent){
      esp_err_t result = esp_now_send(SlaveAddress, (uint8_t *) &dataPacket, sizeof(dataPacket));
//      Serial.printf("%u %u %u\n",dataPacket.Led_R[0],dataPacket.Led_R[1],dataPacket.Led_R[2]);
//      Serial.print("Send Status: ");
//      if (result == ESP_OK) {
//        Serial.println("Success");
//      } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
//        // How did we get so far!!
//        Serial.println("ESPNOW not Init.");
//      } else if (result == ESP_ERR_ESPNOW_ARG) {
//        Serial.println("Invalid Argument");
//      } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
//        Serial.println("Internal Error");
//      } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
//        Serial.println("ESP_ERR_ESPNOW_NO_MEM");
//      } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
//        Serial.println("Peer not found.");
//      } else {
//        Serial.println("Not sure what happened");
//      }
      ESPNowMsgSent=false;
      if(LED.Remaing_counts>=82)
        LED.Remaing_counts=LED.Remaing_counts-82;
      else
        LED.Remaing_counts=0;
    }
  }
}

void pixel_sclicing(){
   for(int i=0;i<max_1D_led_count;i++){
      if(i<P1P2_count){
        get_Pixel_RGB_avg(floor(P1P2_fact*(P1P2_count-i-1)),P1P2,i,P1P2_dis);
//        Serial.printf("Fact : %f  1:Side index: %f led index: %d P1P2_count-i-1 :%d P1P2_count:%d \n",
//                      P1P2_fact,floor(P1P2_fact*(P1P2_count-i-1)),i,P1P2_count-i-1,P1P2_count);
      }

      if(i<P1P4_count){
        get_Pixel_RGB_avg(floor(P1P4_fact*(i)),P1P4,P1P2_count+i,P1P4_dis);
//        Serial.printf("Fact : %f  2:Side index: %f led index: %d i :%d  \n",
//              P1P4_fact,floor(P1P4_fact*(i)),P1P2_count+i,i);
      }
      if(i<P4P3_count){
        get_Pixel_RGB_avg(floor(P4P3_fact*(i)),P4P3,P1P2_count+P1P4_count+i,P4P3_dis);
//        Serial.printf("Fact : %f  3:Side index: %f led index: %d i :%d  \n",
//              P4P3_fact,floor(P4P3_fact*(i)),P1P2_count+P1P4_count+i,i);
      }
      if(i<P2P3_count){
        get_Pixel_RGB_avg(floor(P2P3_fact*(P2P3_count-i-1)),P2P3,P4P3_count+P1P2_count+P1P4_count+i,P2P3_dis);
//        Serial.printf("Fact : %f  4:Side index: %f led index: %d i :%d  \n",
//              P2P3_fact,floor(P2P3_fact*(P2P3_count-i-1)),P4P3_count+P1P2_count+P1P4_count+i,i);
      }

      
//      if(i<P2P3_count){
//        get_Pixel_RGB_avg(floor(P2P3_fact*(i)),P2P3,P1P2_count+i,P2P3_dis);
////        Serial.printf("Fact : %f  2:Side index: %f led index: %d i :%d  \n",
////              P2P3_fact,floor(P2P3_fact*(i)),P1P2_count+i,i);
//      }
//      if(i<P4P3_count){
//        get_Pixel_RGB_avg(floor(P4P3_fact*(i)),P2P3,P1P2_count+P2P3_count+i,P4P3_dis);
////        Serial.printf("Fact : %f  3:Side index: %f led index: %d i :%d  \n",
////              P4P3_fact,floor(P4P3_fact*(i)),P1P2_count+P2P3_count+i,i);
//      }
//      if(i<P1P4_count){
//        get_Pixel_RGB_avg(floor(P1P4_fact*(P1P2_count-i-1)),P2P3,P1P2_count+P2P3_count+P4P3_count+i,P1P4_dis);
////        Serial.printf("Fact : %f  4:Side index: %f led index: %d i :%d  \n",
////              P1P4_fact,floor(P1P4_fact*(P1P2_count-i-1)),P1P2_count+P2P3_count+P4P3_count+i,i);
//      }
      
   }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  typedef struct recv_message{
    uint8_t Led_R[82];
    uint8_t Led_G[82];
    uint8_t Led_B[82];
    uint16_t start_pos;
    uint16_t len;    
  } recv_message;

  recv_message recv_data;
  memcpy(&recv_data, incomingData, sizeof(recv_data));
  if(recv_data.Led_R[0]==1 && 
    recv_data.Led_G[0]==2 &&
    recv_data.Led_B[0]==1 &&
    recv_data.start_pos==2 &&
    recv_data.len==0){
    Serial.println("Switching to Webserver mode");
    enableServerMode(); 
  }
  
}
