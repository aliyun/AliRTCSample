import Vue from 'vue'
import axios from 'axios'
import ElementUI from 'element-ui';
import 'element-ui/lib/theme-chalk/index.css';

import * as filters from './filters'

Object.keys(filters).forEach(key => {
  Vue.filter(key, filters[key])
})

import './assets/styles/basic.scss'

import './utils/array.js'

import App from './App'
// import router from './router'
import store from './store'

Vue.use(ElementUI, { size: 'mini', zIndex: 3000 });

if (!process.env.IS_WEB) Vue.use(require('vue-electron'))
Vue.http = Vue.prototype.$http = axios
Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
  components: { App },
  // router,
  store,
  template: '<App/>'
}).$mount('#app')
