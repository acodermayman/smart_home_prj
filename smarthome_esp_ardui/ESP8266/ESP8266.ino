#include <SoftwareSerial.h>
#include "ArduinoJson.h"
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;
#define RX_PIN D2 
#define TX_PIN D1 
//Setup Wifi
const char *ssid =  "iphone galaxy y";   
const char *password =  "123456789"; 
SoftwareSerial mySerial(RX_PIN , TX_PIN);
// WebSocket server
WebSocketsClient webSocket;
SocketIOclient socketIO;

StaticJsonDocument<200> json;

float temperature = 0;
float humidity = 0;
bool livingRoomLedState = false;
bool bedRoomLedState = false;
bool doorState = false;
bool fireDetected = false;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Đang kết nối Wi-Fi...");
  }
  // WiFiMulti.addAP(ssid, password);
  // while (WiFiMulti.run() != WL_CONNECTED) { // Tự động chọn mạng khả dụng
  //   delay(500);
  //   Serial.print(".");
  // }
  Serial.println("Wi-Fi đã kết nối!");
  Serial.println(WiFi.localIP());
  // Kết nối WebSocket tới server
  webSocket.begin("172.20.10.7", 3000);
  webSocket.onEvent(webSocketEvent);
    // Serial.println("Attempting WebSocket connection...");

  // socketIO.begin("192.168.1.12", 3000,"/esp");
  // socketIO.onEvent(socketIOEvent);
  // socketIO.on("esp_command",messageEvenetHandler);
  // void messageEvenetHandler(const char * payload, size_t length) {
  // Serial.printf("got message: %s\n", payload);

  // socketIO.on("message",handleMessage);
  // void messageEvenetHandler(const char * message, size_t length) {
  // Serial1.printf(message);
}



void loop() {
  webSocket.loop();
  // socketIO.loop();
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
    livingRoomLedState = json["livingRoomLedState"];
    bedRoomLedState = json["bedRoomLedState"];
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
    sentStatus();
  }
}

void sentStatus() {
  json.clear();
  json["livingRoomLedState"] = livingRoomLedState;
  json["bedRoomLedState"] = bedRoomLedState;
  json["doorState"] = doorState;
  json["temperature"] = temperature;
  json["humidity"] = humidity;
  json["fireDetected"] = fireDetected;
  char msg[256];
  serializeJson(json, msg);
  webSocket.sendTXT(msg);
  if (!webSocket.sendTXT(msg)) {
    Serial.println("Error sending WebSocket message!");
}
  // socketIO.emit("data",msg);  
  // socketIO.sendEVENT(msg); 
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

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  //webscket event method
  String cmd = "";
  switch (type) {
    case WStype_DISCONNECTED:
      Serial1.println("Websocket is disconnected");
      webSocket.sendTXT("esp is disconnected websocket");
      break;
    case WStype_CONNECTED: {
        Serial1.println("Websocket is connected");
        // Serial1.println(webSocket.remoteIP(num).toString());
        webSocket.sendTXT("esp connected websocket");
      }
      break;
    case WStype_TEXT:
      cmd = "";
      for (int i = 0; i < length; i++) {
        cmd = cmd + (char) payload[i];
      } //merging payload to single string
      Serial1.print("Data from server:");
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

// void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
// //  //Serial.print(payload);
//     switch(type) {
//         case sIOtype_DISCONNECT:
//             Serial.printf("[IOc] Disconnected!\n");
//             break;
//         case sIOtype_CONNECT:
//             Serial.printf("[IOc] Connected to url: %s\n", payload);
//             // join default namespace (no auto join in Socket.IO V3)
//             socketIO.send(sIOtype_CONNECT, "/");
//             break;
//         case sIOtype_EVENT:
//         {
//             // Serial.printf("[IOc] get event: %s\n", payload);
//             // String txt = (const char *) &payload[0];
//             // Serial.println((const char *) &payload[2]);
//             // Serial.println(txt);
//             // Serial.println(txt[2]);
//             // if (txt[2] == '~') {
//             //   Serial.println("True request");
//             //   digitalWrite(0, HIGH); // Khi client phát sự kiện "LED_ON" thì server sẽ bật LED
//             //   digitalWrite(12, HIGH);  
//             //   digitalWrite(13, HIGH);
//             // } else if (txt[2] == '!') {
//             //   Serial.println("OFF");
//             //   digitalWrite(0, LOW); // Khi client phát sự kiện "LED_OFF" thì server sẽ tắt LED
//             //   digitalWrite(12, LOW);
//             //   digitalWrite(13, LOW);
//             // }
//         }
//             break;
//         case sIOtype_ACK:
//             Serial.printf("[IOc] get ack: %u\n", length);
//             hexdump(payload, length);
//             break;
//         case sIOtype_ERROR:
//             Serial.printf("[IOc] get error: %u\n", length);
//             hexdump(payload, length);
//             break;
//         case sIOtype_BINARY_EVENT:
//             Serial.printf("[IOc] get binary: %u\n", length);
//             hexdump(payload, length);
//             break;
//         case sIOtype_BINARY_ACK:
//             Serial.printf("[IOc] get binary ack: %u\n", length);
//             hexdump(payload, length);
//             break;
//     }
// }
