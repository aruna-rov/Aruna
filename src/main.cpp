#include <Arduino.h>
#include <WiFi.h>

const char* ssid     = "smoeskopjes";
const char* password = "nope";

void setup(){
    Serial.begin(115200);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop(){
    Serial.println("Hello world");
    delay(5000);
}