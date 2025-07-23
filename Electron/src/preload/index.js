const { ipcRenderer } = require('electron');
 
ipcRenderer.on('crash-file-path', (event, args) => {
  console.warn('crash-file-path:', args);
});