
# PProx

## Description

Source code of PProx proxy, and experimentation scripts from the Middleware'21 paper: PProx: Efficient Privacy for Recommendation-as-a-Service.
If you want to use this code, please cite the paper:
```
Rosinosky, G., Da Silva, S., Ben Mokhtar, S., Négru, D., Réveillère, L., & Rivière, E. (2021, November). PProx: efficient privacy for recommendation-as-a-service. In Proceedings of the 22nd International Middleware Conference (pp. 14-26).
```

ACM Middleware 2021 presentation video: https://www.youtube.com/watch?v=WTJfoH6jLd4

## Abstract

We present _PProx_, a system preventing recommendation-as-a-service (RaaS) providers from accessing sensitive data about the users of applications leveraging their services.
_PProx_ does not impact recommendations accuracy, is compatible with arbitrary recommendation algorithms, and has minimal deployment requirements.
Its design combines two proxying layers directly running inside SGX enclaves at the RaaS provider side.
These layers transparently pseudonymize users and items and hide links between the two, and _PProx_ privacy guarantees are robust even to the corruption of one of these enclaves.
We integrated _PProx_ with the Harness recommendation engine and evaluated it on a 27-node cluster.
Our results indicate its ability to withstand a high number of requests with low end-to-end latency, horizontally scaling up to match increasing workloads of recommendations.

## Repository content

In this repository, we propose PProx source code, helper scripts for deployment on Docker Compose and Kubernetes using Helm charts.
We also propose load testing scripts for tests on [Harness](https://github.com/actionml/harness) and [Universal Recommender](https://actionml.com/docs/h_ur) as a recommendation system (macro benchmarks), and on a "stub" installation comprised of an Nginx server sending back a fixed recommendation result (microbenchmarks).  

The repository directories content is the following:

- [PrivateRecSys-proxy/](PrivateRecSys-proxy/) source code of PProx
- [harness-recommender/](harness-recommender/): Dockerfile and source code of helper scripts used for Harness/Universal Recommender initialization
- [stub/](stub/): Dockerfile and source code of the Nginx based stub, stub proxy and stub client
- [manager-scripts/](manager-scripts/): initialization scripts and Jupyter notebooks for load tests
- [charts/](charts/): Helm charts for the deployment of Harness/Universal Recommender coupled to PProx, stub coupled to PProx, and injector used for load testing
- [paper/plots](paper/plots): results datasets and generation scripts
- [ManagerKubernetesExperimentations/](ManagerKubernetesExperimentations/): chart for installation of required modules for experimentations orchestration of load tests, and logging and storage of results (Jupyter, Fluentd, MongoDB)

## Deployment with Kubernetes

PProx has been tested on Kubernetes v1.15.9-beta.0. You need to have a working cluster with SGX-able nodes for PProx to make it work. See [details-install.md](details-install.md) for more details on how to install an on-premises Kubernetes cluster using [Kubespray](https://github.com/kubernetes-sigs/kubespray#quick-start).

Load tests can be done on a Harness installation, or a "stub" installation using Nginx to emulate a recommendation system. We propose Helm charts for the deployment of the different tested modules, and the execution of the load testing scenarios.

In the following, we assume the user uses an on-premises Kubernetes cluster (_in the current state, only [hostPath based volumes](https://kubernetes.io/docs/concepts/storage/volumes/#hostpath) are supported by the experimentation orchestration scripts_), and has a dedicated "manager" node, used for the orchestration and storage of results. The user should have SSH access to this node, and use it for the manipulations.

The stub stack is used for microbenchmarks to emulate a recommendation system with a web server. It is available in the [charts/stub-recsys](charts/stub-recsys) directory, and is composed of the following sub-charts :
| Component | Version | Chart | Version | Source code | Origin |
|--|--|--|--|--|--|
| proxy-sgx | - | proxy-sgx | | [charts/stub-recsys/charts/proxy-sgx](charts/stub-recsys/charts/proxy-sgx) | |
| stub-recsys | - | stub-recsys | | [charts/stub-recsys/charts/stub-recsys](charts/stub-recsys/charts/stub-recsys) | |

The Harness/Universal recommender chart is available in the [charts/private-recsys](charts/private-recsys) directory, and is composed of the following sub-charts:

| Component | Version | Chart | Version | Source code | Origin |
|--|--|--|--|--|--|
| Mongo Database | 4.0.12-debian-9-r7 | mongodb | 5.16.4 | https://github.com/helm/charts/tree/master/stable/mongodb | https://charts.helm.sh/stable |
| ElasticSearch | 5.6.16 |elasticsearch | 7.1.1 | https://github.com/elastic/helm-charts/tree/master/elasticsearch | https://helm.elastic.co |
| Spark | 2.3.3  | actionml/spark | latest | [charts/private-recsys/charts/spark](charts/private-recsys/charts/spark) |  |
| harness | develop-2020-03-11 | harness | | [charts/private-recsys/charts/harness](charts/private-recsys/charts/harness) | |
| proxy-sgx | - | proxy-sgx | | [charts/private-recsys/charts/proxy-sgx](charts/private-recsys/charts/proxy-sgx) | |

For both configurations (stub and Harness), an [injector chart](charts/injector) based on [loadtest 3.0.8](https://www.npmjs.com/package/loadtest/v/3.0.8) library is used to emulate users with multiple levels of Requests Per Second (RPS).

To orchestrate experimentations, we use a management stack used on the `manager` node:

| Component | Version | Chart | Version | Source code | Origin |
|--|--|--|--|--|--|
|fluentd|v1.7-debian-1|fluentd| |[ManagerKubernetesExperimentations/chart/charts/fluentd](ManagerKubernetesExperimentations/chart/charts/fluentd) | |
|jupyter|latest|jupyter||[ManagerKubernetesExperimentations/chart/charts/jupyter](ManagerKubernetesExperimentations/chart/charts/jupyter)| |
| Mongo Database | 4.0.12-debian-9-r7 | mongodb | 4.10.1 | [https://github.com/helm/charts/tree/master/stable/mongodb](https://github.com/helm/charts/tree/master/stable/mongodb) | [https://charts.helm.sh/stable](https://charts.helm.sh/stable) |
|mongo-express|0.49|mongo-express | |[ManagerKubernetesExperimentations/chart/charts/mongo-express](ManagerKubernetesExperimentations/chart/charts/mongo-express) |

### SGX Driver

Each SGX node must have the SGX driver installed. Launch the following commands and _reboot_ the node :

```bash
curl -fssl https://raw.githubusercontent.com/SconeDocs/SH/master/install_sgx_driver.sh | bash
# sudo service aesmd stop # seems not needed anyore
```

If you use Ansible (as in Kubespray in the last part), you can use the following one-liner to stop the service on every node (you can replace `all` by a comma separated list of nodes):

```bash
ansible -i inventory/cluster_nuc/hosts.yml -m shell -a 'curl -fssl https://raw.githubusercontent.com/SconeDocs/SH/master/install_sgx_driver.sh | bash' all
# ansible -i inventory/cluster_nuc/hosts.yml -m shell -a 'sudo service aesmd stop' --become -K all #seems not needed anymore
```

Helper initialization scripts are located in [manager-scripts/init](manager-scripts/init). Please go in this directory.

We need the cluster to be able to cope with SGX resources. We use a DaemonSet plugin developed by Vaucher et al., available here: <https://github.com/sebva/sgx-device-plugin>. Deploy it on the cluster from the manager node using:

```bash
kubectl apply -f ./sgx_deviceplugin.yaml
```

### Setting up the cluster

Still from the manager node, you need now to set up the node labelling: Each node will be given a role (proxy, injector, manager, etc.) for each needed module for the experimentation. Please prepare carefully a script using [this](manager-scripts/init/label_nodes-mini-cluster.sh) template, and execute it. The following labels should be defined:

- manager (1 node, __preferably with SSD__)
- pprox (a multiple of 2, with SGX support.)
- harness (used for Harness and stub modules, any needed number of nodes)
- mongo (1 node, __preferably with SSD__)
- elastic (1, 3, 5 or 7 nodes, __preferably with SSD__)
- injector (any needed number of nodes)

Appropriate directories for the Mongo database used for experiment results should be set in [manager-scripts/init/persistentVolume-mongoxp.yaml](manager-scripts/init/persistentVolume-mongoxp.yaml), and the location of the source code repository should be set in [manager-scripts/init/persistentVolume-jupyter.yaml](manager-scripts/init/persistentVolume-jupyter.yaml). These are set respectively to `/home/ubuntu/xp/volumes/mongo` and `/home/ubuntu/xp/PrivateRecSys` in the current files.
The `mongo` directory should be previously created with `777` rights (with `chmod -R`). Every script directory that the user intends to modify with Jupyter should be set to `777` as well, or modifications will not be possible (more specifically [work/manager-scripts/test](manager-scripts/test) and [work/manager-scripts/xp](manager-scripts/xp) directories for experimentation launches).

Now, to the installation of all manager dependencies. The only needed parameter is the address of the manager's Ingress (will be the point of entry for access to the Mongo, Jupyter of the manager). Modify the `values.yaml` and set `hostAddress` to a valid DNS name pointing to the node manager (by default, `10-0-0-33.nip.io`). Note that in our case we use the online [nip.io](https://nip.io/) service permitting to map a DNS name to a local address.

Launch the following scripts (sudo might help):

```bash
./init-cluster.sh
./init-manager.sh
```

An available registry on `localhost:32000` (using [kube-registry-proxy](https://hub.docker.com/r/solsson/kube-registry-proxy/) should have been deployed by the initialization scripts. You can now build all Docker images and push (default to Docker repository).

```bash
docker-compose -f ./docker-compose-build.yaml build
docker-compose -f ./docker-compose-build.yaml push
```

For tests with Harness, you need to download the MovieLens dataset that will be used for the experiments. A helper script is available for this intent:

```bash
cd manager-scripts/datasets
python3 ./generate-dataset-json.py
```

Wait for a few minutes for the dataset to be generated.

You can now access the different modules using a browser and the fixed Ingress DNS entry (replace the `10-0-0-33` address with your own):

- Jupyter: <http://jupyter.10-0-0-33.nip.io/lab/>
- Mongo: <http://mongo.10-0-0-33.nip.io/lab/>

### Test

Two notebooks are proposed for load test scenario launches: one for Harness [manager-scripts/test/debug-harness-install.ipynb](manager-scripts/test/debug-harness-install.ipynb) and one for the stub [manager-scripts/test/debug-stub-install.ipynb](manager-scripts/test/debug-stub-install.ipynb).
They can be found in the Jupyter notebook in the [work/manager-scripts/test](manager-scripts/test) directory.

Both launch notebooks are organized the same way: global parameters set, help functions and experiments launches.
The user should set the `hostAddress` in the first cell to the actual DNS name of the manager node (the same used for access to Jupyter, i.e. `10-0-0-33.nip.io`)
Initialization cells should then be launched sequentially, and the desired test cells should be launched manually.

Note: For the Harness scenario, please consider using 3 replicas for Elastic, as the used version does not deploy successfully half of the time when using 1 or an odd number of replicas.

### Launching an experiment

In each experiment launch cell, the parameters are organized as maps of maps, and used for the generation of the Helm install commands. Sub-chart parameters are generated by replacing "!" with ".". Loops on parameters permit to chain multiple experiments.
For more details, please check the corresponding charts.
For each launched experiment, an additional entry is generated in the metadata collection `experiment` in the `privaterecsys` database in the experiment Mongo database.
 containing all the used parameters.

#### Global parameter map

| Parameter | Role |
|--|--|
|`stub`|"1" if SUT is stub, "0" if SUT (System Under Test) is Harness|
|`start_time`|Start timestamp, used to name experiments with name|
|`params_recsys`|PProx and SUT parameters map|
|`params_injector`|Injector parameters map|
|`name`|Name of the experiment|
|`status`|Initial status of the experiment|


#### PProx and SUT (Stub or Harness) parameter map (params_recsys)

| Parameter | Role |
|--|--|
|`hostAddress`| manager name |
|`harness!replicaCount`| number of Harness or stub replicas |
|`proxy-sgx!replicaCount`| number of PProx replicas per layer |
|`proxy-sgx!proxyType1`|First proxy layer ("1" = UA, "2" = IA) |
|`proxy-sgx!proxyType2`|Second proxy layer ("1" = UA, "2" = IA) |
|`proxy-sgx!targetPort`|Port of SUT|
|`proxy-sgx!enabledEncryption`|Encryption enabled in proxies|
|`proxy-sgx!bufferShuffling`|Size of buffer shuffling (1 = disabled)|
|`proxy-sgx!responseTimeout`|Delay before PProx sends a response anyways|
|`proxy-sgx!debug`|PProx debug mode|
|`proxy-sgx!image!repository`| Used image (can be localhost:32000/sgx-proxy or localhost:32000/sgx-proxy-nosgx)|
|`name`|Name of the experiment|
|`timeout`|Chart deployment timeout|
|Harness specific parameters|
|`elastic!replicas`|Number of Elastic replicas|
|`elastic!esJavaOpts`|Java opts. Set -Xmx and -Xms to the maximum available memory in Elastic nodes.|
|`elastic!resources!requests!cpu`|Elastic CPU requests|
|`elastic!resources!requests!memory`|Elastic memory request|
|`elastic!resources!limits!cpu`|Elastic CPU limits|
|`elastic!resources!limits!memory`|Elastic memory limits|
|`mongo!replicaSet!enabled`|Usage of replicaset in Mongo, please set to "false"|
|`mongo!clusterDomain`|Name of the cluster (set to your cluster name)|

#### Injector parameter map (params_injector)

| Parameter | Role |
|--|--|
|`harness`| "true" if SUT is Harness, "false" if SUT is stub|
|`harnessReplicas`|number of Harness replicas|
|`duration`|Duration of injection|
|`clients`|Number of parallel clients (equivalent to injected RPS)|
|`replicaCount`|Number of injectors|
|`targetHost`|target host (can be PProx or SUT)|
|`concurrency`|Concurrency (see [loadtest documentation](https://www.npmjs.com/package/loadtest) for more information) |
|`timeout`|Chart deployment timeout|
|`asyncFluentd`|"true" if results are sent to fluentd during experiments, "false" if the injector waits for the end before sending it. Set to "false" in all experiments.|
|`enabledEncryption`|Encrypted entries injected if "1", raw entries if "0". Use with `proxy-sgx!enabledEncryption`|
|`randomShift`|not used|
|`name`|Name of the experiment|

Load testing results are aggregated in the `private-recsys` database in Mongo, and then archived to another database (name depend on the used test, see the code for details).
A statictics notebook is available [manager-scripts/test/debug-stats.ipynb](manager-scripts/test/debug-stats.ipynb). It reads from a target database (`private-recsys` by default), and proposes some general statistics and plots.

## Middleware paper experimentations

Experimentations launched in the Middleware 2021 paper are available in the [work/manager-scripts/xp](manager-scripts/xp) directory. You can find it the following notebooks, following the same organization and map parameters as the install test scripts:

- [micro1-9.ipynb](manager-scripts/xp/micro1-9.ipynb) for the microbenchmark (stub) load test launch scripts
- [macro10-12.ipynb](manager-scripts/xp/macro10-12.ipynb) for the macrobenchmark (Harness) load test launch scripts

The [statistics notebook](manager-scripts/test/debug-stats.ipynb) can be used as well with the results of these experiments. The corresponding Mongo database and wanted parameters should be set.

An export notebook [raw-latencies/export-raw-RTT.ipynb](manager-scripts/xp/raw-latencies/export-raw-RTT.ipynb) can be used to generate CSV files for the experimentations results. The results should be used accordingly with the Gnuplot scripts [paper/plots/only_harness_cut/plot_all.sh](paper/plots/only_harness_cut/plot_all.sh) and [paper/plots/only_harness_cut/plot_all.sh](paper/plots/only_harness_cut/plot_all.sh) 
