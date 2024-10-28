package apptoken

import (
	"bytes"
	"encoding/binary"
	"io"
)

const (
	PRIVILEGE_ENABLED       int32 = 1
	PRIVILEGE_AUDIO_PUBLISH int32 = 2
	PRIVILEGE_VIDEO_PUBLISH int32 = 4
)

const WILDCARD_CHARACTERS = "*"

type Service struct {
	ChannelId string
	UserId    string
	Privilege *int32
}

func CreateService(channelId string, userId string) Service {
	return Service{
		ChannelId: channelId,
		UserId:    userId,
	}
}

func CreateServiceOnlyWithUserId(userId string) Service {
	return Service{
		ChannelId: WILDCARD_CHARACTERS,
		UserId:    userId,
	}
}

func CreateServiceOnlyWithChannelId(channelId string) Service {
	return Service{
		ChannelId: channelId,
		UserId:    WILDCARD_CHARACTERS,
	}
}

func (service *Service) Validate() {
	if service.ChannelId == "" || service.UserId == "" {
		panic("illegal ChannelId or UserId")
	}
}

func (service *Service) AddAudioPublishPrivilege() {
	if service.Privilege == nil {
		service.Privilege = new(int32)
		*service.Privilege = PRIVILEGE_ENABLED
	}
	*service.Privilege = *service.Privilege | PRIVILEGE_AUDIO_PUBLISH
}

func (service *Service) AddVideoPublishPrivilege() {
	if service.Privilege == nil {
		service.Privilege = new(int32)
		*service.Privilege = PRIVILEGE_ENABLED
	}
	*service.Privilege = *service.Privilege | PRIVILEGE_VIDEO_PUBLISH
}

func (service *Service) Pack() ([]byte, error) {
	buf := new(bytes.Buffer)

	// channelId
	channelId := []byte(service.ChannelId)
	if err := binary.Write(buf, binary.BigEndian, int32(len(channelId))); err != nil {
		return nil, err
	}
	if _, err := buf.Write(channelId); err != nil {
		return nil, err
	}

	// userId
	userId := []byte(service.UserId)
	if err := binary.Write(buf, binary.BigEndian, int32(len(userId))); err != nil {
		return nil, err
	}
	if _, err := buf.Write(userId); err != nil {
		return nil, err
	}

	// hasPrivilege
	hasPrivilege := service.Privilege != nil
	if err := binary.Write(buf, binary.BigEndian, hasPrivilege); err != nil {
		return nil, err
	}
	// privilege
	if hasPrivilege {
		if err := binary.Write(buf, binary.BigEndian, *service.Privilege); err != nil {
			return nil, err
		}
	}
	return buf.Bytes(), nil
}

func UnpackService(buf io.Reader) (*Service, error) {

	service := Service{}

	// channelId
	var channelIdLength int32
	if err := binary.Read(buf, binary.BigEndian, &channelIdLength); err != nil {
		return nil, err
	}
	channelId := make([]byte, channelIdLength)
	if _, err := io.ReadFull(buf, channelId); err != nil {
		return nil, err
	}
	service.ChannelId = string(channelId)

	// userId
	var userIdLength int32
	if err := binary.Read(buf, binary.BigEndian, &userIdLength); err != nil {
		return nil, err
	}
	userId := make([]byte, userIdLength)
	if _, err := io.ReadFull(buf, userId); err != nil {
		return nil, err
	}
	service.UserId = string(userId)

	// privilege
	var hasPrivilege bool
	if err := binary.Read(buf, binary.BigEndian, &hasPrivilege); err != nil {
		return nil, err
	}
	if hasPrivilege {
		var privilege int32
		if err := binary.Read(buf, binary.BigEndian, &privilege); err != nil {
			return nil, err
		}
		service.Privilege = &privilege
	}

	return &service, nil
}
