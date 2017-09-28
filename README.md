can
===

Can is like a container, only not as nice.

### Build

```
make clean all
```

### Setup

Do all this stuff as root.

#### Filesystem
```
mkdir -p /var/can/mnt
mkdir -p /var/can/aufs/{container,config,layers}
```

#### Network namespace
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

#### Root fileystem layer

Get the Alpine Mini Root Filesystem (`alpine-minirootfs-3.6.2-x86_64.tar.gz`) from the [Alpine Linux Downloads page](https://alpinelinux.org/downloads/).

```
mkdir /var/can/aufs/layers/0
tar -C /var/can/aufs/layers/0 -zxpvf alpine-minirootfs-3.6.2-x86_64.tar.gz
```

#### Configuration layer

Create a `resolv.conf` file.

```
mkdir /var/can/aufs/config/etc
echo "nameserver 8.8.8.8"  >/var/can/aufs/config/etc/resolv.conf
```

### Run

```
sudo ./can
```


