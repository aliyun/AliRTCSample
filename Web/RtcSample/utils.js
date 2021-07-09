/**
 * 初始化订阅状态
 * @param {String} userId 
 */
function initialization(userId) {
    if (aliWebrtc) {
        aliWebrtc.configRemoteAudio(userId, false);
        aliWebrtc.configRemoteCameraTrack(userId, false, false);
        aliWebrtc.configRemoteScreenTrack(userId, false);
    }
}

/**
 * 分析远端用户信息
 * @param {String} userId 
 */
function getSubscribeInfo(userId) {
    var userInfo = aliWebrtc.getUserInfo(userId);
    var subscribeInfo = [], subscribeInfoArr = [], isSubAudio = false, isSubLarge = false, isSubSmall = false, isSubCamera = false, isSubScreen = false, isSubVideo = false;
    if (userInfo) {
        userInfo.streamConfigs.forEach(v => {
        if (v.subscribed) {
            subscribeInfo.push(v.label);
            subscribeInfoArr.push(v);
            v.type == "audio" ? isSubAudio = true : "";
            v.type == "video" ? isSubVideo = true : "";
            v.label == "sophon_video_camera_large" ? isSubLarge = true : "";
            v.label == "sophon_video_camera_small" ? isSubSmall = true : "";
            v.label == "sophon_video_screen_share" ? isSubScreen = true : "";
            if (isSubLarge || isSubSmall) {
                isSubCamera = true;
            }
        }
        });
    }
    return { subscribeInfo: subscribeInfo, subscribeInfoArr: subscribeInfoArr, isSubLarge: isSubLarge, isSubSmall: isSubSmall, isSubCamera: isSubCamera, isSubAudio: isSubAudio, isSubScreen: isSubScreen, isSubVideo: isSubVideo };
}

/**
 * 取消订阅设置
 * @param {String} userId 
 * @param {String} type 
 * @param {String} label 
 */
function setConfigRemote(userId, type, label){
    return new Promise((resolve, reject) => {
        if (type == "audio") {
            aliWebrtc.configRemoteAudio(userId, false);
        } else {
            if (label === "sophon_video_camera_large") {
                aliWebrtc.configRemoteCameraTrack(userId, false, false);
                console.warn("取消相机流");
            } else if (label === "sophon_video_screen_share") {
                console.warn("取消共享流");
                aliWebrtc.configRemoteScreenTrack(userId, false);
            }
        }
    aliWebrtc.subscribe(userId).then(re => {
        resolve();
        }).catch(err => { console.error("重新订阅失败", err); alert(err.message); })
    });
}

/**
 * 显示提示信息，并打印
 * @param {String} text 要显示的信息
 * @param {String} type 信息类型(默认成功)
 * @param {Number} delay 延迟时间(默认2.5s)
 */
function showAlert(text,type,delay) {
    if(!text) return;
    var _type = type ?  "alert-" + type : "alert-success"
    var _delay = delay || 2500
    $(".alert").html(text).addClass(_type).show().delay(_delay).fadeOut("normal",() => {$(".alert").removeClass(_type)});
    if(_type === "warning"){
        console.warn(text)
    }else if(_type === "danger"){
        console.error(text)
    }else {
        console.log(text)
    }
}

/**
 * 根据属性值获取在数组中的index
 */
Array.prototype.getIndexByProprety = function (val, proprety) {
    var arr = this;
    var index = -1;
    arr.forEach((v, i, a) => {
        if (v[proprety] == val) {
            index = i;
        }
    });
    return index;
}

/**
 * 根据属性值获取数组中的某项
 */
Array.prototype.getObjByProprety = function (val, proprety) {
    var arr = this;
    var obj = {};
    arr.forEach((v, i, a) => {
        if (v[proprety] === val) {
            obj = v;
        }
    });
    return obj;
}