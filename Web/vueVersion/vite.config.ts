import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import vueJsx from '@vitejs/plugin-vue-jsx'
import Components from 'unplugin-vue-components/vite';
import { AntDesignVueResolver } from 'unplugin-vue-components/resolvers';
import getPublicPath from './getPublicPath';
import path from 'node:path';

import commonjs from 'vite-plugin-commonjs';

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
  base: getPublicPath(),
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
  },
});