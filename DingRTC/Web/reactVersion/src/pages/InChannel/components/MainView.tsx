import { Row } from "dingtalk-design-desktop"
import { memo, useEffect, useRef } from "react"
import { useRecoilValue } from "recoil"
import { mainView } from "~/store"
import styles from '../index.module.less';

const Index = () => {
  const mainViewTrack = useRecoilValue(mainView);
  const ref = useRef(null);

  useEffect(() => {
    if (mainViewTrack && !mainViewTrack.isPlaying) {
      mainViewTrack?.play(ref.current);
    }
  }, [mainViewTrack])

  return (
    <Row ref={ref} className={styles.mainView} />
  )
}

export default memo(Index)