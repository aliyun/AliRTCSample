import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
import commonjs from 'vite-plugin-commonjs';

const path = require('path');


// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    react(),
    commonjs(),
  ],
  resolve: {
    extensions: ['.js', '.ts', '.d.ts', '.tsx'],
    alias: {
      'public': path.join(__dirname, 'public'),
      '~': path.join(__dirname, 'src'),
    },
  },
  server: {
    port: 5174,
    open: true,
    cors: true,
  },
});