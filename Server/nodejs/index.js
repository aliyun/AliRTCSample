const { AppToken } = require('./app_token');
const { Service } = require('./service');
const { AppTokenOptions } = require('./app_token_options');
const { ReadonlyByteBuffer } = require('./util/bytes_utils');



// find appId in your RTC console (https://rtc.console.aliyun.com/#/manage/list)
const appId = 'replace_your_appId';
// find appKey in your RTC console
const appKey = 'replace_your_appKey';
// Token is valid for a maximum of 24 hours. This example uses 12 hours, adjust according to your needs.
const expiredTs = Math.floor(Date.now() / 1000) + 12 * 60 * 60;
const channelId = 'replace_your_channelId';
const userId = 'replace_your_userId';

let appToken = new AppToken(appId, appKey, expiredTs);

// By default, all privileges are allowed. You can control audio/video/screen privileges individually as shown in the example below.
// Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail privilege informations.
// Example0: full privileges as default
let service = new Service(channelId, userId);
appToken.setService(service);
token = appToken.build();
console.log(token)
// Example1: only allow audio
// let service = new Service(channelId, userId);
// service.addAudioPublishPrivilege();
// appToken.setService(service);
// token = appToken.build();
// console.log(token)

// Example2: only allow audio and video
// let service = new Service(channelId, userId);
// service.addAudioPublishPrivilege();
// service.addVideoPublishPrivilege();
// appToken.setService(service);
// token = appToken.build();
// console.log(token)

// By default, no more additional options are set. You can set more options individually as shown in the example below.
// Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail options informations.
// Example1: add more options
// let service = new Service(channelId, userId);
// appToken.setService(service);
// let options = {
//     "duration_per_channel": "86400",
//     "delay_close_per_channel": "60"
// };
// let tokenOptions = new AppTokenOptions(options);
// appToken.setOptions(tokenOptions);
// token = appToken.build();
// console.log(token)
