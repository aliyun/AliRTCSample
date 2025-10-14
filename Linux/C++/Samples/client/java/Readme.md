# Linux java demo 运行说明

## 命令控制运行（推荐）
app_config.json 与C++ Demo一致，只是不会主动入会和推流，按输入命令执行
1. 执行脚本
```
./run_demo.sh
```
2. 进入命令行后，有提示命令说明，按命令对应数字输入即可
1 : join
2 : leave
4 : pub audio
5 : unpub audio
5 : pub video
6 : unpub video
0 : exit

3. 命令运行模式控制台不会输出日志，命令行相关日志将保存在本地：command_log.log，
有问题时需要同时提供该日志，注意：该日志每次运行会覆盖。

## 自动运行（与C++ Demo行为一致）
1. 执行脚本
```
./run_demo.sh 1
```
2. 将自动运行直至推流文件读取结束，与C++ Demo行为一致