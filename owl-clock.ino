#include <ESP8266WiFi.h>
#include <microDS3231.h>
#include "GyverTM1637.h"
#include "GyverTimer.h"

MicroDS3231 rtc;
GyverTM1637 disp1(2,0); //clk, dio D3, D4
GyverTM1637 disp2(12,14); // D5, D6
GTimer myTimer(MS);

byte number[10] {_0, _1, _2, _3, _4, _5, _6, _7, _8, _9}; // 0 – 9
void disp();

void setup() {
  myTimer.setInterval(30000); myTimer.stop();
  WiFi.mode(WIFI_OFF); delay(500);
  sync();
}

void loop(){
  if (myTimer.isReady()){
    disp();
  }
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
