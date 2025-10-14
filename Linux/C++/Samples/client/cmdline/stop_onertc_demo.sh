#!/bin/bash

# Usage: ./stop_onertc_demo.sh [pid]

if [ -n "$1" ]; then
    PID="$1"
    echo "Your input pid: $PID"
fi

if [ -z "$PID" ]; then
    LOGDIR=onertc_log
    LATEST_PID_FILE="$(ls -Art ${LOGDIR}/*.pid | tail -n 1)"
    if [ -f ${LATEST_PID_FILE} ]; then
        PID=$(cat ${LATEST_PID_FILE})
        echo "Read from latest pid file: $LATEST_PID_FILE, pid: $PID"
    fi
fi

if [ -n "$PID" ]; then
    echo "Terminate PID: $PID"
    kill -15 $PID
fi
