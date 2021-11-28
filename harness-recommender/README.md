# Harness-recommender

Deploy with Docker Compose.

```bash

docker-compose up --build

```

## Manual test

Create the engine sgx_ur

```bash

curl -d "@ur.json" -X POST  -H "Content-Type: application/json" http://localhost:9090/engines/

```

Add an event

```bash

curl -H "Content-Type: application/json" -d '{"event": "film", "entityType": "user", "entityId": "Jack", "targetEntityType": "item", "targetEntityId": "Fight club", "eventTime": "2019-08-06T21:57:13.936Z"}' http://localhost:9090/engines/sgx_ur/events

```

Train manually the model

```bash

curl -X POST  http://localhost:9090/engines/sgx_ur/jobs

```

Wait for the model learning. It can take a minute.
Get recommendation.

```bash

curl -H "Content-Type: application/json" -d '{"user": "Jack"}' http://localhost:9090/engines/sgx_ur/queries

```
