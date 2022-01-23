#!/bin/bash
set -e

# Workaround for working with secrets in docker-compose,
# used offered by upstream MySQL and WordPress entrypoints.
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

file_env "GF_SECURITY_ADMIN_PASSWORD"
file_env "INFLUX_TOKEN"

exec /upstream-entrypoint.sh $@