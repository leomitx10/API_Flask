from datetime import datetime
from flask import Flask, Response, request
from flask_sqlalchemy import SQLAlchemy
import mysql.connector
import json
from flask import Flask, request, jsonify
from flask_mqtt import Mqtt
import requests

#RODAR = flask run

app = Flask(__name__)

app.config['MQTT_BROKER_URL'] = 'broker.emqx.io'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = ''  # Set this item when you need to verify username and password
app.config['MQTT_PASSWORD'] = ''  # Set this item when you need to verify username and password
app.config['MQTT_KEEPALIVE'] = 5  # Set KeepAlive time in seconds
app.config['MQTT_TLS_ENABLED'] = False  # If your broker supports TLS, set it True
topic = '/flaskk/mqtt'

mqtt_client = Mqtt(app)


@mqtt_client.on_connect()
def handle_connect(client, userdata, flags, rc):
   if rc == 0:
       print('Connected successfully')
       mqtt_client.subscribe(topic) # subscribe topic
   else:
       print('Bad connection. Code:', rc)


@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
   data = dict(
       topic=message.topic,
       payload=message.payload.decode()
  )
   print('Received message on topic: {topic} with payload: {payload}'.format(**data))


app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = True
app.config['SQLALCHEMY_DATABASE_URI'] = 'mysql://root:Senha%40123@localhost/rastrobotdb2'
db = SQLAlchemy(app)

@app.route('/')
def index():
    return '<h1>Hello, World!<h1>'

class ESP32Data(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    sensor_esquerdo = db.Column(db.String(100))
    sensor_direito = db.Column(db.String(100))
    velocidade = db.Column(db.Float, default=0.0)
    distancia = db.Column(db.Float, default=0.0)
    timestamp = db.Column(db.DateTime, default=datetime.utcnow)

    def to_json(self):
        return {"id": self.id, "sensor_esquerdo": self.sensor_esquerdo, 
                "sensor_direito": self.sensor_direito, "velocidade": self.velocidade,
                "distancia": self.distancia, "timestamp": self.timestamp.strftime('%Y-%m-%d %H:%M:%S')}

with app.app_context():
    # Criar tabelas
    db.create_all()

# Callback para quando uma mensagem é recebida no tópico subscrito
@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(
        topic=message.topic,
        payload=message.payload.decode()
    )
    print('Mensagem recebida no tópico: {topic} com o payload: {payload}'.format(**data))
    
    # Transformar os dados recebidos em um objeto JSON
    json_data = json.loads(data['payload'])
    
    # Fazer uma requisição POST para o endpoint /dadoscad
    try:
        response = requests.post('http://localhost:5000/dadoscad', json=json_data)
        if response.status_code == 201:
            print("Dados inseridos com sucesso no banco de dados.")
        else:
            print("Erro ao inserir dados no banco de dados:", response.text)
    except Exception as e:
        print("Erro ao fazer requisição POST para /dadoscad:", e)

# Cadastrar
@app.route("/dadoscad", methods=["POST"])
def cria_dados():
    body = request.get_json()

    try:
        timestamp = datetime.strptime(body["timestamp"], '%Y-%m-%d %H:%M:%S')

        dados = ESP32Data(sensor_esquerdo=body["sensor_esquerdo"], sensor_direito = body["sensor_direito"],
        velocidade = body["velocidade"], distancia = body["distancia"], timestamp = timestamp)
        db.session.add(dados)
        db.session.commit()
        return gera_response(201, "dados", dados.to_json(), "Criado com sucesso")
    
    except Exception as e:
        print('Erro', e)
        return gera_response(400, "dados", {}, "Erro ao cadastrar")
    

def gera_response(status, nome_do_conteudo, conteudo, mensagem=False):
    body = {}
    body[nome_do_conteudo] = conteudo

    if(mensagem):
        body["mensagem"] = mensagem

    return Response(json.dumps(body), status=status, mimetype="application/json")

if __name__ == '__main__':
    app.run()


# EXEMPLO DE CODIGO PARA TESTAR O MQTT
# mosquitto_pub -h broker.emqx.io -t "/flaskk/mqtt" -m '{"sensor_esquerdo": 1, "sensor_direito": 1, "velocidade": 10, "distancia": 20, "timestamp": "2024-06-15 11:30:00"}'


