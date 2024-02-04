<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <meta name="format-detection" content="telephone=no">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <link rel="icon" id="favicon" href="https://gw.alicdn.com/imgextra/i3/O1CN01eMicSg1GVD4uXMWGv_!!6000000000627-73-tps-32-32.ico" type="image/x-icon" />
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, minimum-scale=1, user-scalable=no,viewport-fit=cover">
    <title>OneRTC Demo</title>
    <link href="https://g.alicdn.com/dingding/dingtalk-theme/dingtalk-x/index.css" rel="stylesheet">
    <% for (var chunk in htmlWebpackPlugin.files.css) { %>
      <link href="<%= htmlWebpackPlugin.files.css[chunk] %>" rel="stylesheet">
    <% } %>
    <script src="https://g.alicdn.com/dingding/dingtalk-theme/entry.js"></script>
  <body>
    <div id="root"></div>
    <% for (var chunk in htmlWebpackPlugin.files.js) { %>
      <script type="text/javascript" src="<%= htmlWebpackPlugin.files.js[chunk] %>" crossorigin></script>
    <% } %>
  </body>
</html>