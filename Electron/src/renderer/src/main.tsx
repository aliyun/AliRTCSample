import { createPinia } from 'pinia';
import { createApp } from 'vue';

import App from './pages/App/App.vue';
// import './style.css';

const pinia = createPinia();
const app = createApp(App);

app.use(pinia);
createApp(App).mount('#root');
