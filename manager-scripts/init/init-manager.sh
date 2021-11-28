#!/bin/bash

CHART_DIRECTORY=../../ManagerKubernetesExperimentations/chart
helm delete --namespace manager manager 
helm delete nginx-ingress --namespace kube-system
kubectl delete crd --all
kubectl delete -f ./persistentVolume-jupyter.yaml
kubectl delete -f ./persistentVolume-mongoxp.yaml
kubectl apply -f ./persistentVolume-jupyter.yaml
kubectl apply -f ./persistentVolume-mongoxp.yaml

# used for sequence in deployments in manager (for instance jupyter waits for mongo) (groundnuty/k8s-wait-for)
kubectl apply -f $CHART_DIRECTORY/../cluster-role-binding-manager.yaml
helm dep update $CHART_DIRECTORY
helm install manager $CHART_DIRECTORY --namespace manager -f values.yaml 

helm install nginx-ingress stable/nginx-ingress --namespace kube-system --set controller.stats.enabled=true --set controller.kind=DaemonSet --set controller.daemonset.useHostPort=true  --set controller.service.type=NodePort --set defaultBackend.nodeSelector.tier=manager

# used for sequence in deployments in default (for instance, proxy wait for harness/stub) (groundnuty/k8s-wait-for)
kubectl apply -f ./cluster-role-binding-default.yaml