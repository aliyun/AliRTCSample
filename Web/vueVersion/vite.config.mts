import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import vueJsx from '@vitejs/plugin-vue-jsx';
import Components from 'unplugin-vue-components/vite';
import { AntDesignVueResolver } from 'unplugin-vue-components/resolvers';
import path from 'node:path';
import fs from 'fs';

import commonjs from 'vite-plugin-commonjs';

// 默认使用 http 启动；若根目录下放置了 key.pem / cert.pem，则自动切换为 https。
// 浏览器要求安全上下文才能访问摄像头麦克风，localhost 下 http 即可满足；
// 若需要用局域网 IP 在其他设备上访问，请自行生成证书，例如：
// openssl req -x509 -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 365 -subj "/CN=localhost"
const keyPath = path.resolve(__dirname, 'key.pem');
const certPath = path.resolve(__dirname, 'cert.pem');
const httpsOptions =
  fs.existsSync(keyPath) && fs.existsSync(certPath)
    ? { key: fs.readFileSync(keyPath), cert: fs.readFileSync(certPath) }
    : undefined;

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    vueJsx(),
    commonjs(),
    Components({
      resolvers: [
        AntDesignVueResolver({
          importStyle: false, // css in js
        }),
      ],
    }),
  ],
  base: '/',
  define: {
    local: true,
  },
  resolve: {
    extensions: ['.js', '.ts', '.d.ts', '.tsx', '.vue'],
    alias: {
      '~': path.join(__dirname, 'src'),
    },
  },
  server: {
    open: true,
    cors: true,
    port: 8084,
    https: httpsOptions,
  },
  build: {
    // 禁用代码压缩
    minify: false,

    // 启用 source maps
    sourcemap: true,
  },
});
