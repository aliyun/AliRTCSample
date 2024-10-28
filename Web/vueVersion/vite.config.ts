import path from 'path';
import { AntDesignVueResolver } from 'unplugin-vue-components/resolvers';
import Components from 'unplugin-vue-components/vite';
import { defineConfig } from 'vite';
import commonjs from 'vite-plugin-commonjs';

import vue from '@vitejs/plugin-vue';

import getPublicPath from './webpack/getPublicPath';

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    vue(),

    Components({
      resolvers: [
        AntDesignVueResolver({
          importStyle: false, // css in js
        }),
      ],
    }),
    commonjs(),
  ],
  define: {
    local: true,
  },
  base: getPublicPath(),
  build: {
    outDir: './dist',
  },
  resolve: {
    extensions: ['.js', '.ts', '.d.ts', '.tsx'],
    alias: {
      '@src': path.resolve(__dirname, 'src'),
    },
  },
});
