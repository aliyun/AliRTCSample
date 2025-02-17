
export interface LoginInfo {
  appId: string;
  token: string;
  cname: string;
  uid: string;
  uname?: string;
}

export class User {
  uid: string;
  nickName?: string;
  isAudioOn: boolean = true
  isVideoOn: boolean = true
  isCameraOn: boolean = true
  isPreviewOn: boolean = true
  isAudioMuted: boolean = false
  isVideoMuted: boolean = false
  isScreenOn: boolean = false
  isSpeakerphone: boolean = true;
  isFrontCamera: boolean = true;
  width: number = 640;
  height: number = 480;
  local: boolean = false;  // Is yourself?

  constructor(uid: string) {
    this.uid = uid;
  }
}


