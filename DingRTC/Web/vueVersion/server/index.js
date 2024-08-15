import cors from 'cors';
import express from 'express';

import pkg from '@dingrtc/token-generator';

const { produce } = pkg;

const app = express();
const PORT = 3001;
app.use(cors()); // 允许跨域请求
app.get('/login', (req, res) => {
  const { appId, channelId, userId, appKey } = req.query;
  res.json({
    data: {
      token: produce(appId, appKey, channelId, userId),
    },
  });
});
app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
