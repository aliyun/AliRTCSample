export const print = (...args: any) => {
  const outputArea = document.getElementById('logOutput');
  if (outputArea) {
    const now = new Date();
    const timeStr = `${now.toLocaleString()}:${now.getMilliseconds()}`;
    outputArea.innerText += `[${timeStr} ${args.join(':')}]\r\n`;
  }
  console.log(...args);
};

export const parseTime = (baseTime: number) => {
  const seconds = Math.floor(baseTime % 60);
  const minutes = Math.floor(baseTime % 3600 / 60);
  const hours =  Math.floor(baseTime / 3600);
  return `${hours}:${minutes >= 10 ? minutes : `0${minutes}`}:${seconds >= 10 ? seconds : `0${seconds}`}`;
};

export const loopReadFile = (config: {
  filePath: string;
  interval: number;
  highWaterMark: number;
}, calkback: any) => {
  const { createReadStream } =  require('node:fs');
  const { filePath, highWaterMark, interval } = config;
  let readStream
  let timer: number;
  let firstTime = true;
  let readyData: any;
  const fn = () => {
    readStream = createReadStream(filePath, { highWaterMark });
    readStream.on('end', () => {
      clearInterval(timer);
      firstTime = true;
      readyData = null;
      fn()
    });
    readStream.on('data', (data) => {
      if (firstTime) {
        firstTime = false;
        timer =  window.setInterval(() => {
          fireData();
        }, interval);
        calkback(data);
        return;
      }
      readyData = data;
      readStream.pause();
    });
    const fireData = () => {
      if (readyData) {
        calkback(readyData);
      }
      readyData = null;
      readStream.resume();
    }
  }
  fn();
  return () => {
    clearInterval(timer);
    readStream?.close();
    readStream?.push(null);
    readStream?.read(0);
  }
}

export const transMbToGb = (mb: number) => {
  if (!mb) return 0 + 'MB';
  const gbs = (mb % 1024) === 0 ? mb / 1024 : (mb / 1024).toFixed(1);
  return mb > 1024 ? gbs + ' GB' : mb + ' MB';
}