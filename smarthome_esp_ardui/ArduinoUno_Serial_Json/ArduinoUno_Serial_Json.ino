#include <SoftwareSerial.h>
#include "ArduinoJson.h"
#include "DHT.h"

#define RX_PIN 10
#define TX_PIN 11 
#define DHTsensor_PIN 7
#define LED_PIN 6

float temperature = 0;
float humidity = 0;
unsigned long lastUpdateTime = 0;

SoftwareSerial mySerial(RX_PIN, TX_PIN); 
StaticJsonDocument<100> json;
DHT dht(DHTsensor_PIN,DHT11);

void setup() {
  dht.begin();
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);
  Serial.begin(9600);
  mySerial.begin(9600);
  dhtEvent();
}

bool led= false;
void loop() {
  unsigned long currentTime = millis(); 
  if (currentTime - lastUpdateTime >= 2000) {
  // Đọc giá trị nhiệt độ và độ ẩm từ cảm biến
  dhtEvent();
  lastUpdateTime = currentTime;
  }

  json.clear();
  if (mySerial.available()){
    String line = mySerial.readStringUntil('\n');
    DeserializationError error = deserializeJson(json, line);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    led = json["led"];  
  }
  digitalWrite(LED_PIN, led);

}
void dhtEvent(){
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  json["temperature"] = temperature;
  json["humidity"] = humidity;
  serializeJson(json, mySerial);
  mySerial.println();
  serializeJson(json, Serial);
  Serial.println();
}