from locust import HttpLocust
from locust import TaskSet
from locust import task
from locust import events
import json
from datetime import datetime
import socket
from pprint import pprint
import logging
import os
from os import getenv as os_getenv
import time
import sys
import rsa
import base64

enable_encryption = os_getenv("ENABLE_ENCRYPTION", False)
if enable_encryption == "1":
	enable_encryption = True
else:
	enable_encryption = False

with open('public_P1.pem', mode='rb') as public_P1:
    keydata1 = public_P1.read()
pubkeyp1 = rsa.PublicKey.load_pkcs1(keydata1)
with open('public_P2.pem', mode='rb') as public_P2:
    keydata2 = public_P2.read()
pubkeyp2 = rsa.PublicKey.load_pkcs1(keydata2)

sys.path.append(os.getcwd())

import common.movielens
from common.movielens import MovielensInjectionTaskSet

index = 0

class EventInjectionTaskSet(MovielensInjectionTaskSet):
    def on_start(self):
        super(EventInjectionTaskSet, self).on_start()

    @task
    def events(self):
        # curl -H "Content-Type: application/json" -d '{"event": "film", "entityType": "user", "entityId": "Jack", "targetEntityType": "item", "targetEntityId": "Fight club", "eventTime": "2019-08-06T21:57:13.936Z"}' http://localhost:9090/engines/sgx_ur/events
        global index
        sample = self.ratings_app.iloc[index]
        index = index + 1
        tEI = base64.b64encode(rsa.encrypt(str(sample["movieId"]).encode('utf8'), pubkeyp2)).decode('utf8') if enable_encryption else str(sample["movieId"])
        eI = base64.b64encode(rsa.encrypt(str(sample["userId"]).encode('utf8'), pubkeyp1)).decode('utf8') if enable_encryption else str(sample["userId"])
        payload = {
            "event": "film",
            "entityType":"user",
            "entityId":eI,
            "targetEntityType":"item",
            "targetEntityId":tEI,
            "eventTime":datetime.utcfromtimestamp(time.time() / 1e9).isoformat(timespec="milliseconds") + "Z"
            }
        print('payload: ' + str(payload))
        time_start = time.time()
        response = self.client.post("/engines/sgx_ur/events", json=payload)
        time_end = time.time()
        print('response: ' + str(response))

class EventInjectionLocust(HttpLocust):
    task_set = EventInjectionTaskSet
    min_wait = 100
    max_wait = 100
