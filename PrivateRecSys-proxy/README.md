# PrivateRecSys

## How to setup and run PrivaterecSys HTTP proxy inside SGX using Docker

First, setup linux SGX driver on your host.
You can either follow the instructions on the [official repository](https://github.com/01org/linux-sgx-driver) or use the following script (Ubuntu only):

```bash
curl -fssl https://raw.githubusercontent.com/SconeDocs/SH/master/install_sgx_driver.sh | bash
```

Then, stop the aesmd service:

```bash
sudo service aesmd stop
```

Afterwards you can build, then launch the proxy using the following command:

```bash
docker run --device /dev/isgx --device /dev/mei0 -p 8081:8081 proxysgx "./app" [...]
```

## In case --device /dev/isgx fails

```bash
sudo service aesmd stop
sudo modprobe -r -f isgx
sudo rmmod -f isgx
sudo rm -rf /dev/isgx
```

And then reinstall

## SGX OpenSSL

THE ORDER OF LINKED LIBRARIES MATTERS... >_<

