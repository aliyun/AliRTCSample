import {DingRtcVideoView} from '@dingrtc/dingrtc';

@Builder
export function BuildDingRtcVideoView(params: ESObject) {
  DingRtcVideoView({
	canvasId: params.id,
  })
  // .attributeModifier(params.attributeUpdater)
}
