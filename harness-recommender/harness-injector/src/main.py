import requests
import sys
from threading import Thread

import json
from datetime import datetime
import socket
from pprint import pprint
import logging
import os
import time
from os import getenv as os_getenv
import rsa
import pandas as pd
import base64
import random
import traceback
from Crypto.Cipher import AES

enable_encryption = os_getenv("ENABLE_ENCRYPTION", False)
if enable_encryption == "1":
	enable_encryption = True
else:
	enable_encryption = False

USER_KEY_LEN = 16

ASYNC_FLUENTD = os_getenv("ASYNC_FLUENTD", "false")
if ASYNC_FLUENTD == "true":
    from fluent import asyncsender as fluentd_sender
else:
    from fluent import sender as fluentd_sender

TIMEOUT_REQUEST = os_getenv("TIMEOUT_REQUEST", 800)
PERIOD_REQUEST = os_getenv("PERIOD_REQUEST", 1000)
NB_REQUESTS = os_getenv("NB_REQUESTS", 10)
RANDOM_SEED = os_getenv("RANDOM_SEED", 0)
HOSTNAME = os_getenv("HOSTNAME", "host")
logger = logging.getLogger(__name__)
logger.setLevel(os.getenv("LOG_LEVEL", "INFO").upper())
fmt_pattern = '{"time":"%(asctime)s.%(msecs)03dZ", ' \
              '"loglevel":"%(levelname)s", "hostname":"' \
              + HOSTNAME + '", "message":%(message)s}'
datefmt_pattern = '%Y-%m-%dT%H:%M:%S'                       # date format
FLUENTD_HOST = os_getenv("FLUENTD_HOST", "http://manager-fluentd")
XP_NAME = os_getenv("XP_NAME", "test")
xp_name = "{}.{}".format(
    "private-recsys",
    XP_NAME
)

URL_BASE = os_getenv("URL_BASE", "http://recsys-harness:9090")
DURATION = os_getenv("DURATION", 10)

print ("Starting experiment " + xp_name)

with open('public_P1.pem', mode='rb') as public_P1:
    keydata1 = public_P1.read()
pubkeyp1 = rsa.PublicKey.load_pkcs1(keydata1)
with open('public_P2.pem', mode='rb') as public_P2:
    keydata2 = public_P2.read()
pubkeyp2 = rsa.PublicKey.load_pkcs1(keydata2)

sys.path.append(os.getcwd())

def log_fluentd(log):
    if FLUENTD_HOST:
        if not sender.emit('locust_request_result', log):
            print(sender.last_error)
            sender.clear_last_error() # clear stored error after handled errors
from multiprocessing import Process
from datetime import timedelta, datetime
if FLUENTD_HOST:
    print("Use fluentd on " + FLUENTD_HOST)
    sender = fluentd_sender.FluentSender(xp_name, host=FLUENTD_HOST, port=24224, nanosecond_precision=True)

def log(url, payload, response, res_json, start, response_time, diff=None, exception=None):
    if response is not None:
        log = {
            "dt": datetime.fromtimestamp(start).isoformat(),
            "start":start,
            "payload": payload,
            "url":url,
            "http_status":response.status_code,
            "response_time": response_time,
            "response_length": len(response.content),
            "content": res_json,
            "exception": "",
            "diff":diff.microseconds / 1000
        }
    else:
        log = {
            "dt": datetime.fromtimestamp(start).isoformat(),
            "start": start,
            "payload": payload,
            "url": url,
            "http_status": "",
            "response_time": response_time,
            "response_length": 0,
            "content": "",
            "exception":exception,
            "diff":diff.microseconds / 1000
        }


    #logger.info(json.dumps(log))
    log_fluentd(log)

def run(url, duration, period, timeout):
    def dateparse (time_in_secs):
        return datetime.utcfromtimestamp(float(time_in_secs))
    print(url)
    ratings = pd.read_csv("ml-latest-small/ratings.csv", parse_dates=[3], date_parser=dateparse)
    movies = pd.read_csv("ml-latest-small/movies.csv")

    ratings_app = ratings.sample(frac=0.8, random_state=1)
    ratings_test = ratings.drop(ratings_app.index)

    sample = ratings_test.sample(n=1)

    for i in range(duration):
        e = None
        response = None
        try:
            sample = ratings_test.sample(n=1)
            uID = base64.b64encode(rsa.encrypt(str(sample["userId"].values[0]).encode('utf8'), pubkeyp1)).decode('utf8') if enable_encryption else str(sample["userId"].values[0])
            digits = '0123456789'
            k1 = ''.join((random.choice(digits) for i in range(USER_KEY_LEN)))
            k = base64.b64encode(rsa.encrypt(str(k1).encode('utf8'), pubkeyp2)).decode('utf8') if enable_encryption else str(k1)
            payload = {
                "user":uID,
                "key":k
            }

            start = time.time()
            start_dt = datetime.fromtimestamp(start)

            response = requests.post(url, timeout=timeout/1000, json=payload)
            end = time.time()

        except requests.exceptions.ReadTimeout:
            end = time.time()
            e = "TIMEOUT"
        except:
            end = time.time()
            e = traceback.format_exc()
            print("Unexpected error:", e)


        now = datetime.now()
        diff = (start_dt + timedelta(seconds=period/1000)) - now
        
        symmetric_iv = "01234567"
        cipher = AES.new(k1.encode('utf8'), AES.MODE_CTR, nonce=symmetric_iv.encode('utf8'))
        res_json = None
        if response:
        	res_json = json.loads(response.text)
        #if res_json:
            #for movie in res_json['result']:
                #movie['item'] = cipher.decrypt(base64.b64decode(movie['item'])).decode('utf8') if enable_encryption else movie['item']
        
        log(url, payload, response, res_json, start, end - start, diff, e)

        if diff >= timedelta(0):
            time.sleep(diff.total_seconds())
            print("wait {}".format(diff.total_seconds()))
        else:
            print("ERROR {}".format(diff))

if __name__ == "__main__":
    processes = []
    for i in range(int(NB_REQUESTS)):
        p = Process(target=run,args=[URL_BASE + "/engines/sgx_ur/queries", int(DURATION), int(PERIOD_REQUEST) , int(TIMEOUT_REQUEST)])
        processes.append(p)
    for p in processes:
        p.start()
    for p in processes:
        p.join()
    print("end")
    if FLUENTD_HOST:
        sender.close()
        
