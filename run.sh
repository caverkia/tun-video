#! /bin/bash

localip=192.168.1.128
localport=8888

remoteip=192.168.1.187
remoteport=8888

dev=tun1

./tunudp ${dev} ${localip} ${localport} ${remoteip} ${remoteport}

