#!/bin/bash
set -e

# Workaround for working with secrets in docker-compose,
# offered by upstream MySQL and WordPress entrypoints.
file_env() {
    local var="$1"
    local fileVar="${var}_FILE"
    local def="${2:-}"

    if [ "${!var:-}" ] && [ "${!fileVar:-}" ]; then
        echo >&2 "error: both $var and $fileVar are set (but are exclusive)"
        exit 1
    fi

    local val="$def"

    if [ "${!var:-}" ]; then
        val="${!var}"
    elif [ "${!fileVar:-}" ]; then
        val="$(< "${!fileVar}")"
    fi

   export "$var"="$val"
   unset "$fileVar"
}

file_env "DOCKER_INFLUXDB_INIT_PASSWORD"
file_env "DOCKER_INFLUXDB_INIT_ADMIN_TOKEN"

exec /upstream-entrypoint.sh $@