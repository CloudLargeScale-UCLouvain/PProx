#!/bin/bash
helm delete --purge recsys
sleep 1
kubectl delete pvc elasticsearch-master-elasticsearch-master-0 
sleep 1
kubectl delete pv elastic
