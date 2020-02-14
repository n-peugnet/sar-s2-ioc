#!/bin/bash

if [ -z $2 ]
then
    echo "Usage:"
    echo "MPledbp.sh <led1,led2> <btn>"
    echo ""
    echo "Ex: MPledbp.sh 4,17 18"
    exit 0
fi

if [ $USER != "root" ]
then
    echo "Script must be run as root."
    exit -1
fi

./insdev MPledbp leds=$1 btn=$2