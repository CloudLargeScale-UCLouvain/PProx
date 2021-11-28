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
#from fluent import asyncsender as sender
from fluent import sender

RANDOM_SEED = os_getenv("RANDOM_SEED", 0)
HOSTNAME = os_getenv("HOSTNAME", "host")
logger = logging.getLogger(__name__)
logger.setLevel(os.getenv("LOCUST_LOG_LEVEL", "INFO").upper())
fmt_pattern = '{"time":"%(asctime)s.%(msecs)03dZ", ' \
              '"loglevel":"%(levelname)s", "hostname":"' \
              + HOSTNAME + '", "message":%(message)s}'
datefmt_pattern = '%Y-%m-%dT%H:%M:%S'                       # date format
fluentd_host = os.getenv("LOCUST_FLUENTD_HOST", None)
xp_name = os.getenv("XP_NAME", "test")
xp_name = "{}.{}".format(
    "private-recsys",
    xp_name
)
print ("Starting experiment " + xp_name)
if fluentd_host:
    print("Use fluentd on " + fluentd_host)
    sender = sender.FluentSender(xp_name, host=fluentd_host, port=24224, nanosecond_precision=True)

def log_fluentd(log):
    if fluentd_host:
        if not sender.emit('locust_request_result', log):
            print(sender.last_error)
            sender.clear_last_error() # clear stored error after handled errors

def success_handler(request_type, name, response_time, response_length, **kwargs):
    """ handles success requests """
    log = {
         "class":"locust",
         "response_type":"success",
         "request_type":request_type,
         "name":name,
         "exception": "",
         "response_time": response_time,
         "response_length": response_length
    }

    logger.info(json.dumps(log))
    log_fluentd(log)

def fail_handler(request_type, name, response_time, exception, **kwargs):
    """ handles success fail """
    log = {
         "class":"locust",
         "response_type":"error",
         "request_type":request_type,
         "name":name,
         "exception": repr(exception),
         "response_time": response_time,
         "response_length": 0
    }
    logger.info(json.dumps(log))
    log_fluentd(log)

events.request_success += success_handler
events.request_failure += fail_handler

class MovielensInjectionTaskSet(TaskSet):
    def on_start(self):
        import pandas as pd
        # read from
        def dateparse (time_in_secs):
            return datetime.utcfromtimestamp(float(time_in_secs))

        ratings = pd.read_csv("/dataset/init1.csv")
        #self.movies = pd.read_csv("ml-latest-small/movies.csv")

        self.ratings_app = ratings
        self.ratings_test = ratings

    def on_stop(self):
        # Here we log out.
        print("Stopping.")
