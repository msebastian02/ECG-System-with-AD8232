//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"      //Library for connection to Firebase
#include <ESP8266WiFi.h>          //Basic library for Wifi Module
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //WiFi Configuration Magic
#include <ESP8266TrueRandom.h>    //Generador de UUID
#include <Ticker.h>               //Interrupcion LED

#define FIREBASE_HOST "ecgdb-usta.firebaseio.com"
#define FIREBASE_AUTH "zt4UFnzC9BFW90mBkCDG7xqhAPsexRSHWUFxSAyq"
//#define WIFI_SSID "RED MUNAR"
//#define WIFI_PASSWORD "e2d70f620f6998cd10b8a27440"

void parpadeoLed(void);

Ticker ticker;
FirebaseData firebaseData;
FirebaseJson json;
byte pinLed = LED_BUILTIN;
String Uuid,Dato;
byte uuidNumber[16];
String path;
int contador = 0;
int Timestamp;
int bandera=0;

void setup()
{
  Serial.begin(9600);
  pinMode(pinLed, OUTPUT);
  ticker.attach(0.2, parpadeoLed);
  //WifiManager Config
  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  if(!wifiManager.autoConnect("ECGUSTA")){
    //Serial.println("Fallo en la conexión (timeout)");
    ESP.reset();
    delay(500);
  }
  //Serial.println("Ya estás conectado");
  //Serial.println(WiFi.localIP());
  //Ticker LED
  ticker.detach();
  digitalWrite(pinLed, HIGH);
  //Uuid Config
  ESP8266TrueRandom.uuid(uuidNumber);
  Uuid=ESP8266TrueRandom.uuidToString(uuidNumber);
  //Firebase Config
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  firebaseData.setBSSLBufferSize(4096, 4096);
  firebaseData.setResponseSize(4096);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "large");
  path = "/Data/"+Uuid;

  if (Firebase.setTimestamp(firebaseData, path+"/DateTime"))
  {
    //Serial.println("PASSED");
    Timestamp = firebaseData.intData();
  }
  else
  {
    //Serial.println("FAILED");
    //Serial.println("REASON: " + firebaseData.errorReason());
  }  
}

void loop()
{
  if(bandera==0)
  {
    Serial.println(Timestamp);
    delay(1000);
  }

  if(Serial.available()>0){
     while (Serial.available()) {
        bandera=1;
        Dato = Serial.readString();
        int longitud = Dato.length();
        if(longitud==2016){
          contador ++;
          //longitud = longitud - 2;
          //Dato.remove(longitud);
          if (Firebase.setString(firebaseData, path+"/Samples"+contador, Dato))
          {
            Serial.println("PASSED");
          }
          else
          {
            Serial.println("FAILED");
            Serial.println("REASON: " + firebaseData.errorReason());
          }
        }
     }
  }
}

void parpadeoLed(){
  byte estado = digitalRead(pinLed);
  digitalWrite(pinLed, !estado);
}
