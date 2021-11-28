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
import time
import sys
import rsa
import base64
import random

enable_encryption = False
USER_KEY_LEN = 10

with open('public_P1.pem', mode='rb') as public_P1:
    keydata1 = public_P1.read()
pubkeyp1 = rsa.PublicKey.load_pkcs1(keydata1)
with open('public_P2.pem', mode='rb') as public_P2:
    keydata2 = public_P2.read()
pubkeyp2 = rsa.PublicKey.load_pkcs1(keydata2)

sys.path.append(os.getcwd())

import common.movielens
from common.movielens import MovielensInjectionTaskSet

class QueryInjectionTaskSet(MovielensInjectionTaskSet):
    def on_start(self):
        super(QueryInjectionTaskSet, self).on_start()

    @task
    def queries(self):
        # curl -H "Content-Type: application/json" -d '{"user": "Jack"}' http://localhost:9090/engines/sgx_ur/queries
        sample = self.ratings_test.sample(n=1)
        uID = base64.b64encode(rsa.encrypt(str(sample["userId"].values[0]).encode('utf8'), pubkeyp1)).decode('utf8') if enable_encryption else str(sample["userId"].values[0])
        digits = '0123456789'
        k1 = ''.join((random.choice(digits) for i in range(USER_KEY_LEN)))
        k = base64.b64encode(rsa.encrypt(str(k1).encode('utf8'), pubkeyp2)).decode('utf8') if enable_encryption else str(k1)
        print(k)
        payload = {
            "user":uID,
            "key":k
        }
        time_start = time.time()
        response = self.client.post("/engines/sgx_ur/queries", json=payload)
        time_end = time.time()
#        logger.info("Response - URL: {url}. Status code: {status}. "
#                    "Latency: {duration}".format(url=response.url,
#                                                 status=response.status_code,
#                                                 duration=round(time_end - time_start, 3)))

class QueryInjectionLocust(HttpLocust):
    task_set = QueryInjectionTaskSet
    min_wait = 100
    max_wait = 100
