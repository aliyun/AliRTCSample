var E = function(id) { return document.getElementById(id); };

const serverUrl = E('serverUrl');
const joinBtn = E('joinBtn');
const leaveBtn = E('leaveBtn');
const log = E('log');
const isWsServer = E('isWsServer');

// const ws_url = 'ws://127.0.0.1:8000';
// const http_url = 'http://127.0.0.1:8000';

isWsServer.addEventListener('change', function() {
    var url = serverUrl.value
    if (this.checked) {
        url = url.replace("http://", "ws://");
    } else {
        url = url.replace("ws://", "http://");
    }
    serverUrl.value = url;
    console.info("isWsServer changed, checked: ", this.checked, "url: ", url);
});
joinBtn.addEventListener('click', joinChannel);
leaveBtn.addEventListener('click', leaveChannel);
E('clearLog').addEventListener('click', function() {
    log.innerHTML = '';
});

let websocket;

function getAppConfig() {
    const envOption = E('envOption').value;
    const channelId = E('channelId').value;
    const appId = E('appId').value;
    const userId = E('userId').value;
    const userName = E('userName').value;
    const token = E('token').value;
    
    const publishAudio = E('publishAudio').checked;
    const publishVideo = E('publishVideo').checked;
    const subscribeAllAudio = E('subscribeAllAudio').checked;
    const subscribeAllVideo = E('subscribeAllVideo').checked;
    const dumpPlaybackAudio = E('dumpPlaybackAudio').checked;
    const dumpRemoteVideo = E('dumpRemoteVideo').checked;
    
    const loopPlayCount = E('loopPlayCount').value;
    const pcmFilePath = E('pcmFilePath').value;
    const pcmSampleRate = E('pcmSampleRate').value;
    const pcmChannels = E('pcmChannels').value;
    const videoFilePath = E('videoFilePath').value;
    const videoPixelFormat = E('videoPixelFormat').value;
    const videoWidth = E('videoWidth').value;
    const videoHeight = E('videoHeight').value;
    const videoFps = E('videoFps').value;
    const videoRotation = E('videoRotation').value;

    env = envOption
    if (env == 'onertcPre') {
        appServer = "https://pre-onertc-demo-app-server.dingtalk.com";
    } else if (env == 'onertcOnline') {
        appServer = "https://onertc-demo-app-server.dingtalk.com";
    }

    const appConfig = {
        appServer,
        env,
        channelId,
        appId,
        userId,
        userName,
        token,
        publishAudio,
        publishVideo,
        subscribeAllAudio,
        subscribeAllVideo,
        dumpPlaybackAudio,
        dumpRemoteVideo,
        pcmFilePath,
        pcmSampleRate: parseInt(pcmSampleRate),
        pcmChannels: parseInt(pcmChannels),
        videoFilePath,
        videoPixelFormat,
        videoWidth: parseInt(videoWidth),
        videoHeight: parseInt(videoHeight),
        videoFps: parseInt(videoFps),
        videoRotation: parseInt(videoRotation),
        loopPlayCount: parseInt(loopPlayCount)
    }
    return appConfig;
}

function onServerMessage(message) {
    if ("datetime" in message) {
        log.innerHTML += `[${message.datetime}] `;
    }
    if (message.type === "OnJoinChannelResult") {
        if (message.result == 0) {
            log.innerHTML += `加入频道成功, elapsed: ${message.elapsed}` + '<br/>';
        } else {
            log.innerHTML += `加入频道失败, result: ${message.result}, elapsed: ${message.elapsed}` + '<br/>';
        }
    } else if ((message.type === "OnLeaveChannelResult")) {
        log.innerHTML += `离开频道, result: ${message.result}` + '<br/>';
        if (isWsServer.checked) {
            websocket.close();
        }
    } else if ((message.type === "OnRemoteUserOnLineNotify")) {
        log.innerHTML += `远端用户在线, uid: ${message.uid}, elapsed: ${message.elapsed}` + '<br/>';
    } else if ((message.type === "OnRemoteUserOffLineNotify")) {
        log.innerHTML += `远端用户离线, uid: ${message.uid}, reason: ${message.reason}` + '<br/>';
    } else if ((message.type === "OnUserAudioMuted")) {
        log.innerHTML += `用户mute音频, uid: ${message.uid}, isMute: ${message.isMute}` + '<br/>';
    } else if ((message.type === "OnUserVideoMuted")) {
        log.innerHTML += `用户mute视频, uid: ${message.uid}, isMute: ${message.isMute}, videoTrack: ${message.videoTrack}` + '<br/>';
    } else if ((message.type === "OnFirstLocalVideoFrameDrawn")) {
        log.innerHTML += `本地视频首帧渲染,  width: ${message.width}, height: ${message.height}, elapsed: ${message.elapsed}` + '<br/>';
    } else if ((message.type === "OnFirstRemoteVideoFrameDrawn")) {
        log.innerHTML += `远端视频首帧渲染, uid: ${message.uid}, videoTrack: ${message.videoTrack}, width: ${message.width}, height: ${message.height}, elapsed: ${message.elapsed}` + '<br/>';
    } else if ((message.type === "OnFirstVideoPacketSend")) {
        log.innerHTML += `视频首包发送, videoTrack: ${message.videoTrack}, timeCost: ${message.timeCost}` + '<br/>';
    } else if ((message.type === "OnFirstVideoPacketReceived")) {
        log.innerHTML += `视频首包到达, uid: ${message.uid}, videoTrack: ${message.videoTrack}, timeCost: ${message.timeCost}` + '<br/>';
    } else if ((message.type === "OnFirstVideoFrameReceived")) {
        log.innerHTML += `视频首帧到达, uid: ${message.uid}, videoTrack: ${message.videoTrack}, timeCost: ${message.timeCost}` + '<br/>';
    } else if ((message.type === "OnFirstAudioPacketSend")) {
        log.innerHTML += `音频首包发送, timeCost: ${message.timeCost}` + '<br/>';
    } else if ((message.type === "OnFirstAudioPacketReceived")) {
        log.innerHTML += `音频首包到达, uid: ${message.uid}, timeCost: ${message.timeCost}` + '<br/>';
    } else if ((message.type === "OnAudioPublishStateChanged")) {
        log.innerHTML += `音频流发布状态变化, oldState: ${message.oldState}, newState: ${message.newState}, elapsed: ${message.elapsed}` + '<br/>';
    } else if ((message.type === "OnVideoPublishStateChanged")) {
        log.innerHTML += `视频流发布状态变化, oldState: ${message.oldState}, newState: ${message.newState}, elapsed: ${message.elapsed}` + '<br/>';
    } else if ((message.type === "OnScreenSharePublishStateChanged")) {
        log.innerHTML += `屏幕共享流发布状态变化, oldState: ${message.oldState}, newState: ${message.newState}, elapsed: ${message.elapsed}` + '<br/>';
    } else if ((message.type === "OnAudioSubscribeStateChanged")) {
        log.innerHTML += `音频流订阅状态变化, uid: ${message.uid}, oldState: ${message.oldState}, newState: ${message.newState}, elapsed: ${message.elapsed}` + '<br/>';
    } else if ((message.type === "OnVideoSubscribeStateChanged")) {
        log.innerHTML += `视频流订阅状态变化, uid: ${message.uid}, oldState: ${message.oldState}, newState: ${message.newState}, elapsed: ${message.elapsed}` + '<br/>';
    } else if ((message.type === "OnScreenShareSubscribeStateChanged")) {
        log.innerHTML += `屏幕共享流订阅状态变化, uid: ${message.uid}, oldState: ${message.oldState}, newState: ${message.newState}, elapsed: ${message.elapsed}` + '<br/>';
    } else {
        log.innerHTML += `收到消息, type: ${message.type}` + '<br/>';
    }
}

function wsJoinChannel() {
    if (websocket instanceof WebSocket && websocket.readyState !== WebSocket.CLOSED) {
        return; // If the connection is already open or in process, do nothing.
    }

    websocket = new WebSocket(serverUrl.value);
    console.info('joinChannel, create websocket: ', websocket)

    websocket.onopen = function() {
        log.innerHTML += 'WebSocket连接成功.<br/>';

        const joinRequest = {
            type: 'joinChannel',
            appConfig: getAppConfig(),
        };

        joinBtn.disabled = true;
        leaveBtn.disabled = false;

        log.innerHTML += `正在加入频道...<br/>`;
        websocket.send(JSON.stringify(joinRequest));
    };

    websocket.onmessage = function(event) {
        console.info("WebSocket onmessage event: ", event);
        var message;
        try {
            message = JSON.parse(event.data);
            onServerMessage(message);
        } catch (e) {
            console.error("parse event data error: ", e);
        }
    };

    websocket.onerror = function(event) {
        console.info('websocket onerror, event: ', event)
        log.innerHTML += 'WebSocket发生错误.<br/>';
    };

    websocket.onclose = function(event) {
        console.info('websocket onclose, event: ', event)
        log.innerHTML += `WebSocket连接关闭.<br/>`;
        joinBtn.disabled = false;
        leaveBtn.disabled = true;
    }
}

function wsLeaveChannel() {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        log.innerHTML += `正在离开频道...<br/>`;
        // Send a message to leave channel before closing
        websocket.send(JSON.stringify({ type: "leaveChannel" }));
    }
}

function httpJoinChannel() {
    joinBtn.disabled = true;
    leaveBtn.disabled = false;
    log.innerHTML += `正在加入频道...<br/>`;

    const joinRequest = {
        type: 'joinChannel',
        appConfig: getAppConfig(),
    };
    
    fetch(serverUrl.value + '/api', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(joinRequest)
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }
        return response.json(); // Or response.text() if the response is not in JSON format
    })
    .then(data => {
        console.log('POST request succeeded with JSON response', data);
        onServerMessage(data);
    })
    .catch(error => {
        console.error('POST request failed', error);
    });
}

function httpLeaveChannel() {
    joinBtn.disabled = false;
    leaveBtn.disabled = true;
    log.innerHTML += `正在离开频道...<br/>`;

    const leaveRequest = {
        type: 'leaveChannel'
    };

    fetch(serverUrl.value + '/api', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(leaveRequest)
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }
        return response.json(); // Or response.text() if the response is not in JSON format
    })
    .then(data => {
        console.log('POST request succeeded with JSON response', data);
        onServerMessage(data);
    })
    .catch(error => {
        console.error('POST request failed', error);
    });
}

function joinChannel() {
    if (isWsServer.checked) {
        wsJoinChannel();
    } else {
        httpJoinChannel();
    }
}

function leaveChannel() {
    if (isWsServer.checked) {
        wsLeaveChannel();
    } else {
        httpLeaveChannel();
    }
}
