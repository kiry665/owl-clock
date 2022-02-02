#include <ESP8266WiFi.h>
#include <microDS3231.h>
#include "GyverTM1637.h"
#include "GyverTimer.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

MicroDS3231 rtc;
GyverTM1637 disp1(2,0); //clk, dio D3, D4
GyverTM1637 disp2(12,14); // D5, D6
GTimer myTimer(MS);

byte number[10] {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9}; // 0 – 9
bool flag = false; bool work = true;

void setup() {
  myTimer.setInterval(30000); myTimer.stop();
  pinMode(13, INPUT_PULLUP);
  bool btn = !digitalRead(13);
  if (btn) work = false;
  if (work) {
    WiFi.mode(WIFI_OFF); delay(500);
    sync();
  }else OTAS();
}

void loop(){
  if (work){
    if (myTimer.isReady()){
      disp();
    }
    bool btn = !digitalRead(13);
    if (btn && !flag){
      flag = true;
      myTimer.stop();
      temp();
    }
    if (!btn && flag){
      flag = false;
    }
  }else OTAL();
}

void disp(){
  int l = map(analogRead(0), 0, 1000, 0, 7);
  int t = rtc.getTemperature();
  int h = rtc.getHours();
  int m = rtc.getMinutes();
  
  disp1.clear();
  disp1.brightness(l);
  disp1.displayByte(1, number[h / 10]);
  disp1.displayByte(0, number[h % 10]);
  
  disp2.clear();
  disp2.brightness(l);
  disp2.displayByte(1, number[m / 10]);
  disp2.displayByte(0, number[m % 10]);
}

void sync(){
  disp();
  int a = rtc.getMinutes(); int b = rtc.getMinutes();
  while (a == b) {
    a = rtc.getMinutes();
    delay(500);
  }
  disp();
  myTimer.start();
}

void temp(){
  int t = rtc.getTemperature();
  int l = map(analogRead(0), 0, 1000, 0, 7);

  disp1.clear();
  disp1.brightness(l);
  disp1.displayByte(1, number[t / 10]);
  disp1.displayByte(0, number[t % 10]);
  
  disp2.clear();
  disp2.brightness(l);
  disp2.displayByte(1, 0x63);
  disp2.displayByte(0, 0x39);

  delay(10000);
  sync();
}

void OTAS(){
  int l = map(analogRead(0), 0, 1000, 0, 7);
  
  disp1.clear();
  disp1.brightness(l);
  disp1.displayByte(1, 0x7c);
  disp1.displayByte(0, 0x5c);
  
  disp2.clear();
  disp2.brightness(l);
  disp2.displayByte(1, 0x5c);
  disp2.displayByte(0, 0x78);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin("ard", "nke321nke");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }
  
  ArduinoOTA.setPort(53);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
  });

  ArduinoOTA.begin();
}

void OTAL(){
  ArduinoOTA.handle();
}
