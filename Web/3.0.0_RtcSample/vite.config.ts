import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    react(),
  ],
  resolve: {
    extensions: ['.js', '.ts', '.d.ts', '.tsx'],
  },
  server: {
    port: 5174,
    open: true,
    cors: true,
  },
});
