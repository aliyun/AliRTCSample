import axios from "axios"
import sha256 from "sha256"
import { AppId, AppKey } from "../data/config"
export default class Utils {
  /**
    * 获取测试鉴权信息
    * @param {string} channel 
    * @returns 
    */
  static getRTCAuthInfo(channel) {
    let userId = 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
        let r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
        return v.toString(16);
    });// 可以自定义
    let timestamp = parseInt(new Date().getTime() / 1000 + 48 * 60 * 60);
    let nonce = 'AK-' + timestamp;
    let token = sha256(AppId + AppKey + channel + userId + nonce + timestamp);
    return {
        appid: AppId,
        userid: userId,
        timestamp: timestamp,
        nonce: nonce,
        token: token,
        gslb: ["https://rgslb.rtc.aliyuncs.com"],
        channel: channel
    };
  }
  
  // static getRTCAuthInfo(url, channel, displayName) {
  //   return new Promise(function (resolve, reject) {
  //     axios({
  //       method: 'post',
  //       url: url + "/login?room=" + channel + "&user=" + displayName + "&passwd=1234",
  //     }).then((re) => {
  //       re.data.data.channel = channel
  //       console.log(re)
  //       resolve(re.data.data);
  //     }).catch((err) => {
  //       if (err.status == 404) {
  //         err.message = "获取鉴权信息失败,可能是AppServer地址错误";
  //       }
  //       reject(err)
  //     });
  //   });
  // }
}