import logging
import signal
import os
import json
import datetime
import socket

import paho.mqtt.client as mqtt
import influxdb_client

from influxdb_client.client.write_api import SYNCHRONOUS
from time import sleep

ENV_DEBUG_DATA_FLOW = os.environ.get("DEBUG_DATA_FLOW")
ENV_INFLUX_BUCKET   = os.environ.get("INFLUX_BUCKET")
ENV_INFLUX_ORG      = os.environ.get("INFLUX_ORG")
ENV_INFLUX_TOKEN    = os.environ.get("INFLUX_TOKEN")
INFLUX_URL          = "http://sprc_influx:8086"
BROKER_CONN_FLAG    = False
BROKER_CONN_TIMEOUT = 60

tsdb_client = influxdb_client.InfluxDBClient(
   url=INFLUX_URL,
   token=ENV_INFLUX_TOKEN,
   org=ENV_INFLUX_ORG,
)

write_api = tsdb_client.write_api(write_options=SYNCHRONOUS)
query_api = tsdb_client.query_api()

def create_point(data, topic):
    topic_split = topic.split('/')
    location    = topic_split[0]
    station     = topic_split[1]
    timestamp   = data.get('timestamp', datetime.datetime.now().isoformat())
    flattened   = [(k, v) for k, v in data.items()]

    valid_data  = list(filter(lambda x: (
        x[0] != "timestamp" and \
        x[0] != "status" and \
        not isinstance(x[1], str)
        ), flattened
    ))

    point = influxdb_client.Point("data") \
        .tag("location", location) \
        .tag("station", station) \
        .time(timestamp)

    logging.info("The valid fields to be added to the data point: ")
    for kv in valid_data:
        point = point.field(kv[0], kv[1])

    return point

def on_connect(client, userdata, flags, rc):
    logging.info("Connected to broker. Result code: " + str(rc))
    client.subscribe("#")


def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    payload_json = json.loads(payload)

    logging.info('New message on topic ' + msg.topic + ' with payload: ')
    logging.info(json.dumps(payload_json, indent=4, sort_keys=True))

    point= create_point(payload_json, msg.topic)

    logging.info("Created point! Writing point to influx!")
    write_api.write(bucket=ENV_INFLUX_BUCKET, org=ENV_INFLUX_ORG, record=point)

def terminate(signalNumber, frame):
    logging.warning('Received SIGTERM: ' + str(signalNumber))
    logging.warning('Gracefully terminating the process...')
    exit(0)

def init_config():
    if ENV_DEBUG_DATA_FLOW == "false":
        logger = logging.getLogger()
        logger.disabled = True
    else:
        logging.basicConfig(format='%(asctime)s - %(message)s', level=logging.INFO)
        logging.basicConfig(format='%(asctime)s - %(message)s', level=logging.WARNING)

    signal.signal(signal.SIGTERM, terminate)

if __name__ == '__main__':
    init_config()

    health = tsdb_client.health()
    while health.status != "pass":
        logging.info("Influx connection is unhealthy. Waiting 2 seconds...")
        sleep(2)
        health = tsdb_client.health()

    logging.info("Influx connection is healthy.")

    mqtt_client = mqtt.Client('adapter_client')
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    while BROKER_CONN_FLAG == False and BROKER_CONN_TIMEOUT > 0:
        try:
            mqtt_client.connect("sprc_broker", 1883)
            BROKER_CONN_FLAG = True

        except socket.gaierror as e:
            logging.info("Broker connection is unhealthy. Waiting 5 seconds...")
            sleep(5)
            BROKER_CONN_TIMEOUT -= 5

    mqtt_client.loop_start()

    while True:
        sleep(5)
        logging.info("Broker connection is healthy.")
