package main

import (
	apptoken "apptoken/apptoken"
	"time"
)

func main() {
	// find appId in your RTC console (https://rtc.console.aliyun.com/#/manage/list)
	appId := "replace_your_appId"
	// find appKey in your RTC console
	appKey := "replace_your_appKey"
	// Token is valid for a maximum of 24 hours. This example uses 12 hours, adjust according to your needs.
	expiredTs := int32(time.Now().Unix()) + 12*60*60
	channelId := "replace_your_channelId"
	userId := "replace_your_userId"

	appTokenX := apptoken.CreateAppToken(appId, appKey, expiredTs)

	// By default, all privileges are allowed. You can control audio/video/screen privileges individually as shown in the example below.
	// Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail privilege informations.
	// Example0: full privileges as default
	service := apptoken.CreateService(channelId, userId)
	appTokenX.SetService(&service)
	appTokenStr, _ := appTokenX.Build()
	println(appTokenStr)

	// Example1: only allow audio
	// service := apptoken.CreateService(channelId, userId)
	// service.AddAudioPublishPrivilege()
	// appTokenX.SetService(&service)
	// appTokenStr, _ := appTokenX.Build()
	// println(appTokenStr)

	// Example2: only allow audio and video
	// service := apptoken.CreateService(channelId, userId)
	// service.AddAudioPublishPrivilege()
	// service.AddVideoPublishPrivilege()
	// appTokenX.SetService(&service)
	// appTokenStr, _ := appTokenX.Build()
	// println(appTokenStr)

	// By default, no more additional options are set. You can set more options individually as shown in the example below.
	// Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail options informations.
	// Example1: add more options
	// service := apptoken.CreateService(channelId, userId)
	// appTokenX.SetService(&service)

	// tokenOptions := apptoken.CreateAppTokenOptions()
	// options := make(map[string]string)
	// options["duration_per_channel"] = "86400"
	// options["delay_close_per_channel"] = "60"
	// tokenOptions.SetEngineOptions(options)
	// appTokenX.SetOptions(&tokenOptions)

	// appTokenStr, _ := appTokenX.Build()
	// println(appTokenStr)
}
