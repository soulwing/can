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
* `netns.c` -- is where the optional network namespace is set up
* `aufs.c` -- is where an AUFS filesystem is mounted consisting of numbered layers 
  organized as directories on another filesystem
* `tmpfs.c` -- is where _tmpfs_ filesystems are mounted for scratch spaces such as
  `/tmp` and `/var/run`

## Build

```
make clean all
```

## Run

By default, _can_ simply starts a process using isolated PID and mount namespaces, remounts the proc filesystem, and runs `/bin/sh`.

```
sudo ./can
```

### Options

Of course, running a process using isolated PID and mount namespaces is cool, but 
that's not all _can_ can do.

#### Running a different command

Even though the default is to run `/bin/sh`, you can run whatever you want in a can.

```
sudo ./can /bin/ps
```

If your command needs options, you'll need to tell _can_ where its options end 
using `--`.

```
sudo ./can -- /bin/ps -ef
```

#### Using `chroot`

If you set up a separate root filesystem somewhere on a mounted filesystem, you can
use it as the root when running a can. A convenient root filesystem to use is the one
for Alpine Linux. Get the Alpine Mini Root Filesystem from the [Alpine Linux Downloads page](https://alpinelinux.org/downloads/). It comes as a tarball.

Suppose you untarred the mini root filesystem in `/var/can/root`. You can use it as the root filesystem in a can like this:

```
sudo ./can --chroot /var/can/root
```

Note that when you do this, the command that is run in the can (default `/bin/sh`)
comes from root that you're using. So in this example, the shell you're running is the
busybox shell from Alpine Linux. In fact, any command that you run from within this 
example command is a busybox command.

#### Using a UTS Namespace

The UTS namespace contains the hostname and NIS domain name assigned to the host.
If you set a _hostname_ in a can, that hostname is known only in that can.

```
sudo ./can --hostname foobar /bin/hostname
```

#### Using a Named Network Namespace

_Can_ can join an existing network namespace. The following commands will create
and configure a network namespace named _ns0_ that contains a virtual ethernet
interface that can communicate with the outside world using NAT.

```
HOST_IF=eth0      # change this to your host's primary network interface
sudo ip netns add ns0
sudo ip link add veth0 type veth peer name eth0 netns ns0
sudo ip address add 10.0.0.1/24 dev veth0
sudo ip link set veth0 up
sudo ip netns exec ns0 ip address add 10.0.0.10/24 dev eth0
sudo ip netns exec ns0 ip link set eth0 up
sudo ip netns exec ns0 ip route add default via 10.0.0.1
sudo sysctl -w net.ipv4.ip_forward=1
sudo iptables -t nat -A POSTROUTING -s 10.0.0.0/24 -o ${HOST_IF} -j MASQUERADE
```

Once the network namespace is set up we, can use it in a can.

```
sudo ./can --netns ns0
```

In the shell for that can, examine the output of `ip link`, `ip address`, `ip route`,
and try pinging the Google nameserver at 8.8.8.8.

#### Simulating Docker Layers Using AUFS

_Can_ also knows how to simulate the concepts of filesystem layers for images and 
containers. It does this using AUFS.

Create a directory structure that will hold the layers ("branches" in AUFS-speak).

```
sudo mkdir -p /var/can/mnt
sudo mkdir -p /var/can/aufs/{image,config,container}
```

Inside the image directory create a base image layer, by creating a subdirectory
named `0` and untarring the [Alpine Mini Root Filesystem](https://alpinelinux.org/downloads/) inside of it.

You can leave the `config` and `container` layers empty for the moment.

Run _can_ and tell it to use your layered fileystem.

```
sudo ./can --chroot /var/can/mnt --fs /var/can/aufs
```

Inside of the can, if you run `mount` you'll see that your root filesystem is of type
`aufs`. 

If you create or edit a file anywhere on the filesystem, after you exit the
can you should see the file(s) you touched in `/var/can/aufs/container`. The 
`container` directory simulates Docker's writable container layer.

Let's do a more compelling demo.

After exiting the running can, blow away the container layer and create a new 
empty one.

```
sudo rm -fr /var/can/aufs/container
sudo mkdir -p /var/can/aufs/container
```

Set up the `ns0` network namespace as shown in [Using a Named Network Namespace](#using-a-named-etwork-namespace). 

Add a DNS resolver configuration file to the `config` directory:
```
sudo mkdir -p /var/can/aufs/config/etc
sudo echo "nameserver 8.8.8.8" >/var/can/aufs/config/etc/resolv.conf
```

Now run a can using the layered filesystem and the network namespace:
```
sudo ./can --chroot /var/can/mnt --fs /var/can/aufs --netns ns0
```

Inside of that can use _apk_ to install a package:
```
apk add --no-cache curl
curl --version
exit
```

After the can has exited, let's save the container layer from the filesystem as a new
image layer:

```
sudo mv /var/can/aufs/container /var/can/aufs/image/1
sudo mkdir /var/can/aufs/container
```

Now run a can again using the layered filesystem and the network namespace:
```
sudo ./can --chroot /var/can/mnt --fs /var/can/aufs --netns ns0
```

Inside of that can, you'll still have curl, but it's now part of the image and not
part of the writable container layer.

#### Using tmpfs

Inside of a can that uses _chroot_ you might want scratch space on the disk to be
backed by _tmpfs_ filesystems. You can do that with _can_ do.

```
sudo ./can --chroot /var/can/mnt --fs /var/can/aufs --tmpfs /tmp --tmpfs /run
```
