module.exports = {
  extends: [
    'eslint-config-ali/typescript/react',
    'prettier',
    'prettier/@typescript-eslint',
    'prettier/react',
  ],
  rules: {
    'react/jsx-uses-react': 'off',
    'react/react-in-jsx-scope': 'off',
    'react/jsx-no-bind': 0,
    'react/no-danger': 0,
    'react-hooks/exhaustive-deps': 0,
    indent: [2, 2, { SwitchCase: 1 }],
  },
};
