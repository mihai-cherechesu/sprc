#!/bin/bash

echo "Stopping the stack..."
docker stack rm sprc
echo ""

sleep 5

echo "Removing the service secrets..."
docker secret rm grafana_root_pass 2> /dev/null
docker secret rm influx_init_pass 2> /dev/null
docker secret rm influx_init_token 2> /dev/null
echo ""

registry_container_id=$(docker ps -a --format "{{.ID}} {{.Names}}" | grep -E 'registry.[a-zA-Z0-9]*' | cut -d' ' -f1 2> /dev/null)
echo "Removing the registry container ${registry_container_id}..."
if [[ ! -z ${registry_container_id} ]]; then
    docker stop ${registry_container_id}
fi
echo ""

if [[ $1 == 'volumes' ]]; then
    echo "Removing the data named volumes..."
    docker volume rm --force sprc_influx-data 2> /dev/null
    docker volume rm --force sprc_grafana-data 2> /dev/null
    docker volume rm --force sprc_mosquitto-data 2> /dev/null
    echo ""
fi
