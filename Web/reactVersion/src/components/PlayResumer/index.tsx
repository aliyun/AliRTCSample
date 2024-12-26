import { LocalVideoTrack, RemoteVideoTrack } from "dingrtc";
import { memo, useCallback, useEffect, useMemo, useState } from "react";
import { isIOS, isWeixin } from "~/utils/tools";
import { Col, Row } from 'dingtalk-design-desktop';
import Icon from "../Icon";
import styles from './index.module.less';

interface IProps {
    track: LocalVideoTrack | RemoteVideoTrack;
    container?: HTMLDivElement;
}

const Index = (props: IProps) => {
  const [played, setPlayed] = useState(false);
  const { track, container } = props;

  const resumePlay = useCallback(() => {
    track?.play?.(container, { fit: 'cover' });
    setPlayed(true)
  }, [track])

  useEffect(() => {
    if (!track) setPlayed(false);
  }, [track])

  const needResume = useMemo(() => {
    if ((!track || !(isIOS() && isWeixin()) || played)) return false;
    return track;
  }, [track, played])
  return (
    <>
      {needResume ? <Row onClick={resumePlay} className={styles.needResume} >
        <Col>
          <Icon type='iconXDS_VideoFill' />
        </Col>
        <Col>
                    iOS微信内需要点击播放
        </Col>
      </Row > : null}
    </>
  )
}

export default memo(Index);