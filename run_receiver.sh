#! /bin/bash

localip=192.168.1.187
localport=8888

remoteip=192.168.1.128
remoteport=8888

dev=tun2

./tunudp ${dev} ${localip} ${localport} ${remoteip} ${remoteport}

