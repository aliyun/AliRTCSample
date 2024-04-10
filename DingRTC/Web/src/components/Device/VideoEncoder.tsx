import { VideoDimension } from "dingrtc"
import { Form, InputNumber, Modal, Select } from "dingtalk-design-desktop"
import { useForm } from "dingtalk-design-desktop/lib/form/Form";
import { memo, useCallback } from "react";

interface IProps {
  frameRate?: number;
  open: boolean;
  dimension?: VideoDimension;
  onClose: () => void;
  onOk: (frameRate: number, dimension: VideoDimension) => void;
}
const videoDimensions = [
  'VD_120x120',
  'VD_160x120',
  'VD_180x180',
  'VD_240x180',
  'VD_320x180',
  'VD_240x240',
  'VD_320x240',
  'VD_424x240',
  'VD_360x360',
  'VD_480x360',
  'VD_640x360',
  'VD_480x480',
  'VD_640x480',
  'VD_840x480',
  'VD_960x540',
  'VD_960x720',
  'VD_1280x720',
  'VD_1920x1080',
];

const Index = (props: IProps) => {
  const { onOk, frameRate: oldFrameRate, dimension: oldDimension, open, onClose } = props;
  const [form] = useForm();

  const onConfirm = useCallback(async () => {
    const { frameRate, dimension } = form.getFieldsValue();
    onOk(frameRate, dimension);
    onClose();
  }, [form])

  return (
    <Modal
      open={open}
      okText="确定"
      cancelText="取消"
      onCancel={onClose}
      onOk={onConfirm}
    >
      <Form
        layout="vertical"
        form={form}
        initialValues={{ frameRate: oldFrameRate, dimension: oldDimension }}
      >
        <Form.Item required name="frameRate" label="帧率">
          <InputNumber />
        </Form.Item>
        <Form.Item required name="dimension" label="分辨率">
          <Select options={videoDimensions.map(item => ({ label: item, value: item }))} />
        </Form.Item>
      </Form>
    </Modal>
  )
}

export default memo(Index)