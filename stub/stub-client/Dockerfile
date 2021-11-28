FROM python:3.7
COPY docker-entrypoint.sh /
RUN pip install locust
RUN  mkdir /locust
WORKDIR /locust
EXPOSE 8089 5557 5558

COPY src/locustfile.py /locust/locustfile.py

ENTRYPOINT ["/docker-entrypoint.sh"]
