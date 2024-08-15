import { CleanWebpackPlugin } from 'clean-webpack-plugin';
import CssMinimizerPlugin from 'css-minimizer-webpack-plugin';
import HtmlWebpackPlugin from 'html-webpack-plugin';
import MiniCssExtractPlugin from 'mini-css-extract-plugin';
import { dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import path from 'path';
import { VueLoaderPlugin } from 'vue-loader';

const __dirname = dirname(fileURLToPath(import.meta.url));
const dist = path.resolve(__dirname, 'dist');

console.log(path.resolve(__dirname, 'src'));

export default {
  mode: 'production',
  entry: './src/main.ts',
  output: {
    path: dist,
    filename: '[name]-[contenthash].js',
  },
  resolve: {
    extensions: ['', '.js', '.vue', '.ts', '.tsx'],
    alias: {
      '@src': path.resolve(__dirname, 'src'),
    },
  },
  plugins: [
    new VueLoaderPlugin(),
    new CleanWebpackPlugin(),
    new HtmlWebpackPlugin(),
    new MiniCssExtractPlugin(),
  ],
  module: {
    rules: [
      {
        test: /\.vue$/,
        loader: 'vue-loader',
        exclude: /node_modules/,
      },
      {
        test: /\.css$/,
        use: [
          MiniCssExtractPlugin.loader,
          {
            loader: 'css-loader',
            options: {
              modules: {
                localIdentName: '[local]--[hash:base64:6]',
              },
            },
          },
        ],
      },
      {
        test: /\.js$|\.ts$|\.tsx$/,
        loader: 'ts-loader',
        exclude: /node_modules/,
      },
      {
        test: /\.(png|jpe?g|gif|webm|mp4|svg)$/,
        loader: 'file-loader',
        options: {
          outputPath: 'assets',
        },
      },
    ],
  },
  optimization: {
    minimizer: [new CssMinimizerPlugin()],
  },
};
