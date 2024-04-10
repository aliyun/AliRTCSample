module.exports = {
  "presets": [
    "@babel/preset-env",
    "@babel/preset-typescript",
    ["@babel/preset-react", {
      "runtime": "automatic"
    }]
  ],
  "plugins": [
    ["@babel/transform-runtime"],
    ["import", { "libraryName": "dingtalk-design-desktop", "libraryDirectory": "es"}, 'dingtalk-design-desktop'],
    ["import", { "libraryName": "dd-icons", "libraryDirectory": "es"}, 'dd-icons']
  ]
}