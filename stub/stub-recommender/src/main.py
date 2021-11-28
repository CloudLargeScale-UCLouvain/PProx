"""
Simple Web Server using twisted parsing JSON and sending it back to stdout

"""
from pprint import pprint
from twisted.web import server, resource
from twisted.internet import reactor, endpoints
import json
from io import BytesIO
from datetime import datetime

class RecommenderStub(resource.Resource):
    isLeaf = True
    numberGetRequests = 0

    def render_POST(self, request):
        data = request.content.getvalue()
        request.responseHeaders.addRawHeader(b"content-type", b"application/json")

        loaded = json.loads(data.decode('unicode-escape').strip('"') )
        loaded.update({"ts_recommender": datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]})

        #print("POST " + json.dumps(loaded))
        return json.dumps(loaded).encode("ascii")

    def render_GET(self, request):
        request.responseHeaders.addRawHeader(b"content-type", b"application/json")
        json_test = { "ts_recommender": datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3] , "user":"someone in the RS", "film":"This film he likes", "note":7}
        #json_test.update(request.args)
        #print("GET" + str(request.args) + "-" + json.dumps(json_test))
        return json.dumps(json_test).encode("ascii")

print("Launch web server")
endpoints.serverFromString(reactor, "tcp:8080").listen(server.Site(RecommenderStub()))
reactor.run()
