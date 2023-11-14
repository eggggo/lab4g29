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

hw_timer_t* timer = NULL;

//m1, m2 encoder derived velocities in terms of ledc duty cycle
int m1_speed = 0, m2_speed = 0;

float p, i, d;

//track desired state globally
bool des_stopped = true;

void handleRoot(){
  h.sendhtml(body);
}

int feedback_control(int desired, int current) {
  //TODO use m1_speed, m2_speed and tgt to adjust ledc cycle
  return desired;
}

void chp() {
  p = h.getVal()/10.f;
  String res = "P: " + String(p);
  h.sendplain(res);
}

void chi() {
  i = h.getVal()/10.f;
  String res = "I: " + String(i);
  h.sendplain(res);
}

void chd() {
  d = h.getVal()/10.f;
  String res = "D: " + String(d);
  h.sendplain(res);
}

void straight(int dir) {
  //val param 0 forwards, 1 backwards
  digitalWrite(M1_H1_PIN, 1-dir);
  digitalWrite(M1_H2_PIN, dir);
  digitalWrite(M2_H1_PIN, 1-dir);
  digitalWrite(M2_H2_PIN, dir);

  //set duty cycle speeds from feedback control
  ledcWrite(0, feedback_control(M_ON_TGT, m1_speed));
  ledcWrite(1, feedback_control(M_ON_TGT, m2_speed));
}

void stop() {
  ledcWrite(0, 0);
  ledcWrite(1, 0);
}

void IRAM_ATTR onTimer() {
  h.serve();
  //printing in ints bad but im lazy
  int tgt = des_stopped ? 0 : M_ON_TGT;
  Serial.print("Target: ");
  Serial.print(tgt);
  Serial.print(", Motor 1: ");
  Serial.print(m1_speed);
  Serial.print(", Motor 2: ");
  Serial.println(m2_speed);
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
  h.attachHandler("/p?val=", chp);
  h.attachHandler("/i?val=",chi);
  h.attachHandler("/d?val=", chd);

  //timer for page serve
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, false);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
}

// the loop function runs over and over again forever
void loop() {
  //TODO read and translate encoder vals to m1_speed, m2_speed

  //forward for 5s, stop 1s, backwards for 5s, stop 1s
  straight(0);
  des_stopped = false;
  delay(5000);
  stop();
  des_stopped = true;
  delay(1000);
  straight(1);
  des_stopped = false;
  delay(5000);
  stop();
  des_stopped = true;
  delay(1000);
}
