#!/bin/bash
helm repo add stable https://charts.helm.sh/stable
helm repo add incubator https://charts.helm.sh/incubator
helm install docker-registry stable/docker-registry --namespace kube-system   --set service.port=5000
helm install kube-registry-proxy incubator/kube-registry-proxy --namespace kube-system  --set registry.host=docker-registry --set registry.port=5000 --set hostPort=32000

# init dependencies for manager chart
CHART_DIRECTORY=../../ManagerKubernetesExperimentations/chart
cd $CHART_DIRECTORY/..; ./init.sh