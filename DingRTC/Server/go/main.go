package main

import (
	apptoken "apptoken/apptoken"
	"time"
)

func main() {
	appId := "appId"
	appKey := "appKey"
	expiredTs := int32(time.Now().Unix()) + 12*60*60 // expired after 12h
	appTokenX := apptoken.CreateAppToken(appId, appKey, expiredTs)

	//
	channelId := "channelId"
	userId := "userId"
	service := apptoken.CreateService(channelId, userId)
	appTokenX.SetService(&service)

	appTokenStr, _ := appTokenX.Build()

	println(appTokenStr)
}
