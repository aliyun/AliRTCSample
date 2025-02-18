import { createPinia } from 'pinia';
import { createApp } from 'vue';
import { RtcWhiteboard, WhiteboardManager } from '@dingrtc/whiteboard';
import PdfPlugin from '@dingrtc/whiteboard/PdfPlugin';
import App from './pages/App/App.vue';
// import './style.css';

RtcWhiteboard.usePdf(PdfPlugin)

const pinia = createPinia();
const app = createApp(App);

app.use(pinia);
createApp(App).mount('#root');

declare global {
  interface Window {
    whiteboard: RtcWhiteboard;
    whiteboardManager: WhiteboardManager;
  }
}
