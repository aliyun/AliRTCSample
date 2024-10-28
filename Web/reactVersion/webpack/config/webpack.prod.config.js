const { CleanWebpackPlugin } = require('clean-webpack-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const { merge } = require('webpack-merge');
const CssMinimizerPlugin = require('css-minimizer-webpack-plugin');
const TerserJSPlugin = require('terser-webpack-plugin');
const baseWebpackConfig = require('./webpack.base.config');
const rootPath = require('../utils/rootPath');
const getPublicPath = require('../utils/getPublicPath');
// const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin

module.exports = merge(baseWebpackConfig, {
  output: {
    publicPath: getPublicPath(),
  },

  optimization: {
    minimizer: [
      new TerserJSPlugin({
        extractComments: false,
      }),
      new CssMinimizerPlugin({
        minimizerOptions: {
          preset: [
            'default',
            {
              discardComments: { removeAll: true },
            },
          ],
        },
      }),
    ],
  },

  plugins: [
    new CleanWebpackPlugin(),
    new HtmlWebpackPlugin({
      filename: 'index.html',
      template: rootPath('src/index.tpl'),
      inject: false,
      env: 'production',
    }),
    // new BundleAnalyzerPlugin({}),
  ],
});
