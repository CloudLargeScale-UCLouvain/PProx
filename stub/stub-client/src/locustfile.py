from locust import HttpLocust
from locust import TaskSet
from locust import task
import json
from datetime import datetime
import socket
from pprint import pprint

class SimpleBehavior(TaskSet):

    def on_start(self):
        # Here we log in.
        print("Beginning.")

    @task
    def get(self):
        params = {"host": socket.gethostname(), "ts_client":datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]}
        self.client.get("/", params=params, name="/")

    @task
    def post(self):
        payload = { "host": socket.gethostname(),"user":"me, the client", "film":"This film I like", "note":9, "ts_client":datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]}
        self.client.post("/", json=json.dumps(payload))

    def on_stop(self):
        # Here we log out.
        print("Stopping.")

class MyLocust(HttpLocust):
    task_set = SimpleBehavior
    min_wait = 100
    max_wait = 100
