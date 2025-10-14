#! /bin/bash
# 
# Created by yuqilin on 03/21/2024
# Copyright (C) 2024 yuqilin <yuqilin>
#

DATE_TIME=$(date '+%Y%m%d%H%M%S')
LOGDIR=onertc_log

mkdir -p $LOGDIR

APP_PATH=./onertc_demo
# APP_PATH=./out/Release/onertc_demo
APP_NAME=$(basename ${APP_PATH})

if [ ! -f ${APP_PATH} ]; then
    echo "App not found: $APP_PATH"
    exit 127
fi

nohup ${APP_PATH} --basic-test --log-dir=${LOGDIR} > ${LOGDIR}/nohup_${DATE_TIME}.log 2>&1 &
PID=$!
sleep 0.2
PROCESS_NAME=$(ps -p $PID -o comm=)

echo "PID=$PID, PROCESS_NAME=$PROCESS_NAME"

if [ "${PROCESS_NAME}" = "${APP_NAME}" ]; then
    echo "Start ${APP_NAME} success, running pid: ${PID}, log dir: ${LOGDIR}"
    echo $PID > ${LOGDIR}/onertc_demo_${DATE_TIME}.pid
else
    echo "Start ${APP_NAME} failed!"
fi
