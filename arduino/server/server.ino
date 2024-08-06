#include <ESP8266WiFi.h>
#include <HttpClient.h>

#include "settings.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

// Get settings from settings file
const char* ssid1 = STASSID1;
const char* password1 = STAPSK1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  String thisBoard= ARDUINO_BOARD;
  Serial.println(thisBoard);

   WiFi.mode(WIFI_STA); // Sets wifi to station mode
  WiFi.enableInsecureWEP();
  WiFi.begin(ssid1, password1);

  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void FailConnect(){
  Serial.println("Connection failed");
  blinkTimes(10, 250);
}

bool WiFiConnect(){
  Serial.print("Wait for WiFi... ");
  Serial.print("WiFi.status(): ");
  Serial.print(WiFi.status());

  while (WiFi.status() != WL_CONNECTED) {
      WiFi.printDiag(Serial);
      delay(3500);
    }
    blinkTimes(3, 250);
    Serial.println("");
    if(WiFi.localIP().toString().indexOf("169") != -1){
      Serial.println("WiFi Not connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      return false;
    } else{
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      return true;
    }
}

void WiFiClientPost(int currentState){
  if(WiFiConnect()){
    Serial.println("WiFiClientPost");
    if (client.connect(host, 54322)) {
      prevState = currentState;

      client.println(HTTP_METHOD + " " + PATH_NAME + " HTTP/1.1");
      client.println("Host: " + String(host));
      client.println("Connection: close");
      client.println();  // end HTTP header

      // send HTTP body
      client.println("sensor_id=" + sensorid + 
                    "&message=" + message + 
                    "&token=" + token);

      client.println("Content-Type: application/x-www-form-urlencoded");


      while(client.connected()){
        if(client.available()){
          char c = client.read();
          Serial.print(c);
        }
      }
      Serial.println("request sent");
      client.stop();
      Serial.println("disconnected");
      blinkTimes(5, 250);
      delay(TIME_TO_WAIT_AFTER_TRIGGER); 
    } else{
      FailConnect();
      return;
    }
  }else{
    while(true){
      blinkTimes(10, 250);
      delay(1000);
    }
  }
}

void HttpClientPost(int currentState){
  if(WiFiConnect()){
    prevState = currentState;
    Serial.println("HttpClientPost");
    HttpClient http;

    http.begin(client, "http://" + host + ":" + port + PATH_NAME);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.post("sensor_id=" + sensorid + 
                                     "&message=" + message + 
                                     "&token=" + token);

    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

  }else{
    while(true){
      blinkTimes(10, 250);
      delay(1000);
    }
  }
}