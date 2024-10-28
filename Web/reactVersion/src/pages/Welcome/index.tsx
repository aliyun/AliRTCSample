import Join from "./components/Join";
import Preview from "./components/Preview";
import styles from './index.module.less';
import { memo, useCallback, useState } from "react";
import { Row, Typography, Col, Button } from "dingtalk-design-desktop";
import { startTestNetworkQuality } from "~/utils/networkQuality";

const { Title } = Typography;

const Index = () => {
  const [testing, setTesting] = useState(false);
  const [ticktok, setTicktok] = useState(15);
  const onTestNetwork = useCallback(() => {
    setTesting(true);
    setTicktok(15);
    const interval = setInterval(() => {
      setTicktok((prev) => Math.max(0, prev - 1))
    }, 1000)
    startTestNetworkQuality().then(() => {
      setTesting(false);
      clearInterval(interval);
    })
  }, [])
  return (
    <Row className={styles.wrap}>
      <Title level={6} className={styles.demoTitle}>DingRTC Demo</Title>
      <Button disabled={testing} className={styles.testNetwork} onClick={onTestNetwork}>{!testing ? '测试网络' : `请等待${ticktok}s` }</Button>
      <Col className={styles.main}>
        <Preview />
        <Join />
      </Col>
    </Row>
  )
}

export default memo(Index);