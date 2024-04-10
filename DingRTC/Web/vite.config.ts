import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

const path = require('path');

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    react(),
  ],
  resolve: {
    extensions: ['.js', '.ts', '.d.ts', '.tsx'],
    alias: {
      '~': path.join(__dirname, 'src'),
    },
  },
  server: {
    port: 5174,
    open: true,
    cors: true,
  },
});
