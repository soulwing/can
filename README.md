can
===

A _can_ is like a container, only not as nice.

The purpose of _can_ is to help demonstrate the salient features of Linux namespaces
and their application to container systems such as [Docker](https://docker.com) in 
a simple demo written in C. _Can_ demonstrates the use of many of the Linux namespace types, as well as related features such _chroot_, layered union filesystems (_aufs_),
and _tmpfs_. 

_Can_ sets up a simple container-like environment using a layered fileystem and chroot,
and runs in isolated process ID and mount namespaces. Optionally, the container can 
use an existing isolated network namespace and can create its own UTS namespace for
setting a hostname in the container.

## Guide to the Source Code

These are the main files to peruse to understand how namespaces are used to implement
containers.

* `can.c` -- is the main entry point to the demo; this is where most of the namespace
  setup mechanics happen
* `aufs.c` -- is where an AUFS filesystem is mounted consisting of numbered layers 
  organized as directories on another filesystem
* `netns.c` -- is where the optional network namespace is set up
* `tmpfs.c` -- is where _tmpfs_ filesystems are mounted for scratch spaces such as
  `/tmp` and `/var/run`

## Build

```
make clean all
```

## Setup

Do all this stuff as root.

### Filesystem
```
mkdir -p /var/can/mnt
mkdir -p /var/can/aufs/{container,config,layers}
```

### Network namespace
```
HOST_IF=eth0      # change this to your host's primary network interface
ip netns add ns0
ip link add veth0 type veth peer name eth0 netns ns0
ip address add 10.0.0.1/24 dev veth0
ip link set veth0 up
ip netns exec ns0 ip address add 10.0.0.10/24 dev eth0
ip netns exec ns0 ip link set eth0 up
ip netns exec ns0 ip route add default via 10.0.0.1
sysctl -w net.ipv4.ip_forward=1
iptables -t nat -A POSTROUTING -s 10.0.0.0/24 -o ${HOST_IF} -j MASQUERADE
```

### Root fileystem layer

Get the Alpine Mini Root Filesystem (`alpine-minirootfs-3.6.2-x86_64.tar.gz`) from the [Alpine Linux Downloads page](https://alpinelinux.org/downloads/).

```
mkdir /var/can/aufs/layers/0
tar -C /var/can/aufs/layers/0 -zxpvf alpine-minirootfs-3.6.2-x86_64.tar.gz
```

### Configuration layer

Create a `resolv.conf` file.

```
mkdir /var/can/aufs/config/etc
echo "nameserver 8.8.8.8"  >/var/can/aufs/config/etc/resolv.conf
```

## Run

```
sudo ./can
```


