TUN="tun1"
ADDR="10.0.0.1/24"
sudo openvpn --mktun --dev ${TUN}
sudo ip link set ${TUN} up
sudo ip addr add ${ADDR} dev ${TUN}
WH test
