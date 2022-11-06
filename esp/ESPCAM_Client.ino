/**
 * @file ESPCAM_Client.ino
 * @authors Matthew Rubino and Aditya Pandya
 * @brief Sends video data over WebSocket
 * @version 1.0
 * @date 11-05-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#define CAMERA_MODEL_AI_THINKER

#include <WiFi.h>
#include <esp_websocket_client.h>

#include "ArduinoJson.h"
#include "esp_camera.h"
#include "camera_pins.h"
#include "homecredentials.h"


// Add trusted root certificate
const esp_websocket_client_config_t ws_cfg {
  .uri = "wss://changingtheworldwith.tech/machine",
  .cert_pem = "-----BEGIN CERTIFICATE-----\nMIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\nTzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\ncmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\nWhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\nZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\nMTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\nh77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\nA5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\nT8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\nB5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\nB5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\nKBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\nOlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\njh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\nqHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\nrU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\nHRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\nhkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\nubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\nNFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\nORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\nTkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\njNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\noyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\nmRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\nemyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n-----END CERTIFICATE-----"
};
esp_websocket_client_handle_t ws_client;


void setup() {
  Serial.begin(115200);

  // WiFi
  Serial.println("Connecting to WiFi...");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  
  Serial.println("Successfully connected to WiFi!");
  delay(1000);

  // WebSocket
  Serial.println("Connecting to WebSocket...");

  ws_client = esp_websocket_client_init(&ws_cfg);
  esp_websocket_client_start(ws_client);
  
  delay(3000);
  Serial.println("Checking connection...");
  
  if (esp_websocket_client_is_connected(ws_client)) {
    Serial.println("Successfully connected to WebSocket!");
  } else {
    Serial.println("Failed to connect to WebSocket.");
    return;
//    exit(0);
  }

  // Cameras
  // From File > Examples > ESP32 > Camera > CameraWebServer
  Serial.println("Initializing camera...");
  
  camera_config_t cam_cfg;
  cam_cfg.ledc_channel = LEDC_CHANNEL_0;
  cam_cfg.ledc_timer = LEDC_TIMER_0;
  cam_cfg.pin_d0 = Y2_GPIO_NUM;
  cam_cfg.pin_d1 = Y3_GPIO_NUM;
  cam_cfg.pin_d2 = Y4_GPIO_NUM;
  cam_cfg.pin_d3 = Y5_GPIO_NUM;
  cam_cfg.pin_d4 = Y6_GPIO_NUM;
  cam_cfg.pin_d5 = Y7_GPIO_NUM;
  cam_cfg.pin_d6 = Y8_GPIO_NUM;
  cam_cfg.pin_d7 = Y9_GPIO_NUM;
  cam_cfg.pin_xclk = XCLK_GPIO_NUM;
  cam_cfg.pin_pclk = PCLK_GPIO_NUM;
  cam_cfg.pin_vsync = VSYNC_GPIO_NUM;
  cam_cfg.pin_href = HREF_GPIO_NUM;
  cam_cfg.pin_sscb_sda = SIOD_GPIO_NUM;
  cam_cfg.pin_sscb_scl = SIOC_GPIO_NUM;
  cam_cfg.pin_pwdn = PWDN_GPIO_NUM;
  cam_cfg.pin_reset = RESET_GPIO_NUM;
  cam_cfg.xclk_freq_hz = 10000000;
  cam_cfg.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    cam_cfg.frame_size = FRAMESIZE_VGA;
    cam_cfg.jpeg_quality = 70;
    cam_cfg.fb_count = 2;
  } else {
    cam_cfg.frame_size = FRAMESIZE_SVGA;
    cam_cfg.jpeg_quality = 70;
    cam_cfg.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&cam_cfg);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    exit(0);
  }
}

void loop() {
  if (esp_websocket_client_is_connected(ws_client)) {
      camera_fb_t *fb = esp_camera_fb_get();

      if(!fb)
      {
        Serial.println("Camera capture failed");
        esp_camera_fb_return(fb);
        return;
      }

      if(fb->format != PIXFORMAT_JPEG)
      {
        Serial.println("Non-JPEG data not implemented");
        return;
      }
      
      esp_websocket_client_send_bin(ws_client, (const char*)fb->buf, fb->len, portMAX_DELAY);
      esp_camera_fb_return(fb);

      delay(0.05);
  }
}
