FROM python:3.7
MAINTAINER Guillaume Rosinosky <guillaumerosinosky@yahoo.fr>
RUN apt update
RUN pip install pandas fluent-logger rsa requests pycryptodome
WORKDIR /
ADD http://files.grouplens.org/datasets/movielens/ml-latest-small.zip /ml-latest-small.zip
COPY public_P1.pem /public_P1.pem
COPY public_P2.pem /public_P2.pem
RUN unzip ml-latest-small.zip
COPY src/main.py /main.py
ENV FLUENTD_HOST manager_fluentd
ENV XP_NAME test
ENV URL_BASE http://harness.130-104-229-243.nip.io
ENV DURATION 10
ENV NB_REQUESTS 10
ENTRYPOINT ["python", "main.py"]
