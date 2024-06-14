# API_Flask
Esta API, desenvolvida em Python utilizando o Flask, recebe dados via MQTT da ESP32 e os armazena em um banco de dados MySQL local. Foi projetada com o propósito de ser utilizada em um carrinho seguidor de linha para coletar dados durante uma corrida.

# Requisitos

```bash
$ pip install Flask Flask-SQLAlchemy 
$ pip install mysql-connector-python
$ pip install Flask-MQTT
$ pip install requests
$ sudo apt-get install mosquitto-clients
```
