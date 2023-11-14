/*
Group 29
*/

#include <WiFi.h>
#include <WiFiUdp.h>
#include "html510.h"
#include "webpage.h"

//assumes m1 on left, m2 on right from back of bot
#define M1_PWM_PIN 6
#define M1_H1_PIN 18
#define M1_H2_PIN 19
#define M2_PWM_PIN -1
#define M2_H1_PIN -1
#define M2_H2_PIN -1
#define M_FREQ 2000
#define RES_BITS 10

//motor duty cycle target for full on
#define M_ON_TGT 1023

const char* ssid = "SSID_HERE";
const char* pass = "PASS_HERE";
IPAddress src_IP(192, 168, 1, 154);
HTML510Server h(80);

//m1, m2 encoder derived velocities in terms of ledc duty cycle
int m1_speed = 0, m2_speed = 0;

void handleRoot(){
  h.sendhtml(body);
}

int feedback_control(int desired, int current) {
  //TODO use m1_speed, m2_speed and tgt to adjust ledc cycle
  return desired;
}

void stop() {
  ledcWrite(0, 0);
  ledcWrite(1, 0);
}

void turn() {
  //m1 backwards, m2 forwards for left, vice versa for right
  //val param = 0 is left, 1 is right
  int dir = h.getVal();
  digitalWrite(M1_H1_PIN, dir);
  digitalWrite(M1_H2_PIN, 1-dir);
  digitalWrite(M2_H1_PIN, 1-dir);
  digitalWrite(M2_H2_PIN, dir);

  //set duty cycle speeds from feedback control
  ledcWrite(0, feedback_control(M_ON_TGT, m1_speed));
  ledcWrite(1, feedback_control(M_ON_TGT, m2_speed));
}

void straight() {
  //val param 0 forwards, 1 backwards
  int dir = h.getVal();
  digitalWrite(M1_H1_PIN, 1-dir);
  digitalWrite(M1_H2_PIN, dir);
  digitalWrite(M2_H1_PIN, 1-dir);
  digitalWrite(M2_H2_PIN, dir);

  //set duty cycle speeds from feedback control
  ledcWrite(0, feedback_control(M_ON_TGT, m1_speed));
  ledcWrite(1, feedback_control(M_ON_TGT, m2_speed));
}

void setup() {
  //setup ledc with channel 0 and 1, 2khz, 10 bits res for both motors
  ledcSetup(0, M_FREQ, RES_BITS);
  ledcAttachPin(M1_PWM_PIN, 0);
  ledcSetup(1, M_FREQ, RES_BITS);
  ledcAttachPin(M2_PWM_PIN, 0);
  //setup dir control pins for both motors
  pinMode(M1_H1_PIN, OUTPUT);
  pinMode(M1_H2_PIN, OUTPUT);
  pinMode(M2_H1_PIN, OUTPUT);
  pinMode(M2_H2_PIN, OUTPUT);

  //setup udp send/receive
  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.config(src_IP, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("Use this URL to connect: http://");
  Serial.print(WiFi.localIP()); Serial.println("/");

  //html page handlers
  h.begin();
  h.attachHandler("/ ",handleRoot);
  h.attachHandler("/straight?val=", straight);
  h.attachHandler("/turn?val=",turn);
}

// the loop function runs over and over again forever
void loop() {
  //TODO read and translate encoder vals to m1_speed, m2_speed
  
  //write mapped res directly to ledc channel
  h.serve();
  delay(10);
}
