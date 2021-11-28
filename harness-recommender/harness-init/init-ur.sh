#!/usr/bin/env bash
export PATH=$PATH:/harness-cli/harness-cli
engine=sgx_ur
engine_json=/harness-cli/ur/ur.json
harness-cli add ${engine_json}
echo $?
until curl -f ${HARNESS_SERVER_ADDRESS}:9090; do
  echo "Waiting for Harness web server"
  sleep 2
done
until harness-cli delete ${engine} && harness-cli add ${engine_json}; do
  echo "Waiting for Harness initialization"
  sleep 2
done
sleep 600
