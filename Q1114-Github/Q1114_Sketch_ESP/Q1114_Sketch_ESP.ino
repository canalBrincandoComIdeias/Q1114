#include <WiFi.h>
#include <MQTT.h>
#include "user.h"  //Informar a rede WiFi, a senha, usuário MQTT e a senha

//const char ssid[] = "xxx";
//const char pass[] = "xxx";

//Definições
#define MEU_DISPOSITIVO "bi_q1113"
#define TOPICO_ALERTA "bi_q1110/alerta"

//Servidor MQTT
//const char servidor[] = "54.233.221.233";
const char servidor[] = "mqtt.monttudo.com";  //(caso o IP não funcione, use a identificação pelo domínio)

//Usuário MQTT
const char deviceName[] = MEU_DISPOSITIVO;
const char mqttUser[] = "todos";
const char mqttPass[] = "cortesi@BrincandoComIdeias";

//Pinos
#define pinLEDPlaca 8  
#define pinSensor 4    
#define pinRele 21     

#define NIVEL_RELE_LIGADO HIGH

//Criação dos Objetos
WiFiClient net;
MQTTClient client;

//Variáveis Globais
bool estado = false;
bool estadoAnt = false;

//Função auxiliar para conectar no WiFi e Servidor MQTT
void connect() {
  Serial.print("->wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(";");

  Serial.print("->mqtt...");
  while (!client.connect(deviceName, mqttUser, mqttPass)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(";");
  Serial.println("->conectado;");
}

//Processamento das mensagens recebidas do Servidor MQTT
void messageReceived(String &topic, String &payload) {
  Serial.println(topic + ":" + payload);

  //if (topic == TOPICO_ALERTA) {
  //  estado = payload.toInt();
  //}

  // Nota: Não use o "client" nessa função para publicar, subscrever ou
  // dessubscrever porque pode causar problemas quando outras mensagens são recebidas
  // enquanto são feitos envios ou recebimentos. Então, altere uma variavel global,
  // coloque numa fila e trate essa fila no loop depois de chamar `client.loop()`.
}

void setup() {
  pinMode(pinLEDPlaca, OUTPUT);
  pinMode(pinSensor, INPUT);
  pinMode(pinRele, OUTPUT);

  digitalWrite(pinLEDPlaca, LOW);
  digitalWrite(pinRele, !NIVEL_RELE_LIGADO);

  Serial.begin(9600);
  delay(2000);  //Espera para iniciar o serial e o módulo HLK-LD2420, que também precisa de um tempo para iniciar.
  Serial.println("Sketch Iniciado...");

  WiFi.begin(ssid, pass);

  // Observação: Nomes de domínio locais (por exemplo, "Computer.local" no OSX)
  // não são suportados pelo Arduino. Você precisa definir o endereço IP diretamente.
  client.begin(servidor, net);
  client.onMessage(messageReceived);

  connect();
  digitalWrite(pinLEDPlaca, HIGH);
}

void loop() {
  client.loop();
  delay(10);  //corrige alguns problemas com a estabilidade do WiFi

  if (!client.connected()) {
    digitalWrite(pinLEDPlaca, LOW);
    connect();
    client.subscribe(TOPICO_ALERTA);
    digitalWrite(pinLEDPlaca, HIGH);
  }


  estado = digitalRead(pinSensor);

  if (estado != estadoAnt) {
    if (estado) {
      Serial.println("Pessoa Detectada!");
      digitalWrite(pinRele, NIVEL_RELE_LIGADO);
      client.publish(TOPICO_ALERTA, "1");
    } else {
      Serial.println("ausente");
      digitalWrite(pinRele, !NIVEL_RELE_LIGADO);
      client.publish(TOPICO_ALERTA, "0");
    }
  }

  estadoAnt = estado;
  delay(20);
}
