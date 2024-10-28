const express = require('express');
const cors = require('cors');
const { produce } = require('@dingrtc/token-generator');

const app = express();
const PORT = 3001;

app.use(cors()); // 允许跨域请求

app.get('/login', (req, res) => {
  const { appId, channelId, userId, appKey } = req.query
  res.json({
    data: {
      token: produce(appId, appKey, channelId, userId),
    }
  });
});

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
