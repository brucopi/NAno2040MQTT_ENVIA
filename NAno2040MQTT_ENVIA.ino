##################MQTT envia 

#include <MQTT.h>
#include <MQTTClient.h>
#include <WiFiNINA.h>


char ssid[] = "";        // your network SSID (name)
char pass[] = "";
int status = WL_IDLE_STATUS;     // the Wifi radio's status
  
WiFiClient wifiClient;
MQTTClient mqttClient;

#define BROKER_IP    "test.mosquitto.org"
#define DEV_NAME     "mqttdevice1"
#define MQTT_USER    "mqtt_user1"
#define MQTT_PW      "mqtt_password1"
#define TOPIC_SUBSCRIBE "FIT_T1"   //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
 const char topic[]  = "FIT_T1";
#define pinBotao1 3  //D3 na placa

unsigned long lastMillis = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinBotao1, INPUT_PULLUP);
   Serial.begin(9600);
    while (!Serial);
   conectawifi();
    unsigned long lastMillis = 0;
      // MQTT brokers usually use port 8883 for secure connections.
    mqttClient.begin(BROKER_IP, 1883, wifiClient);
    
    conectamqtt();  
}

void loop() {
  // put your main code here, to run repeatedly:
   mqttClient.loop();
    if (!mqttClient.connected()) {
      conectawifi();
      conectamqtt();
     
      printData();
      
    } 
     if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    enviaValores();
     }
      
}

 void conectawifi()
  {
     // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to network: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network:
      status = WiFi.begin(ssid, pass);
  
      // wait 10 seconds for connection:
      delay(10000);
    }
  
    // you're connected now, so print out the data:
    Serial.println("You're connected to the network");
    
    Serial.println("----------------------------------------");
    printData();
    Serial.println("----------------------------------------");
  }

  void printData() {
    Serial.println("Board Information:");
    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
  
    Serial.println();
    Serial.println("Network Information:");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
  
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);
  
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
  }

 void conectamqtt()
  {
    Serial.print("\nconnectando MQTT...");
    while (!mqttClient.connect(DEV_NAME));//, MQTT_USER, MQTT_PW)) 
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("\nMQTT connected!");
    Serial.print("BROKER_IP=");
    Serial.println(BROKER_IP);
    Serial.print("DEV_NAM=");
    Serial.println(DEV_NAME);
    Serial.print("MQTT_USER=");
    Serial.println(MQTT_USER);
    Serial.print("MQTT_PW=");
    Serial.println(MQTT_PW);
    
  }


  void enviaValores() {
  static bool estadoBotao1 = HIGH;
  static bool estadoBotao1Ant = HIGH;
  static unsigned long debounceBotao1;

  estadoBotao1 = digitalRead(pinBotao1);

  if (  (millis() - debounceBotao1) > 30 ) {  //Elimina efeito Bouncing
    if (!estadoBotao1 && estadoBotao1Ant) {

      //Botao Apertado  
      mqttClient.publish(topic,"1");
      Serial.println("Botao1 APERTADO. Payload enviado.");

      debounceBotao1 = millis();
    } else if (estadoBotao1 && !estadoBotao1Ant) {

      //Botao Solto
      mqttClient.publish(topic,"0");
      Serial.println("Botao1 SOLTO. Payload enviado.");

      debounceBotao1 = millis();
    }

  }
  estadoBotao1Ant = estadoBotao1;
}
