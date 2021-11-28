#!/usr/bin/python3
from io import BytesIO
from zipfile import ZipFile
from urllib.request import urlopen
import csv
import os
import json
from datetime import datetime
import time

maxTS = 1420070400 #01/01/2015 0:00 #maxTS = 1427784002
minTS = 1388534400 #01/01/2014 0:00 #minTS = 789652004
trainingPercent = 16.67 # January + February as training
dataset = "ml-20m"
filename_init = './{}/init1.csv'.format(dataset)
filename_test = './{}/test1.csv'.format(dataset)
os.makedirs("./{}/".format(dataset), exist_ok=True)


print('Downloading...')
#resp = urlopen("http://files.grouplens.org/datasets/movielens/{{ .Values.datasetName }}.zip")
resp = urlopen("http://files.grouplens.org/datasets/movielens/{}.zip".format(dataset))
zipfile = ZipFile(BytesIO(resp.read()))
print('Extracting...')
zipfile.extract('{}/ratings.csv'.format(dataset)) # userId,movieId,rating,timestamp

print('Converting to list...')
reader = csv.DictReader(open('{}/ratings.csv'.format(dataset), newline=''))
ratings = list(reader)
print('Subsampling ratings...')
ratings = [rating for rating in ratings if (int(rating['timestamp']) > minTS and int(rating['timestamp']) < maxTS)]
print('Sorting ratings...')
ratings = sorted(ratings, key=lambda k:int(k['timestamp']))
print('Writing...')
#fieldnames = ["body"]
fieldnames = ['userId','movieId']
with open(filename_init, 'w') as csvfile_init:
    with open(filename_test, 'w') as csvfile_test:
        writer_init = csv.DictWriter(csvfile_init, fieldnames=fieldnames, extrasaction='ignore')
        writer_test = csv.DictWriter(csvfile_test, fieldnames=fieldnames, extrasaction='ignore')
        writer_init.writeheader()
        writer_test.writeheader()
        for item in ratings:
            print(item)            
            if int(item['timestamp']) < minTS + ((maxTS - minTS) * (trainingPercent / 100)):
                data={}
                data['event'] = 'film'
                data['entityType'] = 'user'
                data['entityId'] = item["userId"]
                data['targetEntityType'] = 'item'
                data['targetEntityId'] = item["movieId"]
                data['eventTime'] = datetime.utcfromtimestamp(time.time() / 1e9).isoformat(timespec="milliseconds") + "Z"
                row = {"body":json.dumps(data)}
                writer_init.writerow(item)
            else:
                data={}
                data["user"] = item["userId"]
                row = {"body":json.dumps(data)}
                writer_test.writerow(item)
print('Done.')