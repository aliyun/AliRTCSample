import * as ReactDOM from 'react-dom/client';
import App from './pages/App';
import { RecoilRoot } from 'recoil';
import theme, { IThemeType } from 'dingtalk-theme';

theme.setTheme(IThemeType.light);

const root = ReactDOM.createRoot(document.getElementById('root'));

root.render(
  <RecoilRoot>
    <App />
  </RecoilRoot>
);
