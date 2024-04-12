package apptoken

import (
	"bytes"
	"testing"
)

func Test_CreateService(t *testing.T) {
	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)
	if serviceO.ChannelId != channelId || serviceO.UserId != userId || serviceO.Privilege != nil {
		t.Error("init CreateService failed")
	}

	serviceBuf, err := serviceO.Pack()
	if err != nil {
		t.Error("init pack failed")
	}

	service, err := UnpackService(bytes.NewReader(serviceBuf))
	if service.ChannelId != channelId || service.UserId != userId || service.Privilege != nil {
		t.Error("init CreateService failed")
	}
}

func Test_CreateServiceOnlyWithChannelId(t *testing.T) {
	channelId := "channelId"
	serviceO := CreateServiceOnlyWithChannelId(channelId)
	if serviceO.ChannelId != channelId || serviceO.UserId != WILDCARD_CHARACTERS || serviceO.Privilege != nil {
		t.Error("init CreateService failed")
	}

	serviceBuf, err := serviceO.Pack()
	if err != nil {
		t.Error("init pack failed")
	}

	service, err := UnpackService(bytes.NewReader(serviceBuf))
	if service.ChannelId != channelId || service.UserId != WILDCARD_CHARACTERS || service.Privilege != nil || err != nil {
		t.Error("init CreateService failed")
	}
}

func Test_CreateServiceOnlyWithUserId(t *testing.T) {
	userId := "userId"
	serviceO := CreateServiceOnlyWithUserId(userId)
	if serviceO.ChannelId != WILDCARD_CHARACTERS || serviceO.UserId != userId || serviceO.Privilege != nil {
		t.Error("init CreateServiceOnlyWithUserId failed")
	}

	serviceBuf, err := serviceO.Pack()
	if err != nil {
		t.Error("init pack failed")
	}

	service, err := UnpackService(bytes.NewReader(serviceBuf))
	if service.ChannelId != WILDCARD_CHARACTERS || service.UserId != userId || service.Privilege != nil || err != nil {
		t.Error("init CreateServiceOnlyWithUserId failed")
	}
}

func Test_AddAudioPublishPrivilege(t *testing.T) {
	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)
	if serviceO.ChannelId != channelId || serviceO.UserId != userId || serviceO.Privilege != nil {
		t.Error("init CreateService failed")
	}

	serviceO.AddAudioPublishPrivilege()

	serviceBuf, err := serviceO.Pack()
	if err != nil {
		t.Error("init pack failed")
	}

	service, err := UnpackService(bytes.NewReader(serviceBuf))
	if service.ChannelId != channelId || service.UserId != userId || *service.Privilege != PRIVILEGE_AUDIO_PUBLISH|PRIVILEGE_ENABLED {
		t.Error("init AddAudioPublishPrivilege failed")
	}
}

func Test_AddVideoPublishPrivilege(t *testing.T) {
	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)
	if serviceO.ChannelId != channelId || serviceO.UserId != userId || serviceO.Privilege != nil {
		t.Error("init CreateService failed")
	}

	serviceO.AddVideoPublishPrivilege()

	serviceBuf, err := serviceO.Pack()
	if err != nil {
		t.Error("init pack failed")
	}

	service, err := UnpackService(bytes.NewReader(serviceBuf))
	if service.ChannelId != channelId || service.UserId != userId || *service.Privilege != PRIVILEGE_VIDEO_PUBLISH|PRIVILEGE_ENABLED {
		t.Error("init AddVideoPublishPrivilege failed")
	}
}

func Test_AddAudioVideoPublishPrivilege(t *testing.T) {
	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)
	if serviceO.ChannelId != channelId || serviceO.UserId != userId || serviceO.Privilege != nil {
		t.Error("init CreateService failed")
	}

	serviceO.AddAudioPublishPrivilege()
	serviceO.AddVideoPublishPrivilege()

	serviceBuf, err := serviceO.Pack()
	if err != nil {
		t.Error("init pack failed")
	}

	service, err := UnpackService(bytes.NewReader(serviceBuf))
	if service.ChannelId != channelId || service.UserId != userId || *service.Privilege != PRIVILEGE_AUDIO_PUBLISH|PRIVILEGE_VIDEO_PUBLISH|PRIVILEGE_ENABLED {
		t.Error("init AddVideoPublishPrivilege failed")
	}
}
