#include "ESP8266WiFi.h"
#include <PubSubClient.h>

//Parametros de conexão
const char* ssid     = "SFmall_2G";      // rede
const char* password = "@Sfmall2022";    // senha


// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org"; //"192.168.0.60";
const char *topic = "ACQUANATIVA/teste";
const char *mqtt_username = "";                  // Usuario
const char *mqtt_password = "";                  // Senha
const int mqtt_port = 1883;                      // Porta

//Variáveis
bool mqttStatus = 0;

//Objetos
WiFiClient espClient;
PubSubClient client(espClient);

//Prototipos
bool connectMQTT();
void callback(char *topic, byte * payload, unsigned int length);

void setup(void)
{
  Serial.begin(9600);

  // Conectar
  WiFi.begin(ssid, password);

  //Aguardando conexão
  Serial.println();
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  //Envia IP através da UART
  Serial.println(WiFi.localIP());

  mqttStatus =  connectMQTT();

}

void loop() {
 static long long pooling  = 0;
  if ( mqttStatus){
    
    client.loop(); 
    
    if (millis() > pooling +1000){
      pooling = millis();
      client.publish(topic, "{teste123,113007042022}");
    }
    
  }
}

bool connectMQTT() {
  byte tentativa = 0;
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  do {
    String client_id = "AcquaNativ-";
    client_id += String(WiFi.macAddress());

    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Exito na conexão:");
      Serial.printf("Cliente %s conectado ao broker\n", client_id.c_str());
    } else {
      Serial.print("Falha ao conectar: ");
      Serial.print(client.state());
      Serial.println();
      Serial.print("Tentativa: ");
      Serial.println(tentativa);
      delay(2000);
    }
    tentativa++;
  } while (!client.connected() && tentativa < 5);

  if (tentativa < 5) {
    // publish and subscribe   
    client.publish(topic, "{teste123,113007042022}"); 
    client.subscribe(topic);
    return 1;
  } else {
    Serial.println("Não conectado");    
    return 0;
  }
}

void callback(char *topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}