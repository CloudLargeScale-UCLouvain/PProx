"""
Reverse proxy based on Twisted

"""
from pprint import pprint
from twisted.internet import reactor
from twisted.web import proxy, server

import os


class MyReverseProxyResource(proxy.ReverseProxyResource):
    def render(self, request):
        pprint("Forwarding request : {}".format(str(request)))
        return proxy.ReverseProxyResource.render(self, request)

tcp_port = int(os.getenv('TCP_PORT', 8080))
target_host = os.getenv('TARGET_HOST', 'localhost')
target_port = int(os.getenv('TARGET_PORT', 8080))

site = server.Site(MyReverseProxyResource(target_host, target_port, b''))
reactor.listenTCP(tcp_port, site)
reactor.run()
