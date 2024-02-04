import { UserBlock } from './components/UserBlock';
import { memo, useState } from 'react';
import './App.css'

const App = () => {

  const [channelName] = useState(`${Math.ceil(Math.random() * 10000)}`)
  return (
    <UserBlock label="1" channelName={channelName} />
  );
};

export default memo(App);
