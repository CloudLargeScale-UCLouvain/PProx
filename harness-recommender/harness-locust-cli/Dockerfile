FROM python:3.6
MAINTAINER Guillaume Rosinosky <guillaumerosinosky@yahoo.fr>
COPY docker-entrypoint.sh /
RUN apt update
RUN apt install -y libzmq3-dev python3-dev python3-pip \
      && pip install locustio==0.11.0 gevent==1.2.2
RUN pip install pandas fluent-logger rsa
RUN  mkdir /locust
WORKDIR /locust
EXPOSE 8089 5557 5558
COPY ur.json /locust/ur.json
COPY public_P1.pem /locust/public_P1.pem
COPY public_P2.pem /locust/public_P2.pem
COPY src/ /locust/
ENTRYPOINT ["/docker-entrypoint.sh"]
