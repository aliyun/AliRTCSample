const { app, BrowserWindow, ipcMain, systemPreferences, session } = require("electron");
const path = require("path");
const { crashReporter } = require('electron')
const os = require('node:os')

let vueDevToolsPath

crashReporter.start({
  uploadToServer: false,
  ignoreSystemCrashHandler: false,
})

process.on('uncaughtException', (error) => {
  console.log(error);
  // 你可以在这里添加额外的错误处理逻辑
});

function createWindow() {
  // 创建浏览器窗口
  const win = new BrowserWindow({
    width: 1200,
    height: 800,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
      preload: path.join(__dirname, "../preload/index.js"),
    },
  });

  win.webContents.openDevTools({
    mode: 'detach',
    activate: true,
  });
  
  if (!app.isPackaged && process.env['ELECTRON_RENDERER_URL']) {
    vueDevToolsPath = path.join(
      os.homedir(),
      '/Library/Application Support/Google/Chrome/Default/Extensions/nhdogjmejiglipccpnnnanhbledajbpd/7.6.5_0'
    )
    win.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    win.loadFile(path.join(__dirname, '../renderer/index.html'))
  }

  win.webContents.once('did-finish-load', async () => {
    ipcMain.handle('IPC_REQUEST_PERMISSION_HANDLER', async (event, arg) => {
      if (
        systemPreferences.getMediaAccessStatus(arg.type) === 'not-determined'
      ) {
        console.log('main process request handler:' + JSON.stringify(arg));
        return await systemPreferences.askForMediaAccess(arg.type);
      }
    });
  });
  win.webContents.on('devtools-opened', () => {
    win.focus();
    setImmediate(() => {
      win.focus();
    });
  });

  win.webContents.on('render-process-gone', (...args) => {
    const heap = process.getHeapStatistics()
    const usedHeapSize = Number((heap.usedHeapSize / 1024).toFixed(2))
    const heapSizeLimit = Number((heap.heapSizeLimit / 1024).toFixed(2))
    console.log(`usedHeapSize:${usedHeapSize},heapSizeLimit:${heapSizeLimit}`)
    console.warn('app:render-process-gone', JSON.stringify(args));
  });
}

app.whenReady().then(async () => {
  vueDevToolsPath && await session.defaultSession.loadExtension(vueDevToolsPath)
  createWindow()
});

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});


app.on('child-process-gone', (event, details) => {
  console.warn('app1:child-process-gone', event, details);
});
app.on("activate", () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});
