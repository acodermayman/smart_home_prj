#include <SoftwareSerial.h>
#include "ArduinoJson.h"
#include <WebSocketsServer.h>

//Setup Wifi
const char *ssid =  "DucDz";   //Wifi SSID (Name)
const char *pass =  "Trunganc"; //wifi password
WebSocketsServer webSocket = WebSocketsServer(81);

#define RX_PIN D2 // Chân RX-của EspSoftwareSerial
#define TX_PIN D1 // Chân TX của EspSoftwareSerial

#define LED_BUTTON_PIN D0
#define DOOR_BUTTON_PIN D5

SoftwareSerial mySerial(RX_PIN , TX_PIN);
StaticJsonDocument<500> json;

float temperature = 0;
float humidity = 0;

bool ledState = LOW;
bool buttonState = HIGH;
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(LED_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOOR_BUTTON_PIN, INPUT_PULLUP);


//setup connect
  Serial1.println("Connecting to wifi");

  IPAddress apIP(192, 168, 99, 100);   //Static IP for wifi gateway
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); //set Static IP gateway on NodeMCU
  WiFi.softAP(ssid, pass); //turn on WIFI

  webSocket.begin(); //websocket Begin
  webSocket.onEvent(webSocketEvent); //set Event for websocket
  Serial1.println("Websocket is started");
}


void loop() {
  webSocket.loop();

  if (mySerial.available()) {
    String line = mySerial.readStringUntil('\n');
    DeserializationError error = deserializeJson(json, line);
    if (error) {
      Serial1.print("deserializeJson() failed: ");
      Serial1.println(error.c_str());
      return;
    }
    ----------------------------
    // Lấy giá trị nhiệt độ và độ ẩm từ đối tượng JsonDocument
    temperature = json["temperature"];
    humidity = json["humidity"];
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    char msg[256];
    serializeJson(json, msg);
    webSocket.broadcastTXT(msg);
  }
  physicalButton();
}
void remoteButton(bool active){
  if(active){
    ledState = HIGH;
  }else{
    ledState = LOW;
  }
  json.clear();
  json["led"] = ledState;
  serializeJson(json, mySerial);
  mySerial.println();
}
void physicalButton(){
  if(digitalRead(LED_BUTTON_PIN)==LOW){
    if (buttonState==HIGH){
      ledState = !ledState;
      delay(200);
    }
    buttonState = LOW;
  }
  else{
    buttonState = HIGH;
  }
  
  json.clear();
  json["led"] = ledState;
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
      if(cmd == "LedOn"){
        remoteButton(true);
      }
      if(cmd == "LedOff"){
        remoteButton(false);
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

