import { defineConfig, externalizeDepsPlugin } from 'electron-vite';
import path from 'node:path';
import { AntDesignVueResolver } from 'unplugin-vue-components/resolvers';
import Components from 'unplugin-vue-components/vite';
import commonjsExternals from 'vite-plugin-commonjs-externals';
import vue from '@vitejs/plugin-vue';
import vueJsx from '@vitejs/plugin-vue-jsx';

const commonjsPackages = [
  'electron',
  'electron/main',
  'dingrtc-electron-sdk',
] as const;

// https://vitejs.dev/config/
export default defineConfig({
  main: {
    plugins: [externalizeDepsPlugin()],
  },
  preload: {
    plugins: [externalizeDepsPlugin()],
  },
  renderer: {
    resolve: {
      extensions: ['.js', '.ts', '.d.ts', '.tsx', '.vue'],
      alias: {
        '~': path.join(__dirname, 'src/renderer/src'),
      },
    },
    plugins: [
      vue(),
      vueJsx(),
      Components({
        resolvers: [
          AntDesignVueResolver({
            importStyle: false, // css in js
          }),
        ],
      }),
      commonjsExternals(
        { externals: commonjsPackages },
      ),
    ],
  },
});
