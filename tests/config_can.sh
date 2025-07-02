#!/bin/bash
# Simple CAN configuration script

CAN_INTERFACE="can0"
CAN_BITRATE="500000"

case "$1" in
    "up")
        echo "Setting up CAN interface..."
        sudo ip link set $CAN_INTERFACE down 2>/dev/null
        sudo ip link set $CAN_INTERFACE type can bitrate $CAN_BITRATE
        sudo ip link set $CAN_INTERFACE up
        echo "CAN interface $CAN_INTERFACE is up"
        ;;
    "down")
        echo "Bringing down CAN interface..."
        sudo ip link set $CAN_INTERFACE down
        echo "CAN interface $CAN_INTERFACE is down"
        ;;
    "status")
        ip link show $CAN_INTERFACE
        ;;
    *)
        echo "Usage: $0 {up|down|status}"
        exit 1
        ;;
esac
