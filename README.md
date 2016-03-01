# dontfragment
Linux kernel and iptables modules to set/reset DF flag

Compilation
-----------
Execute make to download iptables 1.4.21 and compile modules for your kernel and iptables.
Usage
-----
1. modprobe x_tables
2. insmod ipt_DF.ko
3. Put libipt_DF.so to your iptables dynamic libraries folder (usually /lib/[ip,x]tables or /usr/lib/[ip,x]tables)
4. iptables -m mangle -A [PREROUTING/POSTROUTING] -j DF [--set|--reset]
