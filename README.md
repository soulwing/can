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
mkdir -p /var/can/{container,config,layers}
```

#### Network namespace
```
ip netns add ns0
ip link add veth0 type veth peer name eth0 netns ns0
ip address add 10.0.0.1/24 dev veth0
ip link set veth0 up
ip netns exec ns0 ip address add 10.0.0.10/24 dev eth0
ip netns exec ns0 ip link set eth0 up
ip netns exec ns0 ip route add default via 10.0.0.1
sysctl -w net.ipv4.ip_forward=1
iptables -t nat -A POSTROUTING -s 10.0.0.0/24 -o eth0 -j MASQUERADE
```

**Replace *eth0* in the last command with your host's primary network interface**

#### Root fileystem layer

Untar `alpine-minirootfs-3.6.2-x86_64.tar.gz` in a directory named `0` in the `layers` directory.

Put an `etc/resolv.conf` file in `/var/can/config`.

### Run

```
sudo ./can
```


