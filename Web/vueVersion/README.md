# dingrtc web sdk demo

## 本地体验

#### 启动项目

```bash
$ npm install
$ npm run start
```

`npm run start` 会同时启动两个进程：vite 开发服务器（默认 http://localhost:8084）和用于签发 token 的本地 node 服务（http://localhost:3001，代码见 `server/server.js`）。

> 摄像头和麦克风需要浏览器的安全上下文才能访问。用 `localhost` 打开时 http 即可满足；如果需要用局域网 IP 在手机等其他设备上访问，请在项目根目录放置 `key.pem` 和 `cert.pem`，vite 检测到后会自动切换成 https：
>
> ```bash
> $ openssl req -x509 -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 365 -subj "/CN=localhost"
> ```

#### 配置参数

##### 当前有两种方式来配置入会参数：
1. 在阿里云控制台生成token，再将控制台生成token的参数一一对应设置在src/config.json，这种模式下可以不填写其中的appKey，token和其余信息是一一对应的需要完全匹配；
2. 在阿里云控制台获取到对应的appKey，将其和对应的appId配置在src/config.json，其他参数皆为可选，不填写时会默认生成随机值。这种模式会使用demo自带的node server 生成一个token 用来入会，对应代码在server/server.js。后续需要调试生产模式时可在src/utils/request.ts修改APP_SERVER_DOMAIN为自己的业务服务器地址，并调整getAppToken里的逻辑来适配。
