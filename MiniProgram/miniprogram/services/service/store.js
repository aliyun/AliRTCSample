export class ChannelInfo {
  innerState = {};
  constructor() {
      this.groups = undefined;
      this.subscribeAudio = undefined;
  }

  $patch = (obj) => {
      this.innerState = { ...this.innerState, ...obj };
  };
}

export const store = {
  client: null,
  camTrack: null,
  channelInfo: new ChannelInfo(),
  rtm: null,
};
wx.store=store
