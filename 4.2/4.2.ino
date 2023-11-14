/*
Group 29
*/

#include <WiFi.h>
#include <WiFiUdp.h>
#include "html510.h"
#include "webpage.h"

//assumes m1 on left, m2 on right from back of bot
#define M1_PWM_PIN 5
#define M1_H1_PIN 10
#define M1_H2_PIN 1
#define M2_PWM_PIN 4
#define M2_H1_PIN 6
#define M2_H2_PIN 7

#define M1_ENC_PIN 18
#define M2_ENC_PIN 19

#define M_FREQ 2000
#define RES_BITS 10

//motor duty cycle target for full on
#define KP 10

const char* ssid = "TP-Link_E0C8";
const char* pass = "52665134";
IPAddress src_IP(192, 168, 0, 154);
HTML510Server h(80);

//m1, m2 encoder derived velocities in terms of ledc duty cycle
int m1_p = 0, m2_p = 0;
int m1_rise_time = 0, m2_rise_time = 0;
int m_tgt_ms = 0;

int m1_duty = 1023, m2_duty = 1023;
bool stopped = true;

void handleRoot(){
  h.sendhtml(body);
}

int feedback_control(int desired, int current) {
  //simple p controller on desired encoder tick period
  return (desired-current) * KP;
}

//stop
void stop() {
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  stopped = true;
}

void turn() {
  //m1 backwards, m2 forwards for left, vice versa for right
  //val param = 0 is left, 1 is right
  int dir = h.getVal();
  digitalWrite(M1_H1_PIN, 1-dir);
  digitalWrite(M1_H2_PIN, dir);
  digitalWrite(M2_H1_PIN, dir);
  digitalWrite(M2_H2_PIN, 1-dir);

  //set duty cycle speeds from feedback control
  m_tgt_ms = 15;
  m1_duty = 1023;
  m2_duty = 1023;
  ledcWrite(0, m1_duty);
  ledcWrite(1, m2_duty);
  stopped = false;
}

void straight() {
  //val param 0 forwards, 1 backwards
  int dir = h.getVal();
  digitalWrite(M1_H1_PIN, 1-dir);
  digitalWrite(M1_H2_PIN, dir);
  digitalWrite(M2_H1_PIN, 1-dir);
  digitalWrite(M2_H2_PIN, dir);

  //set duty cycle speeds from feedback control
  m_tgt_ms = 15;
  m1_duty = 1023;
  m2_duty = 1023;
  ledcWrite(0, m1_duty);
  ledcWrite(1, m2_duty);
  stopped = false;
}

//encoder periods per tick
void IRAM_ATTR handleM1Int() {
  if (digitalRead(M1_ENC_PIN)) m1_rise_time = millis();
  else m1_p = millis() - m1_rise_time;
}

void IRAM_ATTR handleM2Int() {
  if (digitalRead(M2_ENC_PIN)) m2_rise_time = millis();
  else m2_p = millis() - m2_rise_time;
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
  digitalWrite(2, HIGH);

  //encoder pins
  pinMode(M1_ENC_PIN, INPUT_PULLUP);
  pinMode(M2_ENC_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(M1_ENC_PIN), handleM1Int, CHANGE);
  attachInterrupt(digitalPinToInterrupt(M2_ENC_PIN), handleM2Int, CHANGE);

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

  //html page handlers(only called on transition states)
  h.begin();
  h.attachHandler("/ ",handleRoot);
  h.attachHandler("/straight?val=", straight);
  h.attachHandler("/turn?val=",turn);
  h.attachHandler("/stop", stop);
}

// the loop function runs over and over again forever
void loop() {
  //continuous feedback control if in motion
  if (!stopped) {
    int p1 = feedback_control(m_tgt_ms, m1_p);
    int p2 = feedback_control(m_tgt_ms, m2_p);
    //clamp values then write
    m1_duty = min(1023, max(m1_duty - p1, 0));
    m2_duty = min(1023, max(m2_duty - p2, 0));
    ledcWrite(0, m1_duty);
    ledcWrite(1, m2_duty);
  } else {
    ledcWrite(0, 0);
    ledcWrite(1, 0);
  }

  h.serve();
  delay(10);
}
