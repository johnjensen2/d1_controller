#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>

#include "webui.h"
#include "secrets.h"
#include "codes.h"


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

void handleCommand(const String& cmd) {
  Serial.print("[handleCommand] Received cmd: ");
  Serial.println(cmd);

  if (cmd == "areyou") {
    Serial.println("Reply: Yes, I am here.");
  } else if (cmd == "estop") {
    Serial.println("EMERGENCY STOP triggered!");
    // Insert emergency stop logic here
  } else if (cmd == "override_on") {
    Serial.println("Override enabled.");
    // Insert override enable logic here
  } else if (cmd == "override_off") {
    Serial.println("Override disabled.");
    // Insert override disable logic here
  } else if (cmd == "sensordata") {
    Serial.println("Sending sensor data...");
    // Insert sensor data send logic here
  } else {
    Serial.println("Unknown command.");
  }
}

// ============ Web Requests =============
void setupWeb() {
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", index_html);
});

server.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request){
  String cmd = request->getParam("x")->value();
  Serial.println("[CMD] " + cmd);
  handleCommand(cmd);  // your internal command processor
  request->send(200, "text/plain", "Executed: " + cmd);
});

server.on("/cmd", HTTP_POST, [](AsyncWebServerRequest *request){
  if (request->hasParam("lora", true)) {
    String msg = request->getParam("lora", true)->value();
    Serial.println("[LoRa POST] " + msg);
   sendLoRaCommand(msg);  // your implementation
    request->send(200, "text/plain", "LoRa Sent: " + msg);
  } else {
    request->send(400, "text/plain", "Missing lora parameter");
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

 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }


  setupOTA();
  setupWeb();

  S3_SERIAL.println("D1 Override Node Ready");
}

// ============ Loop =============
void loop() {
  ArduinoOTA.handle();


  delay(1000);
}
