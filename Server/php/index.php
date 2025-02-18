<?php

require_once  'app_token_options.php';
require_once  'service.php';
require_once  'app_token.php';


// find appId in your RTC console (https://rtc.console.aliyun.com/#/manage/list)
$appId = 'replace_your_appId';
// find appKey in your RTC console
$appKey = 'replace_your_appKey';
// Token is valid for a maximum of 24 hours. This example uses 12 hours, adjust according to your needs.
$expiredTs = time() + 12 * 60 * 60;
$channel_id = 'replace_your_channelId';
$user_id = 'replace_your_userId';

$appToken = new AppToken($appId, $appKey, $expiredTs);

// By default, all privileges are allowed. You can control audio/video/screen privileges individually as shown in the example below.
// Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail privilege informations.
// Example0: full privileges as default	
$service = new Service($channel_id, $user_id);
$appToken->setService($service);
$token = $appToken->build();
echo $token . PHP_EOL;

// Example1: only allow audio
// $service = new Service($channel_id, $user_id);
// $service->add_audio_publish_privilege();
// $appToken->setService($service);
// $token = $appToken->build();
// echo $token . PHP_EOL;

// Example2: only allow audio and video
// $service = new Service($channel_id, $user_id);
// $service->add_audio_publish_privilege();
// $service->add_video_publish_privilege();
// $appToken->setService($service);
// $token = $appToken->build();
// echo $token . PHP_EOL;

// By default, no more additional options are set. You can set more options individually as shown in the example below.
// Please check(https://help.aliyun.com/document_detail/2689025.html) for more detail options informations.
// Example1: add more options
// $service = new Service($channel_id, $user_id);
// $appToken->setService($service);
// $options = [
//     "duration_per_channel" => "86400",
//     "delay_close_per_channel" => "60"
// ];
// $tokenOptions = new AppTokenOptions($options);
// $appToken->setOptions($tokenOptions);
// $token = $appToken->build();
// echo $token . PHP_EOL;

