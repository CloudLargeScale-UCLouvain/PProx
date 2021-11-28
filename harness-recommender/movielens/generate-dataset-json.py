#!/usr/bin/python3
from io import BytesIO
from zipfile import ZipFile
from urllib.request import urlopen
import csv
import os
import json

maxTS = 1420070400 #01/01/2015 0:00 #maxTS = 1427784002
minTS = 1388534400 #01/01/2014 0:00 #minTS = 789652004
trainingPercent = 16.67 # January + February as training
filename_init = './datasets/init1.csv'
filename_test = './datasets/test1.csv'
os.makedirs("./datasets/", exist_ok=True)

print('Downloading...')
#resp = urlopen("http://files.grouplens.org/datasets/movielens/ml-20m.zip")
resp = urlopen("http://files.grouplens.org/datasets/movielens/ml-latest-small.zip")
zipfile = ZipFile(BytesIO(resp.read()))
print('Extracting...')
#zipfile.extract('ml-20m/ratings.csv') # userId,movieId,rating,timestamp
zipfile.extract('ml-latest-small/ratings.csv') # userId,movieId,rating,timestamp

print('Converting to list...')
reader = csv.DictReader(open('ml-20m/ratings.csv', newline=''))
ratings = list(reader)
print('Subsampling ratings...')
ratings = [rating for rating in ratings if (int(rating['timestamp']) > minTS and int(rating['timestamp']) < maxTS)]
print('Sorting ratings...')
ratings = sorted(ratings, key=lambda k:int(k['timestamp']))
print('Writing...')
fieldnames = ["body"]
with open(filename_init, 'w') as csvfile_init:
    with open(filename_test, 'w') as csvfile_test:
        writer_init = csv.DictWriter(csvfile_init, fieldnames=fieldnames, extrasaction='ignore')
        writer_test = csv.DictWriter(csvfile_test, fieldnames=fieldnames, extrasaction='ignore')
        #writer_init.writeheader()
        #writer_test.writeheader()
        for item in ratings:
            data = {}           
            if int(item['timestamp']) < minTS + ((maxTS - minTS) * (trainingPercent / 100)):
                data['event'] = 'film'
                data['entityType'] = 'user'
                data['entityId'] = item["userId"]
                data['targetEntityType'] = 'item'
                data['targetEntityId'] = item["movieId"]
                row = {"body":json.dumps(data)}
                writer_init.writerow(row)
            else:
                data={}
                data["user"] = item["userId"]
                row = {"body":json.dumps(data)}
                writer_test.writerow(row)
print('Done.')