#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "camera.h"
#include "functions.h"




void core1_task(void * pvParameters ){
  core1_setup();
  while(true){
    core1_loop();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("-------Serial begin-------");
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector  
  ReadSettingsFromEEPROM();
  getWidthHightFromResolution();  
//  allocMemeForVar();
  cameraInit(ImageResolution);
  setCameraParams(Brightness,Contrast,Saturation,Mode,ISOValue,Clock2x);
  if(!webServerState){
    Serial.println("-------Abmilight Mode-------");
    keystone_setup();
    xTaskCreatePinnedToCore(
                    core1_task,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 0 */  
    
  }
  else{
    Serial.println("-------Webserver and OTAUpdate Mode-------");
    ConnetToWiFiAndStartServer();
    OTASetup();
  }
      
}

void loop() {
  if(!webServerState){
     camera_fb_t * frame_buffer = esp_camera_fb_get(); // gets the frame buffer 
     if (frame_buffer){
//        pre_process(tmep_buff);
        pre_process(frame_buffer->buf);
        esp_camera_fb_return(frame_buffer);              
     };
  }
  else{
    delay(100);
    ArduinoOTA.handle();
  }

}


////////////////////////////////////////////////////////////////////////////
//----------------------CORE-1 TASKS---------------------------
//////////////////////////////////////////////////////////////////////////////

void core1_setup(){
    Serial.print("trasmission Core: ");
    Serial.println(xPortGetCoreID());
    WiFi.mode(WIFI_STA);
  
    if (esp_now_init() != ESP_OK){
      Serial.println("Error initializing ESP-NOW");
      return;
    }
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    //Register peer
    memcpy(peerInfo.peer_addr, SlaveAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    //Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
    }
    
    calculate_factors(); 
    LED.Actaully_total_length=266;
    
}

void core1_loop(){
  pixel_sclicing();
  Send_RGB_data();
}
