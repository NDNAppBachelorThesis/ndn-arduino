#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Error: Please provide the last part of the IP and the NDN prefix."
    exit 1
fi

ip=$1
ndnPrefix=$2

faceId=$(nfdc face create remote udp4://$ip:6363 persistency permanent | grep -o "id=[0-9]*" | awk -F '=' '{print $2}')

nfdc route add $ndnPrefix $faceId origin 123

echo "Added $ndnPrefix for IP $ip to face $faceId."