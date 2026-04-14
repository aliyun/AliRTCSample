# 配置文件说明

## 配置步骤

1. 复制 `config.json.example` 文件为 `config.json`：
   ```bash
   cp config.json.example config.json
   ```

2. 编辑 `config.json` 文件，填入实际的配置值：
   - `appServerConfig.online`: 在线环境的 AppServer 地址
   - `aiAgentConfig.defaultTemplateId`: AI Agent 默认模板ID
   - `aiAgentConfig.rtcServiceName`: Rtc 服务名称
   - `aiAgentConfig.asrVendor`: ASR 服务商
   - `aiAgentConfig.asrModel`: ASR 模型
   - `aiAgentConfig.llmVendor`: LLM 服务商
   - `aiAgentConfig.llmModel`: LLM 模型
   - `aiAgentConfig.llmTemperature`: LLM 温度参数
   - `aiAgentConfig.llmTopP`: LLM TopP 参数
   - `aiAgentConfig.llmMaxToken`: LLM 最大 Token 数
   - `aiAgentConfig.llmHistoryDepth`: LLM 历史深度
   - `aiAgentConfig.llmPrompt`: LLM 提示词
   - `aiAgentConfig.ttsVendor`: TTS 服务商
   - `aiAgentConfig.ttsModel`: TTS 模型
   - `aiAgentConfig.ttsVoice`: TTS 语音
   - `aiAgentConfig.apiStart`: Agent Start API 路径
   - `aiAgentConfig.apiUpdate`: Agent Update API 路径
   - `aiAgentConfig.apiStop`: Agent Stop API 路径
   - `subtitleConfig.apiStart`: Subtitle Start API 路径
   - `subtitleConfig.apiStop`: Subtitle Stop API 路径
   - `siAgentConfig.apiStart`: SiAgent Start API 路径
   - `siAgentConfig.apiStop`: SiAgent Stop API 路径
   - `siAgentConfig.orgId`: SiAgent 组织ID
   - `siAgentConfig.productCode`: SiAgent 产品代码
   - `siAgentConfig.interpretApiKey`: SiAgent 翻译 API Key
   - `siAgentConfig.interpretAsrSubModel`: SiAgent ASR 子模型
   - `siAgentConfig.interpretMaxSentenceSilence`: SiAgent 最大句子静默时间
   - `siAgentConfig.interpretModel`: SiAgent 翻译模型
   - `siAgentConfig.interpretSourceLanguage`: SiAgent 源语言
   - `siAgentConfig.interpretTranslateApiKey`: SiAgent 翻译 API Key
   - `siAgentConfig.interpretTranslateLlmSceneEnabled`: SiAgent 翻译 LLM 场景启用
   - `siAgentConfig.interpretUrl`: SiAgent 翻译 URL
   - `siAgentConfig.interpretVendor`: SiAgent 翻译服务商
   - `siAgentConfig.ttsApiKey`: SiAgent TTS API Key
   - `siAgentConfig.ttsModel`: SiAgent TTS 模型
   - `siAgentConfig.ttsRate`: SiAgent TTS 语速
   - `siAgentConfig.ttsVendor`: SiAgent TTS 服务商
   - `siAgentConfig.ttsVolume`: SiAgent TTS 音量

3. 将 `config.json` 文件放置在 `src/main/assets/` 目录下

## 配置文件格式

```json
{
  "appServerConfig": {
    "online": "oneline_server"
  },
  "aiAgentConfig": {
    "defaultTemplateId": "your_template_id_here",
    "defaultTemplateIdOnline": "your_template_id_online_here",
    "rtcServiceName": "your_rtc_service_name_here",
    "asrVendor": "your_asr_vendor_here",
    "asrModel": "your_asr_model_here",
    "llmVendor": "your_llm_vendor_here",
    "llmModel": "your_llm_model_here",
    "llmTemperature": 0.7,
    "llmTopP": 0.8,
    "llmMaxToken": 500,
    "llmHistoryDepth": 10,
    "llmPrompt": "your_llm_prompt_here",
    "ttsVendor": "your_tts_vendor_here",
    "ttsModel": "your_tts_model_here",
    "ttsVoice": "your_tts_voice_here",
    "apiStart": "",
    "apiUpdate": "",
    "apiStop": ""
  },
  "subtitleConfig": {
    "apiStart": "",
    "apiStop": ""
  }
}
```

## 注意事项

- 如果配置文件不存在或格式错误，程序会使用默认值
