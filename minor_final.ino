/* 
 *  importing library to be used
 */
#include <iostream>
#include <queue>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>
#include <DHT.h>
#include <Wire.h>
#include <Arduino.h>
//#include <DS3231.h>

using namespace std;

/*
 * DHT Sensor Detail and define
 */
 #define DHTPIN D4
 #define DHTTYPE DHT11
 DHT dht(DHTPIN, DHTTYPE);

/*
 * Network configuration
 */
 const char* ssid = "OPPO A33f"; // ssid of network
 const char* password = "12345678"; // password for network

/*
 * Use of RTC module DS3231
 */


 /*
  * Define other used pin 
  */
  #define motorPin D5
  #define buttonPin D6
  #define soilPin A0

  /*
   * define other changing variable
   */
   int soilValue;
   int limit = 300;
   int i = 0;
   int lastButtonState = LOW;
  int motorValue;

/*
 * Start Webserver 
 */
 AsyncWebServer server(80);

 /*
  * Attach ESP-DASH to server
  */
  ESPDash dashboard(&server);

  /*
   * Dashboard component 
   */
Card card1(&dashboard, BUTTON_CARD, "Motor Button");
Card card2(&dashboard, STATUS_CARD,"Motor State", "idle");
Card temperature(&dashboard, TEMPERATURE_CARD, "Temperature", "°C");
Card humidity(&dashboard, HUMIDITY_CARD, "Humidity", "%");
Chart chart1(&dashboard, BAR_CHART, "Humidity-chart");
Chart chart2(&dashboard, BAR_CHART, "temperature-chart");

/*
 * Queue define
 */
queue<float> humidityQueue;
queue<float> temperatureQueue;
float humidityArr[30];
float temperatureArr[30];

void setup() {
  // put your setup code here, to run once:
  /*
   * start serial monitor
   */
   Serial.begin(115200);

   /*
    * RTC_modeule Code
    */

    /*
     * Connect to WiFi
     */
    // WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFi Failed!\n");
      return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  //Serial.print("web Adrees: ");
  //Serial.print("http://");
  //Serial.println(WiFi.localIP());
  
/*
 * Start AsyncWebServer
 */
 server.begin();
/*
 * various Pin setup
 */
pinMode(motorPin, OUTPUT);
digitalWrite(motorPin, lastButtonState);
pinMode(buttonPin, INPUT);

// add soil Sensor
pinMode(soilPin, INPUT);

/*
 * DHT11 setup and start
 */
 dht.begin();
}

/*
 * Update Motorstate if ButtonState is changes
 */
void motor_on(){
  lastButtonState = HIGH;
  digitalWrite(motorPin, lastButtonState);
}
void motor_off(){
  lastButtonState = LOW;
  digitalWrite(motorPin, lastButtonState);
}

/*
 * change queue to array
 */
 void change_to_array(){
    int j = 0;
    queue<float> temp_h = humidityQueue;
    queue<float> temp_t = temperatureQueue;
    while(j< 30){
      humidityArr[j] = temp_h.front();
      temp_h.pop();
      temperatureArr[j] = temp_t.front();
      temp_t.pop();
      j = j+1;
 }}

void loop(){
  // put your main code here, to run repeatedly:
  /*
   * read various values
   */
   //soilValue = analogRead(soilPin);
   //int motorValue = digitalRead(buttonPin);
   //float t = dht.readTemperature();
   //float h = dht.readHumidity();
   // float date 
   // DateTime now = myRTC.now();
   float t = (int)random(0,50);
   float h = (int)random(0,100);
   
   if (i <10){
    soilValue = 100;
    motorValue = HIGH;
   }
   if (i> 10){
    soilValue = 400;
    motorValue = LOW;
   }
   if (i < 30){
    humidityQueue.push(h);
    temperatureQueue.push(t);
    i++;
   }
   if (i >= 30){
    humidityQueue.pop();
    humidityQueue.push(h);
    temperatureQueue.pop();
    temperatureQueue.push(t);
   }

  temperature.update(t);
  humidity.update(h);
  card1.attachCallback([&](bool value){
  Serial.println("[Card1] Button Callback Triggered: "+String((value)?"true":"false"));
   if (value == false && lastButtonState == LOW && soilValue >= limit && motorValue == LOW){
    motor_off();
    card2.update("Idle", "idle");
  }
  else if (value == false && lastButtonState == LOW && soilValue >= limit && motorValue == HIGH){
    motor_on();
    card2.update("Warning", "warning");
  }
  else if (value == false && lastButtonState == LOW && soilValue < limit && motorValue == LOW){
    motor_on();
    card2.update("Working", "success");
  }
  else if (value == false && lastButtonState == LOW && soilValue < limit && motorValue == HIGH){
    motor_on();
    card2.update("Working", "success");
  }else if (value == false && lastButtonState == HIGH && soilValue >= limit && motorValue == LOW){
    motor_off();
    card2.update("Idle", "idle");
  }
  else if (value == false && lastButtonState == HIGH && soilValue >= limit && motorValue == HIGH){
    motor_on();
    card2.update("Warning", "warning");
  }
  else if (value == false && lastButtonState == HIGH && soilValue < limit && motorValue == LOW){
    motor_on();
    card2.update("Working", "success");
  }
  else if (value == false && lastButtonState == HIGH && soilValue < limit && motorValue == HIGH){
    motor_on();
    card2.update("Worko=ing", "success");
  }
  else if (value == true && lastButtonState == LOW && soilValue >= limit && motorValue == LOW){
    motor_off();
    card2.update("Not Working", "danger");
  }
  else if (value == true && lastButtonState == LOW && soilValue >= limit && motorValue ==HIGH){
    motor_on();
    card2.update("Warning", "warning");
  }
  else if (value == true && lastButtonState == LOW && soilValue < limit && motorValue == LOW){
    motor_on();
    card2.update("Working", "success");
  }
  else if (value == true && lastButtonState == LOW && soilValue < limit && motorValue == HIGH){
    motor_on();
    card2.update("Working", "success");
  }
  else if (value == true && lastButtonState == HIGH && soilValue >= limit && motorValue == LOW){
    motor_off();
    card2.update("Danger", "danger");
  }
  else if (value == true && lastButtonState == HIGH && soilValue >= limit && motorValue == HIGH){
    motor_on();
    card2.update("Warning", "warning");
  }
  else if (value == true && lastButtonState == HIGH && soilValue < limit && motorValue == LOW){
    motor_on();
    card2.update("warning", "warning");
  }
  else if (value == true && lastButtonState == HIGH && soilValue < limit && motorValue == HIGH){
    motor_on();
    card2.update("Working", "success");
  }
  card1.update(value);
 });

 // change queue to array
 change_to_array();
 
  int XAxis[30] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
 chart1.updateX(XAxis, 30);
 chart1.updateY(humidityArr, 30);

 chart2.updateX(XAxis, 30);
 chart2.updateY(temperatureArr, 30);

 dashboard.sendUpdates();

 delay(3000);

}
