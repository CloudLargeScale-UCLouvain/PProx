#!/bin/sh
# inspired from https://medium.com/locust-io-experiments/locust-io-experiments-running-in-docker-cae3c7f9386e
set -e
LOCUST_MODE=${LOCUST_MODE:-standalone}
LOCUST_MASTER_BIND_PORT=${LOCUST_MASTER_BIND_PORT:-5557}
LOCUST_FILE=${LOCUST_LOCUSTFILE_PATH:-/locust/locustfile.py}

if [ -z ${LOCUST_TARGET_HOST+x} ] ; then
    echo "You need to set the URL of the host to be tested (LOCUST_TARGET_HOST)."
    exit 1
fi

LOCUST_OPTS="-f ${LOCUST_FILE} --host=${LOCUST_TARGET_HOST} --no-reset-stats $LOCUST_OPTS"

case `echo ${LOCUST_MODE} | tr 'a-z' 'A-Z'` in
"MASTER")
    LOCUST_OPTS="--master --master-bind-port=${LOCUST_MASTER_BIND_PORT} $LOCUST_OPTS"
    ;;

"SLAVE")
    LOCUST_OPTS="--slave --master-host=${LOCUST_MASTER} --master-port=${LOCUST_MASTER_BIND_PORT} $LOCUST_OPTS"
    if [ -z ${LOCUST_MASTER+x} ] ; then
        echo "You need to set LOCUST_MASTER."
        exit 1
    fi
    ;;
esac
cd /locust
locust ${LOCUST_OPTS}
