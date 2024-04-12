package apptoken

import (
	"strconv"
	"testing"
	"time"
)

func Test_CreateAppToken(t *testing.T) {
	appId := "appId"
	appKey := "appKey"
	timestamp := int32(1712709815) //int32(time.Now().Unix())

	appTokenO := CreateAppToken(appId, appKey, timestamp)

	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)

	appTokenO.SetService(&serviceO)

	token, err := appTokenO.Build()

	println(token)

	if err != nil {
		t.Error("init Build failed")
	}

	appToken, err := Parse(token)

	if err != nil {
		t.Error("init Parse failed")
	}

	if appToken.AppId != appTokenO.AppId {
		t.Error("sign not match")
	}

	success := appToken.Validate(appKey)
	if !success {
		t.Error("sign not match")
	}
}

func Test_CreateAppTokenWithAudioPublishPrivilege(t *testing.T) {
	appId := "appId"
	appKey := "appKey"
	timestamp := int32(1712709815) //int32(time.Now().Unix())

	appTokenO := CreateAppToken(appId, appKey, timestamp)

	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)
	serviceO.AddAudioPublishPrivilege()

	appTokenO.SetService(&serviceO)

	token, err := appTokenO.Build()

	println("token->", token)

	if err != nil {
		t.Error("init Build failed")
	}

	appToken, err := Parse(token)

	if err != nil {
		t.Error("init Parse failed")
	}

	if appToken.AppId != appTokenO.AppId {
		t.Error("sign not match")
	}

	success := appToken.Validate(appKey)
	if !success {
		t.Error("sign not match")
	}
}

func Test_CreateAppTokenWithAudioVideoPublishPrivilege(t *testing.T) {
	appId := "appId"
	appKey := "appKey"
	timestamp := int32(1712709815) //int32(time.Now().Unix())

	appTokenO := CreateAppToken(appId, appKey, timestamp)

	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)
	serviceO.AddAudioPublishPrivilege()
	serviceO.AddVideoPublishPrivilege()

	appTokenO.SetService(&serviceO)

	token, err := appTokenO.Build()

	println("token->", token)

	if err != nil {
		t.Error("init Build failed")
	}

	appToken, err := Parse(token)

	if err != nil {
		t.Error("init Parse failed")
	}

	if appToken.AppId != appTokenO.AppId {
		t.Error("sign not match")
	}

	success := appToken.Validate(appKey)
	if !success {
		t.Error("sign not match")
	}
}

func Test_CreateAppTokenWithOptions(t *testing.T) {
	appId := "appId"
	appKey := "appKey"
	timestamp := int32(1712709815) //int32(time.Now().Unix())

	appTokenO := CreateAppToken(appId, appKey, timestamp)

	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)

	appTokenO.SetService(&serviceO)

	appTokenOptionsO := CreateAppTokenOptions()
	appTokenOptionsO.SetEngineOptions(map[string]string{
		"c": "C",
		"b": "B",
		"a": "A",
	})

	appTokenO.SetOptions(&appTokenOptionsO)

	token, err := appTokenO.Build()

	println(token)

	if err != nil {
		t.Error("init Build failed")
	}

	appToken, err := Parse(token)

	if err != nil {
		t.Error("init Parse failed")
	}

	if appToken.AppId != appTokenO.AppId {
		t.Error("sign not match")
	}

	success := appToken.Validate(appKey)
	if !success {
		t.Error("sign not match")
	}
}

func Test_CreateAppTokenWithBigOptions(t *testing.T) {
	appId := "appId"
	appKey := "appKey"
	timestamp := int32(1712709815) //int32(time.Now().Unix())

	appTokenO := CreateAppToken(appId, appKey, timestamp)

	channelId := "channelId"
	userId := "userId"
	serviceO := CreateService(channelId, userId)

	appTokenO.SetService(&serviceO)

	appTokenOptionsO := CreateAppTokenOptions()

	engineOptions := map[string]string{}

	ts := time.Now().Unix()
	for i := 0; i < 1000; i++ {
		engineOptions[strconv.FormatInt(ts, 10)] = strconv.FormatInt(ts, 10)
		ts = ts + 1
	}

	appTokenOptionsO.SetEngineOptions(engineOptions)

	appTokenO.SetOptions(&appTokenOptionsO)

	token, err := appTokenO.Build()

	println(token)

	if err != nil {
		t.Error("init Build failed")
	}

	appToken, err := Parse(token)

	if err != nil {
		t.Error("init Parse failed")
	}

	if appToken.AppId != appTokenO.AppId {
		t.Error("sign not match")
	}

	success := appToken.Validate(appKey)
	if !success {
		t.Error("sign not match")
	}
}

func Test_NextMultiple(t *testing.T) {
	var n = 0
	var expected = 0

	var actual = nextMultiple(n, 256)
	if actual != expected {
		t.Error("")
	}

	n = 100
	expected = 256
	actual = nextMultiple(n, 256)
	if actual != expected {
		t.Error("")
	}

	n = 255
	expected = 256
	actual = nextMultiple(n, 256)
	if actual != expected {
		t.Error("")
	}

	n = 256
	expected = 256
	actual = nextMultiple(n, 256)
	if actual != expected {
		t.Error("")
	}

	n = 257
	expected = 256 * 2
	actual = nextMultiple(n, 256)
	if actual != expected {
		t.Error("")
	}

	n = 511
	expected = 256 * 2
	actual = nextMultiple(n, 256)
	if actual != expected {
		t.Error("")
	}

	n = 512
	expected = 256 * 2
	actual = nextMultiple(n, 256)
	if actual != expected {
		t.Error("")
	}

	n = 513
	expected = 256 * 4
	actual = nextMultiple(n, 256)
	if actual != expected {
		t.Error("")
	}
}

func Test_Validate(t *testing.T) {
	appId := "appId"
	appKey := "appKey"

	token := "000eJxjYGBQ+MjVKznrflVZoflZ3oP3rA7eb92ePZuX+3NmGEfS8bZ13QwMDKyJBQWeKWni4V0+3O+q08TrZoMES1KLSwxhDCMGRoZRMNQAAPBiGrc="
	appToken, err := Parse(token)

	if err != nil {
		t.Error("init Parse failed")
	}

	if appToken.AppId != appId {
		t.Error("sign not match")
	}

	success := appToken.Validate(appKey)
	if !success {
		t.Error("sign not match")
	}
}
