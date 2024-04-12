package main

import (
	apptoken "apptoken/apptoken"
	"time"
)

func main() {
	appId := "appId"
	appKey := "appKey"
	timestamp := int32(time.Now().Unix())
	appTokenX := apptoken.CreateAppToken(appId, appKey, timestamp)

	//
	channelId := "channelId"
	userId := "userId"
	service := apptoken.CreateService(channelId, userId)
	appTokenX.SetService(&service)

	appTokenStr, _ := appTokenX.Build()

	println(appTokenStr)
}
