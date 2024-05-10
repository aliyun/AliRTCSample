const { AppToken } = require('./app_token');
const { Service } = require('./service');
const { AppTokenOptions } = require('./app_token_options');
const { ReadonlyByteBuffer } = require('./util/bytes_utils');

const channelId = 'channelId';
const userId = 'userId';
let service = new Service(channelId, userId);
service.addVideoPublishPrivilege();

// let servicePack = service.pack()
// service = Service.unpack(new ReadonlyByteBuffer(servicePack))

let options = new AppTokenOptions();
options.setEngineOptions({ k1: 'v1', k2: 'v2' });
// let optionsPack = options.pack()
// options = AppTokenOptions.unpack(new ReadonlyByteBuffer(optionsPack))

const appId = 'appId';
const appKey = 'appKey';
const timestamp = Math.floor(Date.now() / 1000);
let appToken = new AppToken(appId, appKey, timestamp);

appToken.setOptions(options);
appToken.setService(service);

token = appToken.build();

console.log(token)

// token = '000eJxjYGBQuLFA92RB7WvHmxoN/526bRvnnN/kIPEogzHRkLf1yNUiLQYGBtbEggLPlDQx9uMwDBTkTM5IzMtLzfFMAXLYSotTizxTGIFMZhDBBMLZhiCyDExmG4HZIHIUDBoAAItnHjU='

appToken = AppToken.parse(token)

console.log(appToken.validate(appKey));