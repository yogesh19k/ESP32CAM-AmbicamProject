#include "esp_camera.h"

//WROVER-KIT PIN Map
#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK    0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

bool cameraInit(framesize_t ImageRes)
{

    camera_config_t config;
    
    config.pin_pwdn     = CAM_PIN_PWDN;
    config.pin_reset    = CAM_PIN_RESET;
    config.pin_xclk     = CAM_PIN_XCLK;
    config.pin_sscb_sda = CAM_PIN_SIOD;
    config.pin_sscb_scl = CAM_PIN_SIOC;

    config.pin_d7       = CAM_PIN_D7;
    config.pin_d6       = CAM_PIN_D6;
    config.pin_d5       = CAM_PIN_D5;
    config.pin_d4       = CAM_PIN_D4;
    config.pin_d3       = CAM_PIN_D3;
    config.pin_d2       = CAM_PIN_D2;
    config.pin_d1       = CAM_PIN_D1;
    config.pin_d0       = CAM_PIN_D0;
    config.pin_vsync    = CAM_PIN_VSYNC;
    config.pin_href     = CAM_PIN_HREF;
    config.pin_pclk     = CAM_PIN_PCLK;
    
    config.xclk_freq_hz = 20000000; // 18-20 Mhz stable with CLK 2X
    
    config.ledc_timer   = LEDC_TIMER_0;
    config.ledc_channel = LEDC_CHANNEL_0;

    config.pixel_format = PIXFORMAT_RGB565;//RGB565 to get RGB values in frame bufffer ( other formates YUV422,GRAYSCALE,RGB565,JPEG)
    config.frame_size   = ImageRes;//QQVGA-QVGA for RGB656 

    config.jpeg_quality = 13; //0-63 lower number means higher quality
    config.fb_count     = 2; //if more than one, i2s runs in continuous mode. Use only with JPEG
//    config.fb_location  = CAMERA_FB_IN_PSRAM;
//    config.grab_mode    = CAMERA_GRAB_LATEST; 
    
    int tries = 0;
    esp_err_t err;
    do
    {
       //power up the camera if PWDN pin is defined
       if(CAM_PIN_PWDN != -1){
          pinMode(CAM_PIN_PWDN, OUTPUT);
          digitalWrite(CAM_PIN_PWDN, HIGH);
          delay(500);
          digitalWrite(CAM_PIN_PWDN, LOW);
          delay(500);
      }
  
      //initialize the camera
      err = esp_camera_init(&config);
      if (err != ESP_OK) {
          Serial.println("Camera Init Failed.");
          delay(500);
      }
      tries++;
    }while(err != ESP_OK && tries < 3);
    
    if(err == ESP_OK){
      Serial.println("Camera Init Success"); 
    }
    else
    {
      Serial.println("Restarting...");
      ESP.restart();
    }
    return err;
    
}

void setCameraParams(int Bright,int Cont,int Satur,int wb_mode,int ISO,int Clock)
{
  sensor_t *sensor = esp_camera_sensor_get();
  sensor->set_brightness(sensor, Bright);
  sensor->set_contrast(sensor, Cont);
  sensor->set_saturation(sensor, Satur);
  sensor->set_special_effect(sensor, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  sensor->set_whitebal(sensor, 0);       // 0 = disable , 1 = enable
  sensor->set_awb_gain(sensor, 1);       // 0 = disable , 1 = enable
  sensor->set_wb_mode(sensor, wb_mode);    //0: auto, 1: sun, 2: cloud, 3: indoors
  sensor->set_exposure_ctrl(sensor, 0);  // 0 = disable , 1 = enable
  sensor->set_aec2(sensor, 0);           // 0 = disable , 1 = enable
  sensor->set_aec_value(sensor, ISO);    // 0 to 1200
  sensor->set_gain_ctrl(sensor, 0);      // 0 = disable , 1 = enable
  sensor->set_ae_level(sensor, 0);       // -2 to 2
  sensor->set_agc_gain(sensor, 0);       // 0 to 30
  sensor->set_bpc(sensor, 0);            // 0 = disable , 1 = enable
  sensor->set_wpc(sensor, 1);            // 0 = disable , 1 = enable
  sensor->set_raw_gma(sensor, 0);        // 0 = disable , 1 = enable
  sensor->set_lenc(sensor, 0);           // 0 = disable , 1 = enable
  sensor->set_dcw(sensor, 1);            // 0 = disable , 1 = enable
//  sensor->set_hmirror(sensor, 1);        // 0 = disable , 1 = enable
//  sensor->set_vflip(sensor, 1);          // 0 = disable , 1 = enable

//  sensor->set_gainceiling(sensor, (gainceiling_t)0);  // 0 to 6
//  sensor->set_colorbar(sensor, 0);       // 0 = disable , 1 = enable

  if(Clock)
    sensor->set_reg(sensor,0x111,0x80,0x80);  // Setting CLK to 2X
  else
    sensor->set_reg(sensor,0x111,0x80,0x00); // Setting CLK to normal

//  int res = sensor->set_res_raw(sensor, 2, 0, 0, 0, 10, 10, 200, 200, 240, 240, 0, 0);

  //sensor->set_exposure_ctrl(sensor, 1);
  //sensor->set_aec_value(sensor, -2);
  //sensor->set_ae_level(sensor, 100);
  //sensor->set_gain_ctrl(sensor, 100);
  
}
