#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Definições dos pinos dos motores
#define MOTOR_ESQUERDO_PIN_A 2  // Pino A do motor esquerdo
#define MOTOR_ESQUERDO_PIN_B 3  // Pino B do motor esquerdo
#define MOTOR_DIREITO_PIN_A 4   // Pino A do motor direito
#define MOTOR_DIREITO_PIN_B 5   // Pino B do motor direito

// WiFi credentials
const char* ssid = "NOME DA REDE WIFI";
const char* password = "SENHA DA REDE WIFI";

// MQTT broker
const char* mqtt_broker = "broker.emqx.io";
const char* topic = "/flaskk/mqtt";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Função de inicialização
void setup() {
  pinMode(MOTOR_ESQUERDO_PIN_A, OUTPUT);
  pinMode(MOTOR_ESQUERDO_PIN_B, OUTPUT);
  pinMode(MOTOR_DIREITO_PIN_A, OUTPUT);
  pinMode(MOTOR_DIREITO_PIN_B, OUTPUT);
  
  Serial.begin(9600);

  // Inicializa a conexão Wi-Fi
  Serial.print("Conectando ao WiFi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" conectado");

  // Configura o MQTT
  client.setServer(mqtt_broker, mqtt_port);

  // Conecta ao broker MQTT
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado ao MQTT!");
    } else {
      Serial.print("Falha na conexão ao MQTT. Estado: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

// Função principal
void loop() {
  // Gera dados simulados
  int sensorEsquerdo = random(2);
  int sensorDireito = random(2);
  float velocidade_inst = random(50, 150); // Velocidade entre 50 e 150 unidades por segundo
  float distancia_parcial = random(1000); // Distância entre 0 e 1000 unidades

  // Cria o JSON com os dados simulados
  String jsonData = String("{\"sensor_esquerdo\": ") + sensorEsquerdo + 
                    String(", \"sensor_direito\": ") + sensorDireito + 
                    String(", \"velocidade\": ") + velocidade_inst + 
                    String(", \"distancia\": ") + distancia_parcial + 
                    String(", \"timestamp\": \"") + getTimestamp() + String("\"}");

  // Publica os dados no tópico MQTT
  client.publish(topic, jsonData.c_str());
  delay(1000); // Espera um segundo antes de enviar os dados novamente
}

// Função para obter o timestamp atual
String getTimestamp() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Falha ao obter o tempo";
  }
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timestamp);
}
