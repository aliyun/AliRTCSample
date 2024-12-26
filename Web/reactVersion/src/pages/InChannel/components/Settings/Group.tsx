import { Button, Row, Space, Typography, Tag, Modal, Input, Tooltip, List, Checkbox } from "dingtalk-design-desktop";
import { BaseSyntheticEvent, memo, useCallback, useEffect, useMemo, useState } from "react"
import { useLocalChannel, useRemoteChannel } from "~/hooks/channel";
import styles from './index.module.less'
import { RemoteAudioTrack, Group } from "dingrtc";
import Icon from "~/components/Icon";
import { CheckboxChangeEvent } from "dingtalk-design-desktop/lib/checkbox";

const { Text } = Typography;

interface GroupItemProps {
    data: Group;
}

const GroupItem = (props: GroupItemProps) => {
  const { micTrack } = useLocalChannel();
  // @ts-ignore
  const { id, joined, users, audioPublished } = props.data;
  const [showActions, setShowActions] = useState(false);
  const { setRemoteChannelInfo, subscribeAudio } = useRemoteChannel();
  const { client } = useLocalChannel();

  const unsubGroup = useCallback(async () => {
    if (subscribeAudio === id) {
      await client.unsubscribeGroup(id);
      setRemoteChannelInfo((prev) => ({ ...prev, subscribeAudio: '', mcuAudioTrack: null }))
    }
  }, [subscribeAudio, id])

  const dismiss = useCallback(async () => {
    await unsubGroup()
    await client.dismissGroup(id)
  }, [id, unsubGroup])

  const leave = useCallback(async () => {
    await unsubGroup();
    await client.leaveGroup(id)
  }, [id, unsubGroup])
  const subGroup = useCallback(async () => {
    if (subscribeAudio && subscribeAudio !== id) {
      await client.unsubscribeGroup(subscribeAudio);
    }
    const audioTrack = await client.subscribeGroup(id);
    audioTrack.play();
    setRemoteChannelInfo((prev) => ({ ...prev, subscribeAudio: id, mcuAudioTrack: audioTrack }))
  }, [subscribeAudio])

  const Actions = useMemo(() => {
    const buttons = [
      {
        text: '离开',
        show: joined,
        onClick: () => {
          leave();
        },
      },
      {
        text: '混音',
        show: joined && !audioPublished,
        onClick: () => {
          client.mixAudioToGroup(micTrack, true, id).then(() => {
            setRemoteChannelInfo((prev) => ({ ...prev, groups: [...client.groups] }));
          })
        },
      },
      {
        text: '取消混音',
        show: joined && audioPublished,
        onClick: () => {
          client.mixAudioToGroup(micTrack, false, id).then(() => {
            setRemoteChannelInfo((prev) => ({ ...prev, groups: [...client.groups] }));  
          })
        },
      },
      {
        text: '订阅',
        show: joined && subscribeAudio !== id,
        onClick: () => {
          subGroup()
        },
      },
      {
        text: '取消订阅',
        show: joined && subscribeAudio === id,
        onClick: () => {
          unsubGroup()
        },
      },
      {
        text: '解散',
        show: true,
        onClick: () => {
          dismiss()
        },
      },

    ].filter((item) => item.show);
    if (!buttons.length) return null;
    return (
      <Tooltip
        trigger="click"
        overlayInnerStyle={{
          backgroundColor: 'rgba(245, 247, 250, 0.9)',
        }}
        open={showActions}
        onOpenChange={(show) => setShowActions(show)}
        title={
          <List>
            {buttons.map((item) => (
              <List.Item
                style={{ cursor: 'pointer' }}
                key={item.text}
                onClick={() => {
                  item.onClick();
                  setShowActions(false);
                }}
              >
                {item.text}
              </List.Item>
            ))}
          </List>
        }
      >
        <div className={styles.items}>
          <Icon type="iconXDS_List" />
        </div>
      </Tooltip>
    );
  }, [
    showActions,
    joined,
    subGroup,
    id,
    micTrack,
    leave,
    dismiss,
    unsubGroup,
    audioPublished,
  ]);
  const tags = useMemo(() => {
    const blueTexts: string[] = []
    if (subscribeAudio === id) {
      blueTexts.push('订阅')
    }
    if (audioPublished) {
      blueTexts.push('混音')
    }
    const list = [
      {
        color: 'green',
        show: joined,
        text: '已加入'
      },
      {
        color: 'blue',
        show: joined && (subscribeAudio === id || audioPublished),
        text: `已${blueTexts.join('、')}`
      }
    ]
    return list.filter(item => item.show);
  }, [joined, subscribeAudio, id, audioPublished]);
  return (
    <Row className={styles.group}>
      <Row>
        <Text>{id}</Text><Text type="secondary">({users.length}人)</Text>
        <Tooltip title={users.map((item: any) => item.userId).join()}>
          <Text className={styles.uidList}>uid列表:
            {users.map((item: any) => item.userId).join()}
          </Text>
        </Tooltip>
      </Row>
      <Row className={styles.actions}>
        {tags.map(item => (<Tag key={item.text} color={item.color} size="small">{item.text}</Tag>))}
        {Actions}
      </Row>
    </Row>
  )
}

interface GroupSettingProps {
    show: boolean;
    onOk: (...args: any) => void;
    onCancel: () => void;
}

const GroupSettingModal = (props: GroupSettingProps) => {
  const { show, onOk, onCancel } = props;
  const [groupId, setGroupId] = useState('');
  const [mixToGroup, setMixToGroup] = useState(true);
  const joinOk = useCallback(() => {
    onOk(groupId, mixToGroup);
    onCancel();
  }, [onOk, groupId, mixToGroup]);
  const onIdChange = useCallback((e: BaseSyntheticEvent) => {
    setGroupId(e.target.value)
  }, [])
  const onMixToGroupChange = useCallback((e: CheckboxChangeEvent) => {
    setMixToGroup(e.target.checked)
  }, [])

  return (
    <>
      {show ? <Modal
        open
        width={400}
        onOk={joinOk}
        title={'加入分组'}
        onCancel={onCancel}
      >
        <Input style={{ marginBottom: 10 }} placeholder="请填写分组Id" value={groupId} onChange={onIdChange} />
        <Checkbox checked={mixToGroup} onChange={onMixToGroupChange}>混音到分组</Checkbox>
      </Modal> : null}
    </>
  )
}

const GroupList = () => {
  const { groups, setRemoteChannelInfo, subscribeAudio, mcuAudioTrack } = useRemoteChannel();
  const { client, micTrack } = useLocalChannel();
  const [showModal, setShowModal] = useState(false);
  const onRefresh = useCallback(() => {
    setRemoteChannelInfo((prev) => ({ ...prev, groups: [...client.groups] }));
  }, []);
  const onJoin = useCallback(async (groupId: string, publishAudioToGroup = true) => {
    if (subscribeAudio === 'mcu' && mcuAudioTrack) {
      await client.unsubscribe('mcu', 'audio');
      setRemoteChannelInfo((prev) => ({ ...prev, groups: [...client.groups], subscribeAudio: '', mcuAudioTrack: null }));
    }
    await client.joinGroup({
      groupId,
      publishAudioToGroup,
    })
  }, [subscribeAudio, mcuAudioTrack])

  const subChannel = useCallback(async () => {
    if (subscribeAudio && subscribeAudio === 'mcu') {
      return
    } else if (subscribeAudio) {
      await client.unsubscribeGroup(subscribeAudio);
    }
    const audioTrack = await client.subscribe('mcu', 'audio') as RemoteAudioTrack;
    audioTrack.play();
    setRemoteChannelInfo((prev) => ({ ...prev, subscribeAudio: 'mcu', mcuAudioTrack: audioTrack }))
  }, [subscribeAudio])
  useEffect(() => {
    setRemoteChannelInfo((prev) => ({ ...prev, groups: [...client.groups] }));
    return () => {
      setShowModal(false);
    }
  }, [])
  return (
    <Row className={styles.groupContainer}>
      <Row className={styles.groupContainerHeader}>
        <Button type="primary" onClick={() => setShowModal(true)}>加入分组</Button>
        <Button disabled={groups.some(item => item.joined)} onClick={() => client.mixAudioToGroup(micTrack, true)}>混音到大厅</Button>
        <Button disabled={groups.some(item => item.joined)} onClick={() => { subChannel() }}>订阅大厅</Button>
        <Button type="link" onClick={() => onRefresh}>刷新</Button>
      </Row>
      <Space direction="vertical">
        {groups.map((item) => (<GroupItem  key={item.id} data={item} />))}
      </Space>
      <GroupSettingModal show={showModal} onOk={onJoin} onCancel={() => { setShowModal(false) }} />
    </Row>
  )
}

export default memo(GroupList);