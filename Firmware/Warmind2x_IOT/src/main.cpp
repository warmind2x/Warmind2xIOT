#include <Arduino.h>
#include <Colors.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <IoTicosSplitter.h>
#include <OneWire.h>
#include <DS18B20.h>

//PINS
#define  ds18B20 4

OneWire oneWire(ds18B20);
DS18B20 sensor(&oneWire);




//Global Vars
WiFiClient  espclient;
PubSubClient client(espclient);
long lastReconnectAttemp = 0;
const char* mqtt_server = "192.168.1.101";
DynamicJsonDocument mqtt_data_doc(2048);



//WiFi
const char* wifi_ssid = "Ninoska";
const char* wifi_password = "ninoska091188";

//MQTT

const char *username = "warmind2x";
const char *password = "1162@Jdgb";


//FUNTION DEF
void clear();
void check_mqtt_connection();
bool reconnect();
void wifi_connect();
int get_temp();

void setup() {
  
Serial.begin(921600);
clear();
wifi_connect();
client.setServer(mqtt_server, 1883);



  

}

void loop() {
  check_mqtt_connection();
  get_temp();
}

void clear(){
  Serial.write(27);
  Serial.print("[2J");
  Serial.write(27);
  Serial.print("[H");
}

void check_mqtt_connection(){
  
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttemp > 5000) {
      lastReconnectAttemp = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttemp = 0;
      }
    }
  } else {
    // Client connected

    client.loop();
  }

}

boolean reconnect() {
  Serial.print(underlinePurple + "\n\n\nTrying MQTT Connection" + fontReset + Purple + "  ⤵");
  if (client.connect("arduinoClient",username,password)) {
    Serial.print(boldGreen + "\n\n         Mqtt Client Connected :) " + fontReset);
    delay(2000);
    // Once connected, publish an announcement...
    client.publish("outTopic","hello world");
    // ... and resubscribe
    client.subscribe("inTopic");
  }
  return client.connected();
}

void wifi_connect(){
  WiFi.begin(wifi_ssid, wifi_password);
  
  Serial.print(underlinePurple + "\n\n\n Conectando a la red " + wifi_ssid + fontReset + Purple);

  int counter = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    counter++;

    if (counter > 10)
    {
      Serial.print("  ⤵" + fontReset);
      Serial.print(Red + "\n\n         Ups WiFi Connection Failed :( ");
      Serial.println(" -> Restarting..." + fontReset);
      delay(2000);
      ESP.restart();
    }
  }

  Serial.print("  ⤵" + fontReset);

  //Printing local ip
  Serial.println(boldGreen + "\n\n         WiFi Connection -> SUCCESS :)" + fontReset);
  Serial.print("\n         Local IP -> ");
  Serial.print(boldBlue);
  Serial.print(WiFi.localIP());
  Serial.println(fontReset);
}

int get_temp(){
  sensor.requestTemperatures();

  while (!sensor.isConversionComplete());  // wait until sensor is ready

  Serial.print("Temp: ");
  Serial.println(sensor.getTempC());

  return sensor.getTempC();
}