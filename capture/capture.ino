#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"
#include "base64.h"
#include <Arduino.h>
#include "soc/soc.h"
#define CAMERA_MODEL_WROVER_KIT
#include "soc/rtc_cntl_reg.h"
#include "camera_pins.h"
#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480



// #include <ArduinoJson.h>
// Set the camera image format
#define CAMERA_PIXEL_FORMAT PIXFORMAT_JPEG
// WiFi credentials
const char* ssid = "1323";
const char* password = "S2r1C$M2";

// Flask server details
String serverUrl = "http://shoppingcart.anaghascu.com";
String serverPath = "/cart";
// const char* serverName = "your_server_name";


void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void loop() {
  // Capture image and convert to base64
  camera_fb_t* fb = esp_camera_fb_get();
  String base64Image = base64::encode(fb->buf, fb->len);
  // Serial.println(base64Image);
  
  // Construct JSON data
  String jsonData = "{\"img_key\":\"" + base64Image + "\"}";

  // Send POST request to server
  HTTPClient http;
  http.begin(serverUrl + serverPath);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonData);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.println("Error sending request");
  }
  http.end();

  esp_camera_fb_return(fb);

  // Wait for some time before taking next picture
  delay(5000);
}