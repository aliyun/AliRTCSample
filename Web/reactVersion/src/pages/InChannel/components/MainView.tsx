import { Row, Tooltip } from "dingtalk-design-desktop"
import { memo, useEffect, useMemo, useRef } from "react"
import { useRecoilValue } from "recoil"
import { globalFlag, mainViewPrefer } from "~/store"
import styles from '../index.module.less';
import { useLocalChannel } from "~/hooks/channel";
import Icon from "~/components/Icon";
import classNames from "classnames";

const Index = () => {
  const { allUsers } = useLocalChannel();
  const { hideToolBar } = useRecoilValue(globalFlag);

  const mainPrefer = useRecoilValue(mainViewPrefer);
  const ref = useRef(null);

  const mainViewUserInfo = useMemo(() => allUsers.find(user => user.userId === mainPrefer.userId), [mainPrefer, allUsers]);

  const mainViewTrack = useMemo(() => {
    if (!mainViewUserInfo) return;
    return mainPrefer.prefer === 'camera' ? mainViewUserInfo.videoTrack : mainViewUserInfo.auxiliaryTrack;
  }, [mainPrefer, mainViewUserInfo]);

  const micIconEnable = useMemo(() => mainViewUserInfo?.hasAudio && !mainViewUserInfo?.audioMuted, [mainViewUserInfo]);

  useEffect(() => {
    if (mainViewTrack) {
      mainViewTrack?.play(ref.current, {
        fit: 'contain',
      });
    }
  }, [mainViewTrack])

  return (
    <Row ref={ref} className={styles.mainView}>
      {mainViewUserInfo ? <div className={classNames(styles.smallViewStatus, !hideToolBar ? styles.higherMainUserInfo : '')}>
        {mainViewUserInfo?.auxiliaryTrack ? (
          <Icon style={{ color: 'limegreen' }} type="iconXDS_share_screen1" />
        ) : !micIconEnable ? <Icon type="iconXDS_UnMute2Fill" /> : <Icon type="iconXDS_Mute2" />}
        <Tooltip title={mainViewUserInfo.userName}>
          <span>{mainViewUserInfo.userName}</span>
        </Tooltip>
      </div> : null}
    </Row>    
  )
}

export default memo(Index)