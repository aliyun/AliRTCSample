import { Drawer, Row } from "dingtalk-design-desktop"
import { memo, useCallback } from "react"
import { useRecoilState } from "recoil"
import { globalFlag } from "~/store"
import styles from '../index.module.less';

const Index = () => {

  const [{ showLog }, setGlobalFlag] = useRecoilState(globalFlag)

  const onClose = useCallback(() => {
    setGlobalFlag((prev) => ({ ...prev, showLog: false }))
  }, [])

  return (
    <Drawer title="操作日志" placement="bottom" mask={false} onClose={onClose} height={showLog ? 400 : 0} open>
      <Row className={styles.logWrap}>
        <div id="logOutput" />
      </Row>
    </Drawer>
  )
}

export default memo(Index);