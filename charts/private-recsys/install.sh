#!/bin/bash
kubectl apply -f ..
helm install --name recsys .  -f ./values-limits.yaml $1 $2
