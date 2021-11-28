# Details installation

## Kubespray tutorial

If you have an already running Kubernetes cluster with a registry you can pass this part. If you don't have, you can use the following steps, based on [Kubespray](https://kubespray.io/#/), [Kubespray official getting started](https://github.com/kubernetes-sigs/kubespray/blob/master/docs/getting-started.md). You can also check and [Leonardo Bueno's tutorial](https://medium.com/@leonardo.bueno/setting-up-a-kubernetes-cluster-with-kubespray-1bf4ce8ccd73). Let's assume you want one master and one worker node, respectively ```192.168.0.81``` and ```192.168.0.82```. First you need to copy public SSH keys to your nodes, so Ansible can login easily.
```
ssh-copy-id user@192.168.0.81
ssh-copy-id user@192.168.0.82
```


Then install Kubespray. You need to install Python 3, Ansible, Jinja, preferably using ```pip```, and then clone their [repository](https://github.com/kubernetes-sigs/kubespray). Details included in their readme it.
```
git clone git@github.com:kubernetes-sigs/kubespray.git
cd kubespray
sudo pip install -r requirements.txt
```

You need now to prepare the scripts for your soon to be k8s cluster. As in the regular tutorial, we name it "mycluster".
```
cp -rfp inventory/sample inventory/mycluster
declare -a IPS=(192.168.0.81 192.168.0.82)
CONFIG_FILE=inventory/mycluster/hosts.yml python3 contrib/inventory_builder/inventory.py ${IPS[@]}
```
Edit the configuration for your convenience, in ```inventory/mycluster```. First, in the described case, ```hosts.yml``` should be edited in order to look like this :
```
all:
  hosts:
    master1:
      ansible_user: ubuntu
      ansible_host: 192.168.0.81
      ip: 192.168.0.81
      access_ip: 192.168.0.81
    worker1:
      ansible_user: ubuntu    
      ansible_host: 192.168.0.82
      ip: 192.168.0.82
      access_ip: 192.168.0.82
  children:
    kube-master:
      hosts:
        master1:
    kube-node:
      hosts:
        worker1:
    etcd:
      hosts:
        master1:
    k8s-cluster:
      children:
        kube-master:
        kube-node:
    calico-rr:
      hosts: {}

```
Of course, in case of additional machines, more should be declared. Don't forget to precise the user for each node. Kubespray Ansible playbooks need root access, so you should have a sudoer.

Other interesting files to edit are ```inventory/mycluster/group_vars/etcd.yaml``` (RAM for etcd), ```inventory/mycluster/group_vars/all/all.yml```, ```inventory/mycluster/group_vars/all/k8s-cluster.yml```. If you want to install Helm, uncomment and change the value to ```helm_enabled: true``` in ```all.yaml```. If you want to control from your local `kubectl`, change the value to `kubeconfig_localhost: true` in `k8s-cluster.yml`. Be aware that you will probably need to upgrade the Tiller (Helm's server) version to match your Helm client version. It can be done with `helm init --upgrade` on the client.

Kubespray proposes for now three storage solutions : local storage, CephFS and rbd. For local storage, place `local_volume_provisioner_enabled` to true and uncomment every following related line. You will need a distributed storage for more advanced usage.

You can check if your nodes are reachable with the following command, all nodes should be in success :
```
ansible -i inventory/mycluster/hosts.yml -m ping all
```

When necessary, you can launch a command on all your nodes, such as a reboot :
```
ansible -i inventory/mycluster/hosts.yml -m shell -a 'sudo reboot' --become all
```

Once you finished, you can launch the command to install the cluster. It can take dozens of minutes depending on what is already installed. Also, depending on the account used for the master and nodes, you should choose wether using "-b" and "-K" options (respectively for Ansible to become root via sudo, and ask for password of sudo before launching the command).
```
ansible-playbook -b -K -i inventory/mycluster/hosts.yml cluster.yml
```

Ssh into your master node. It is not clearly documented, but an additional step to is needed to use kubectl: launch the following command to be able to it to be configured.
```
sudo cp /etc/kubernetes/admin.conf $HOME/ && sudo chown $(id -u):$(id -g) $HOME/admin.conf && export KUBECONFIG=$HOME/admin.conf
echo "export KUBECONFIG=$HOME/admin.conf" >> ~/.bashrc
```

To connect to the dashboard.
```
kubectl cluster-info
kubectl -n kube-system describe secrets \
   `kubectl -n kube-system get secrets | awk '/clusterrole-aggregation-controller/ {print $1}'` \
       | awk '/token:/ {print $2}'
```
Cluster info will give you the URL, the following command the token.

[Helm](https://helm.sh/) is the most used package manager for Kubernetes. We will use it to get the usual packages, and in the near future for our deployments. Version 2 is needed for our scripts. If not installed, use ```sudo snap install helm --channel=2.16/stable --classic```, and initialize it.
```
helm init
```

Give access of your cluster to helm - here full [source](https://stackoverflow.com/questions/46672523/helm-list-cannot-list-configmaps-in-the-namespace-kube-system). This step is not useful if helm support is activated in kubespray.
```
kubectl create serviceaccount --namespace kube-system tiller
kubectl create clusterrolebinding tiller-cluster-rule --clusterrole=cluster-admin --serviceaccount=kube-system:tiller
kubectl patch deploy --namespace kube-system tiller-deploy -p '{"spec":{"template":{"spec":{"serviceAccount":"tiller"}}}}'      
```

If helm support is installed with Kubespray, or if helm client complains about versions, update tiller (server side component of helm) to the last version.
```
helm init --service-account tiller --upgrade
```

If not already done, we highly advice adding auto-completion for kubectl and helm. Add these lines in your .bashrc file.
```
# kubectl
echo 'source <(kubectl completion bash)' >>~/.bashrc
# helm
echo 'source <(helm completion bash)' >>~/.bashrc

```

Install a Docker repository (we use this [helm chart](https://github.com/helm/charts/tree/master/stable/docker-registry) ).
```
helm install  --namespace kube-system stable/docker-registry --name docker-registry --set service.port=5000
```

We want all nodes to be able to pull our images. We assume the registry is on localhost:5000. For this, we use this [helm chart](https://github.com/helm/charts/tree/master/incubator/kube-registry-proxy) who adds a Daemonset deploying one proxy redirecting to the registry on each host.
```
helm repo add incubator https://charts.helm.sh/incubator
helm install --name kube-registry-proxy  --namespace kube-system incubator/kube-registry-proxy --set registry.host=docker-registry --set registry.port=5000 --set hostPort=32000
```

Before pushing your images, forward 5000 port (the Daemonset works on the nodes, not on the master).
```
export POD_NAME=$(kubectl get pods --namespace kube-system -l "app=docker-registry,release=docker-registry" -o jsonpath="{.items[0].metadata.name}")
kubectl -n kube-system port-forward $POD_NAME 32000:32000
```

Deploy Helm chart for nginx ingress. Access will be possible on every node on port 80 or 443. It is better to deploy it on other nodes that the SUT (here on a manager node).
```
helm install stable/nginx-ingress --name nginx-ingress --set controller.stats.enabled=true --set controller.kind=DaemonSet --set controller.daemonset.useHostPort=true  --set controller.service.type=NodePort --namespace kube-system --set defaultBackend.nodeSelector.tier=manager
```
## Scaling

Add or remove nodes.
```
# add nodes
ansible-playbook -b -K -i inventory/mycluster/hosts.yml scale.yml
# remove nodes (here nodename, nodename2)
ansible-playbook -b -K -i inventory/mycluster/hosts.yml remove-node.yml -b -v \

  --extra-vars "node=nodename,nodename2"

```

## Troubleshooting

Sometimes, things don't go well (new node not recognized, new installation using old nodes, etc.). Resetting the cluster is the first thing to do.
```
ansible-playbook -b -K -i inventory/mycluster/hosts.yml reset.yml
```

Sometimes, during a new installation or scale out, there is some issues with cgroup drivers : different parameters are put in `kubelet` and in `docker`. Do the following command in order to force cgroupfs in `kubelet` (see https://github.com/kubernetes-sigs/kubespray/issues/4575)
```
ansible-playbook -b -K -i inventory/mycluster/hosts.yml cluster.yml -e "kubelet_cgroup_driver=cgroupfs"
```

Be aware that cluster name should respect a clear dns-compatible syntax, it can break the cluster if not.

Check that every port is clearly open between all the nodes. If not, silent (or not) errors can happen during the launch of the playbook. For instance, in case of problems, master nodes can begin to reset untimely after the startup of the node. Individual nodes can become in a broken state. It is possible to force reinstallation a (or several) individual node :
```
ansible-playbook -b -K -i inventory/mycluster/hosts.yml --limit node1 cluster.yml
```

However sometimes uninstallation of kubelet or docker daemon can be necessary if it don't work. Check carefully the cluster state with the following commands.
```
sudo journalctl -u kubelet.service
sudo journalctl -u docker.service

```

## Various technical details

In our case, we need important pods to await for each other, more precisely Harness should wait for Mongo, Elastic and Spark. We use init containers in Harness (and all the other modules in a next step) based on https://github.com/groundnuty/k8s-wait-for. This container waits for a job or pod to be deployed or terminate. It needs sufficient authorizations in the `default` namespace. It can be done with this Cluster Role Binding.
```
kubectl apply -f ./cluster-role-binding.yaml
```
These scripts focus on a on-premises Kubernetes. Access to useful endpoints are possible with subdomains assured by Nginx ingress, on a defined node : we focus on a on premises deployment for this first iteration. If not already deployed, install it:
```
helm install stable/nginx-ingress --name nginx-ingress --set controller.stats.enabled=true --set controller.kind=DaemonSet --set controller.daemonset.useHostPort=true  --set controller.service.type=NodePort --namespace kube-system
```
For data persistence, we assume you have sufficient persistent volumes for Mongo and Jupyter. See the example in `charts/manager/install.sh` for instance. Also, in the following example, we take the address `10-0-0-19.nip.io`. You should replace this address with the corresponding node address who will take the endpoint role. A DNS name is needed, so ensure your node is well named or use service such as [nip.io](https://nip.io/).

Each module should be deployed on defined nodes (`injector`, `harness`, elastic, mongo). Your nodes must be labeled with the right `tier` - file `manager-scripts/label_nodes.sh` can be used for this purpose :
```
kubectl get nodes
kubectl label nodes node-nuc2 tier=injector
kubectl label nodes node-nuc6 tier=harness
kubectl label nodes node-nuc7 tier=elastic
kubectl label nodes node-nuc8 tier=mongo
kubectl label nodes node-nuc1 tier=proxy-sgx
```

Manager deployment

You need to create persistent volumes for Jupyter notebook and Mongo database (see `charts/persistentVolume-mongoxp.yaml` and `charts/persistentVolume-jupyter.yaml`). Make the persistent volume target your github directory (in our case `/home/ubuntu/xp/PrivateRecSys`).
Note : in the current implementation you should update host address for the Prometheus Ingress manually in `values.yaml` to make it work:
```
cd manager
helm repo add elastic https://helm.elastic.co
helm dep update
helm install --name manager --namespace manager --set hostAddress=10-0-0-19.nip.io -n manager .
```
Access on Mongo Express (http://mongo.10-0-0-19.nip.io) and Jupyter (http://jupyter.10-0-0-19.nip.io). In order to remove the chart, you need to manually remove the Prometheus created CRD ( with command `kubectl delete crd --all`).

You can then download the dependencies and install:
```
cd ../private-recsys
helm dep update
helm install --name recsys --set hostAddress=10-0-0-19.nip.io .
```



Direct access to Harness via its [REST API](https://github.com/actionml/harness/blob/develop/docs/rest_spec.md) are possible via the subdomain path `harness`. For instance, a model training is possible with the following command:
```
curl -X POST  http://harness.10-0-0-19.nip.io/harness/engines/sgx_ur/jobs/
```
