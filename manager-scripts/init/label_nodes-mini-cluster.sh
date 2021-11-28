# node labels for mini test cluster
# remove labels
kubectl label node nuc1.maas tier-
kubectl label node nuc10.maas tier-
kubectl label node nuc13.maas tier-
kubectl label node nuc14.maas tier-
kubectl label node nuc15.maas tier-
kubectl label node nuc16.maas tier-
kubectl label node nuc18.maas tier-

# label nodes
kubectl label node nuc1.maas tier=manager
kubectl label node nuc13.maas tier=pprox
kubectl label node nuc14.maas tier=pprox
kubectl label node nuc15.maas tier=harness # or stub
kubectl label node nuc10.maas tier=mongo # for harness
kubectl label node nuc16.maas tier=elastic # for harness
kubectl label node nuc18.maas tier=injector
