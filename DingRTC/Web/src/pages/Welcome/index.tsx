import { memo } from "react";
import Join from "./components/Join";
import Preview from "./components/Preview";
import styles from './index.module.less';
import { Row, Typography, Col } from "dingtalk-design-desktop";

const { Title } = Typography;

const Index = () => {
  return (
    <Row className={styles.wrap}>
      <Title level={6} className={styles.demoTitle}>DingRTC Demo</Title>
      <Col className={styles.main}>
        <Preview />
        <Join />
      </Col>
    </Row>
  )
}

export default memo(Index);