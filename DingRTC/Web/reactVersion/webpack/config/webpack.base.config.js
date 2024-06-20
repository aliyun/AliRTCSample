const MiniCssExtractPlugin = require('mini-css-extract-plugin'); // 从JS中分离css，成单独css文件
const rootPath = require('../utils/rootPath');

// const NODE_ENV = process.env.NODE_ENV || 'production';
// const isProd = NODE_ENV === 'production';
// const cssModulesLocalIdentName = isProd ? '[hash:base64:8]' : '[path]--[local]--[hash:base64:5]';

module.exports = {
  entry: rootPath('src/main.tsx'),
  output: {
    filename: '[name]/index.js',
    publicPath: '/',
    path: rootPath('dist'),
  },

  resolve: {
    alias: {
      '@': rootPath('src/'),
      '~': rootPath('src/'),
      'public': rootPath('public/'),
      'react/jsx-dev-runtime.js': 'react/jsx-dev-runtime',
      'react/jsx-runtime.js': 'react/jsx-runtime',
    },
    mainFields: ['main', 'browser', 'module'],
    extensions: ['.ts', '.tsx', '.js', '.json', '.less', '.css'],
  },

  module: {
    rules: [
      {
        test: /\.ts(x?)$/,
        exclude: /node_modules/,
        include: [rootPath('src'), rootPath('demo')],
        use: ['babel-loader'],
      },
      {
        test: /\.css$/i,
        use: [MiniCssExtractPlugin.loader, 'css-loader'],
      },
      {
        test: /\.less$/i,
        use: [
          MiniCssExtractPlugin.loader,
          {
            loader: 'css-loader',
            options: {
              modules: true,
              importLoaders: 1,
            },
          },
          'postcss-loader',
          {
            loader: 'less-loader',
            options: {
              lessOptions: {
                javascriptEnabled: true,
              },
            },
          },
        ],
      },
      {
        test: /\.(png|jpe?g|gif|svg)(\?.*)?$/,
        loader: 'url-loader',
        options: {
          limit: 10000,
          name: 'img/[name].[hash:7].[ext]',
        },
      },
      {
        test: /\.(mp4|webm|ogg|mp3|wav|flac|aac)(\?.*)?$/,
        loader: 'url-loader',
        options: {
          limit: 10000,
          name: 'media/[name].[hash:7].[ext]',
        },
      },
      {
        test: /\.(woff2?|eot|ttf|otf)(\?.*)?$/,
        loader: 'url-loader',
        options: {
          limit: 10000,
          name: 'fonts/[name].[hash:7].[ext]',
        },
      },
    ],
  },
  plugins: [
    new MiniCssExtractPlugin({
      filename: '[name]/[name].css',
    }),
  ],
};
