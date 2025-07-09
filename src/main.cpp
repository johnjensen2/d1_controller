#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>


#include "codes.h"

// Replace with your WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Serial port to S3
#define S3_SERIAL Serial
#define S3_BAUD 115200

AsyncWebServer server(80);


// Send one-byte command over UART
void sendCommand(uint8_t cmd) {
  S3_SERIAL.write(cmd);
}

// Send LoRa command string framed as <LORA:...>
void sendLoRaCommand(const String &msg) {
  S3_SERIAL.print("<LORA:");
  S3_SERIAL.print(msg);
  S3_SERIAL.println(">");
}


// ============ Web Requests =============
void setupWeb() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

server.on("/cmd", HTTP_POST, [](AsyncWebServerRequest *request) {
  if (request->hasParam("cmd", true)) {
    String cmd = request->getParam("cmd", true)->value();

    if (cmd == CMD_STR_ARE_YOU_THERE) {
      sendCommand(CMD_ARE_YOU_THERE);
      request->send(200, "text/plain", "Sent: Are You There?");
    } else if (cmd == CMD_STR_EMERGENCY_STOP) {
      sendCommand(CMD_EMERGENCY_STOP);
      request->send(200, "text/plain", "Sent: Emergency Stop");
    } else if (cmd == CMD_STR_SEND_SENSOR_DATA) {
      sendCommand(CMD_SEND_SENSOR_DATA);
      request->send(200, "text/plain", "Sent: Sensor Data Request");
    } else {
      request->send(400, "text/plain", "Unknown command");
    }
  } else if (request->hasParam("lora", true)) {
    String msg = request->getParam("lora", true)->value();
    Serial.print("<LORA:");
    Serial.print(msg);
    Serial.println(">");
    request->send(200, "text/plain", "LoRa message sent: " + msg);
  } else {
    request->send(400, "text/plain", "No valid parameters");
  }
});


  server.on("/estop", HTTP_POST, [](AsyncWebServerRequest *request){
    S3_SERIAL.println("<ESTOP>");
    request->send(200, "text/plain", "Emergency Stop Sent");
  });

  server.on("/send", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("msg", true)) {
      String msg = request->getParam("msg", true)->value();
      S3_SERIAL.print("<LORA:");
      S3_SERIAL.print(msg);
      S3_SERIAL.println(">");
      request->send(200, "text/plain", "Sent: " + msg);
    } else {
      request->send(400, "text/plain", "Missing 'msg' param");
    }
  });

  server.begin();
}

// ============ OTA =============
void setupOTA() {
  ArduinoOTA.setHostname("rover-d1-mini");

  ArduinoOTA.onStart([]() {
    S3_SERIAL.println("OTA Update Started");
  });

  ArduinoOTA.onEnd([]() {
    S3_SERIAL.println("\nOTA Update Complete");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    S3_SERIAL.printf("OTA Error[%u]\n", error);
  });

  ArduinoOTA.begin();
}

// ============ Setup =============
void setup() {
  S3_SERIAL.begin(S3_BAUD);
  delay(500);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

 if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
  }

  setupOTA();
  setupWeb();

  S3_SERIAL.println("D1 Override Node Ready");
}

// ============ Loop =============
void loop() {
  ArduinoOTA.handle();
}
