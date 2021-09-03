let logitems = [];
let systemInfoChecked = false;

const formatTime = date => {
  const year = date.getFullYear()
  const month = date.getMonth() + 1
  const day = date.getDate()
  const hour = date.getHours()
  const minute = date.getMinutes()
  const second = date.getSeconds()

  return [year, month, day].map(formatNumber).join('/') + ' ' + [hour, minute, second].map(formatNumber).join(':')
}

const formatNumber = n => {
  n = n.toString()
  return n[1] ? n : '0' + n
}

const log = (msg, level) => {
  let time = formatTime(new Date());
  logitems.push(`${time}: ${msg}`);
  if (level === "error") {
    console.error(`${time}: ${msg}`);
  } else {
    console.log(`${time}: ${msg}`);
  }
}

const checkSystemInfo = (app) => {
  if (!systemInfoChecked) {
    systemInfoChecked = true;
    wx.getSystemInfo({
      success: function (res) {
        log(`${JSON.stringify(res)}`);
        let sdkVersion = res.SDKVersion;
        let version_items = sdkVersion.split(".");
        let major_version = parseInt(version_items[0]);
        let minor_version = parseInt(version_items[1]);

        app.globalData.systemInfo = res;

        if (major_version <= 1 && minor_version < 7) {
          wx.showModal({
            title: '版本过低',
            content: '微信版本过低，部分功能可能无法工作',
            success: function (res) {
              if (res.confirm) {
                console.log('用户点击确定')
              } else if (res.cancel) {
                console.log('用户点击取消')
              }
            }
          })
        }
      }
    })
  }
}

module.exports = {
  formatTime: formatTime,
  checkSystemInfo: checkSystemInfo,
  log: log
}
