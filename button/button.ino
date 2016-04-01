#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <MicroGear.h>

const char* ssid     = <WIFI_SSID>;
const char* password = <WIFI_KEY>;

#define APPID   <APPID>
#define KEY     <APPKEY>
#define SECRET  <APPSECRET>

#define ALIAS   "button"

#define BUTTONPIN 13

WiFiClient client;
AuthClient *authclient;

int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

void pressed() {
  Serial.println("pressed");
  microgear.chat("display","1");
}

void released() {
  Serial.println("released");
  microgear.chat("display","0");
}

void setup() {
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    pinMode(BUTTONPIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BUTTONPIN), pressed, RISING);
    attachInterrupt(digitalPinToInterrupt(BUTTONPIN), released, FALLING);
    
    Serial.begin(115200);
    Serial.println("Starting...");

    if (WiFi.begin(ssid, password)) {

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);
}

void loop() {
    if (microgear.connected()) {
        microgear.loop();
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID);
            timer = 0;
        }
        else timer += 100;
    }
    delay(100);
}
