#include <SoftwareSerial.h>
#include "ArduinoJson.h"
#include <WebSocketsServer.h>
#define RX_PIN D2 
#define TX_PIN D1 
//Setup Wifi
const char *ssid =  "DucDz";   
const char *pass =  "Trunganc"; 
SoftwareSerial mySerial(RX_PIN , TX_PIN);
StaticJsonDocument<200> json;
WebSocketsServer webSocket = WebSocketsServer(81);

float temperature = 0;
float humidity = 0;

bool livingRoomLedState = false;
bool bedRoomLedState = false;
bool doorState = false;
bool fireDetected = false;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial1.println("Connecting to wifi");

  IPAddress apIP(192, 168, 99, 100);  
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); //set Static IP gateway on NodeMCU
  WiFi.softAP(ssid, pass);

  webSocket.begin(); 
  webSocket.onEvent(webSocketEvent);
  Serial1.println("Websocket is started");
}


void loop() {
  webSocket.loop();
  receiveDataFromArduino();
}

void receiveDataFromArduino() {
  if (mySerial.available()) {
    String line = mySerial.readStringUntil('\n');
    DeserializationError error = deserializeJson(json, line);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Parse JSON data
    livingRoomLedState = json["livingRoomLed"];
    bedRoomLedState = json["bedRoomLed"];
    doorState = json["doorState"];
    temperature = json["temperature"];
    humidity = json["humidity"];
    fireDetected = json["fireDetected"];

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Broadcast the received data to all WebSocket clients
    broadcastStatus();
  }
}

void broadcastStatus() {
  json.clear();
  json["livingRoomLedState"] = livingRoomLedState;
  json["bedRoomLedState"] = bedRoomLedState;
  json["doorState"] = doorState;
  json["temperature"] = temperature;
  json["humidity"] = humidity;
  json["fireDetected"] = fireDetected;
  char msg[256];
  serializeJson(json, msg);
  webSocket.broadcastTXT(msg);  // Send JSON as a string to all clients
}

void controlLivingRoomLed(bool state) {
  livingRoomLedState = state;
  sendUpdateToArduino("livingRoomLedState", state);
}

void controlBedRoomLed(bool state) {
  bedRoomLedState = state;
  sendUpdateToArduino("bedRoomLedState", state);
}

void controlDoor(bool state) {
  doorState = state;
  sendUpdateToArduino("doorState", state);
}

void sendUpdateToArduino(const char* key, bool state) {
  json.clear();
  json[key] = state;
  serializeJson(json, mySerial);
  mySerial.println();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  //webscket event method
  String cmd = "";
  switch (type) {
    case WStype_DISCONNECTED:
      Serial1.println("Websocket is disconnected");
      break;
    case WStype_CONNECTED: {
        Serial1.println("Websocket is connected");
        Serial1.println(webSocket.remoteIP(num).toString());
        webSocket.sendTXT(num, "connected");
      }
      break;
    case WStype_TEXT:
      cmd = "";
      for (int i = 0; i < length; i++) {
        cmd = cmd + (char) payload[i];
      } //merging payload to single string
      Serial1.print("Data from flutter:");
      Serial1.println(cmd);
      if (cmd == "LivingRoomLedOn") {
        controlLivingRoomLed(true);
      } else if (cmd == "LivingRoomLedOff") {
        controlLivingRoomLed(false);
      } else if (cmd == "BedRoomLedOn") {
        controlBedRoomLed(true);
      } else if (cmd == "BedRoomLedOff") {
        controlBedRoomLed(false);
      } else if (cmd == "OpenDoor") {
        controlDoor(true);
      } else if (cmd == "CloseDoor") {
        controlDoor(false);
      }
      break;
    case WStype_FRAGMENT_TEXT_START:
      break;
    case WStype_FRAGMENT_BIN_START:
      break;
    case WStype_BIN:
      hexdump(payload, length);
      break;
    default:
      break;
  }
}
