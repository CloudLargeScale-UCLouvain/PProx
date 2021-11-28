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
from os import getenv as os_getenv
import sys

sys.path.append(os.getcwd())

import common.movielens
from common.movielens import MovielensInjectionTaskSet

class TrainInjectionTaskSet(MovielensInjectionTaskSet):
    def on_start(self):
        super(TrainInjectionTaskSet, self).on_start()

    @task
    def train(self):
        response = self.client.post("/engines/sgx_ur/jobs")
        running = True
        time_start = time.time()
        while running:
            response = self.client.get("/engines/sgx_ur/")
            print (response.json()["jobStatuses"])
            time.sleep(1)
            running = False
        time_end = time.time()
        log = {
             "class":"locust",
             "response_type":"success",
             "request_type":request_type,
             "name":"train",
             "exception": "",
             "response_time": time_end-time_start,
             "response_length": ""
        }
        log_fluentd(log)

class TrainInjectionLocust(HttpLocust):
    task_set = TrainInjectionTaskSet
    min_wait = 100
    max_wait = 100
