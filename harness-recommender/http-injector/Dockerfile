FROM node:10
# Create app directory
WORKDIR /usr/src/app

EXPOSE 8080

ADD http://files.grouplens.org/datasets/movielens/ml-latest-small.zip ./ml-latest-small.zip
RUN unzip ml-latest-small.zip

ENV FLUENTD_HOST manager_fluentd
ENV XP_NAME test
ENV URL_BASE http://localhost:9090
ENV DURATION 10
ENV NB_REQUESTS 1
ENV ENABLE_ENCRYPTION 0
ENV CSV_BODY /dataset/test1.csv
COPY . .

RUN npm install

CMD sh -c "sleep 45" && node --max-old-space-size=8192 index.js
# sleep 45 for SGX

