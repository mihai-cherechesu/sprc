#!/bin/bash
RED="\u001b[31m"
GREEN="\u001b[32m"
YELLOW="\u001b[33m"

docker swarm init 2> /dev/null

rid=$(openssl rand -hex 20)

# Deploy the registry
docker run -d \
    -p 5000:5000 \
    --name "registry.${rid}" \
    --rm \
    -v "$(pwd)"/registry/data:/var/lib/registry \
    registry:2

while [ "$(docker ps | grep registry.${rid})" == "" ]; do
    printf "${YELLOW} Waiting for registry to start...\n"
    sleep 2
done

influx_init_pass=$(openssl rand -base64 20)
influx_init_token=$(openssl rand -base64 32)
grafana_root_pass=$(openssl rand -base64 20)

printf "${YELLOW} [INFO]: Generating secrets for the services...\n"
printf "${YELLOW} [INFO]: The reason for printing the secrets to stdout is only to get easier access.\n"
printf "${RED} [WARN]: Should disable this in production and access the containers from your private network to get the secrets instead!\n"
printf "${GREEN} [DONE]: Grafana dashboard pass: ${grafana_root_pass} \n"
printf "${GREEN} [DONE]: Influx connection token: ${influx_init_token} \n\n"

echo $influx_init_pass | docker secret create influx_init_pass - > /dev/null 2>&1
echo $influx_init_token | docker secret create influx_init_token - > /dev/null 2>&1
echo $grafana_root_pass | docker secret create grafana_root_pass - > /dev/null 2>&1

sleep 2

printf "${YELLOW}"
# Build the Docker images and push them to the local registry.
docker build influx/ --tag 127.0.0.1:5000/iot-influx:1
docker build grafana/ --tag 127.0.0.1:5000/iot-grafana:1
docker build adapter/ --tag 127.0.0.1:5000/iot-adapter:1

docker push 127.0.0.1:5000/iot-influx:1
docker push 127.0.0.1:5000/iot-grafana:1
docker push 127.0.0.1:5000/iot-adapter:1

# Deploy the whole stack of services
docker stack deploy -c stack.yml sprc