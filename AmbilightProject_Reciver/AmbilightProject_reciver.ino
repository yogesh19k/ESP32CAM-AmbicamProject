/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp8266-nodemcu-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#define FASTLED_FORCE_SOFTWARE_SPI
#define FASTLED_FORCE_SOFTWARE_PINS
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <FastLED.h>

#include <EEPROM.h>
//ota
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define NUM_LEDS 266
#define DATA_PIN 2
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define BRIGHTNESS 100
#define VOLTS 5
#define MAX_AMPS 1500 // value in miliamps

const char* ssid = "get off its mine 2";
const char* password = "home123456";

int Mode;

// Structure example to receive data
// Must match the sender structure

uint8_t led_buffer_R[NUM_LEDS];
uint8_t led_buffer_G[NUM_LEDS];
uint8_t led_buffer_B[NUM_LEDS];


typedef struct struct_message{
  uint8_t Led_R[82];
  uint8_t Led_G[82];
  uint8_t Led_B[82];
  uint16_t start_pos;
  uint16_t len;    
  
}struct_message;

// Create a struct_message called myData
struct_message dataPacket;

CRGB leds[NUM_LEDS];


const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Output - GPIO 5</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"5\" ><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}



// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&dataPacket, incomingData, sizeof(dataPacket));
  if(dataPacket.Led_R[0]==1 && 
    dataPacket.Led_G[0]==2 &&
    dataPacket.Led_B[0]==1 &&
    dataPacket.start_pos==2 &&
    dataPacket.len==0){
    EEPROM.write(0, 0);
    EEPROM.commit();
    ESP.restart();
  }
  Serial.println(dataPacket.Led_R[0]);
  Serial.println(dataPacket.Led_G[0]);
  Serial.println(dataPacket.Led_B[0]);
//  Serial.println(dataPacket.start_pos);
//  Serial.println(dataPacket.len);


//  printf("packet len: {%d} Start pos:{%d} len:{%d}\n",len,dataPacket.start_pos,dataPacket.len);
  memcpy(led_buffer_R+dataPacket.start_pos,dataPacket.Led_R,dataPacket.len);
  memcpy(led_buffer_G+dataPacket.start_pos,dataPacket.Led_G,dataPacket.len);
  memcpy(led_buffer_B+dataPacket.start_pos,dataPacket.Led_B,dataPacket.len);
//  Serial.println("message recived");
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200); 
  Serial.println();
  EEPROM.begin(1);
  Mode=EEPROM.read(0);
  // Set device as a Wi-Fi Station
  if(Mode){
    normalMode();
    Serial.println("Normal mode");
  }
  else{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());   
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    
    ServerSetup();
    StartOTA();
  }
  
}

void loop() {
  if(Mode){
    for(int i=0;i<NUM_LEDS;i++){
//      if(led_buffer_R[i]>20 or led_buffer_G[i]>20 or led_buffer_B[i]>20 ){
//        leds[i]=CRGB(led_buffer_R[i],led_buffer_G[i],led_buffer_B[i]);
//        Serial.printf("%d=(%u,%u,%u) ",i,led_buffer_R[i],led_buffer_G[i],led_buffer_B[i]);
//      }
//      else{
//        leds[i]=CRGB(0,0,0);
//      }
      leds[i]=CRGB(led_buffer_R[i],led_buffer_G[i],led_buffer_B[i]);
//        leds[i]=CRGB((255-i)<0?i:255-i,0,0);
    }
//    Serial.println();
//     leds[0]=CRGB(255,0,0);
//     leds[85]=CRGB(255,0,0);
//     
//     leds[86]=CRGB(0,255,0);
//     leds[132]=CRGB(0,255,0);
//     
//     leds[133]=CRGB(0,0,255);
//     leds[218]=CRGB(0,0,255);
//     
//     leds[219]=CRGB(255,255,255);
//     leds[265]=CRGB(255,255,255);
//  for(int i=0,j=84;i<85;i++,j--)
//  {
//     leds[i]=CRGB(led_buffer_R[j],led_buffer_G[j],led_buffer_B[j]);
//     if(i<16){
//      Serial.printf("%d) R:%d G:%d B:%d\n",i,led_buffer_R[j],led_buffer_G[j],led_buffer_B[j]);
//     }
//  }
    FastLED.show();
  }
  else{
    ArduinoOTA.handle();
  }

  
}

void normalMode(){
  WiFi.mode(WIFI_STA);
//   Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  FastLED.addLeds<CHIPSET,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MAX_AMPS);
  FastLED.clear();
  FastLED.show();
}

void ServerSetup(){  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK");
    EEPROM.write(0, 1);
    EEPROM.commit();
    ESP.restart();
  });

  // Start server
  server.begin();
  Serial.println("Server started");
}

void StartOTA(){
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname("AmbilightReciver");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");

}
