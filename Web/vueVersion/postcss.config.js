module.exports = {
  plugins: [
    require('cssnano')({
      preset: [
        'default',
        {
          discardComments: { removeAll: true },
        },
      ],
    }),
    require('postcss-preset-env'),
  ],
};
