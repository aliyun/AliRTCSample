package com.company.apptoken.enums;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@RequiredArgsConstructor
@Getter
public enum PrivilegeEnum {
    ENABLE_PRIVILEGE(0x1),
    ENABLE_AUDIO_PUBLISH(0x2),
    ENABLE_VIDEO_PUBLISH(0x4),
    ;
    private final int privilege;
}
