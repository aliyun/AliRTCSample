# dingrtc web sdk vue demo

## 本地体验

### 启动项目

```
npm install
npm run dev
```

## 配置参数

### 当前有两种方式来配置入会参数：

1. 在阿里云控制台生成token，再将控制台生成token的参数一一对应设置在src/config.json，这种模式下可以不填写其中的appKey，token和其余信息是一一对应的需要完全匹配；
2. 在阿里云控制台获取到对应的appKey，将其和对应的appId配置在src/config.json，其他参数皆为可选，不填写时会默认生成随机值。这种模式会使用demo自带的node server 生成一个token 用来入会，对应代码在server/server.js。后续需要调试生产模式时可在src/utils/request.ts修改APP_SERVER_DOMAIN为自己的业务服务器地址，并调整getAppToken里的逻辑来适配。
