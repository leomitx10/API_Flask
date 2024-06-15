#include <WiFi.h>
#include <PubSubClient.h>

// Configurações do Wi-Fi
const char* ssid = "TESTE";
const char* password = "12345678";

// Configurações do MQTT
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic = "/flaskk/mqtt";

// Instâncias do cliente Wi-Fi e do cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop até que a conexão MQTT seja estabelecida
  while (!client.connected()) {
    Serial.print("Tentando se conectar ao MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leitura dos dados dos sensores
  int sensorEsquerdo = analogRead(34);  // Exemplo de leitura do sensor esquerdo
  int sensorDireito = analogRead(35);   // Exemplo de leitura do sensor direito
  float velocidade = 10.5;              // Exemplo de leitura da velocidade
  float distancia = 20.0;               // Exemplo de leitura da distância

  // Criação do payload JSON
  char payload[256];
  snprintf(payload, sizeof(payload), "{\"sensor_esquerdo\": %d, \"sensor_direito\": %d, \"velocidade\": %.2f, \"distancia\": %.2f, \"timestamp\": \"%s\"}", 
           sensorEsquerdo, sensorDireito, velocidade, distancia, getCurrentTime().c_str());

  Serial.print("Publicando payload: ");
  Serial.println(payload);

  client.publish(mqtt_topic, payload);

  delay(500);  // Meio segundo
}

String getCurrentTime() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  char buffer[20];
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", 
          p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday, 
          p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
  return String(buffer);
}
