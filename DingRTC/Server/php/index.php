<?php

require_once  'app_token_options.php';
require_once  'service.php';
require_once  'app_token.php';

$channel_id = 'channelId';
$user_id = 'userId';
$service = new Service($channel_id, $user_id);
$service->add_video_publish_privilege();

$options = new AppTokenOptions();
$options->set_engine_options(['k1' => 'v1', 'k2' => 'v2']);

$appId = 'appId';
$appKey = 'appKey';
$timestamp = time() + 1 * 86400;

$appToken = new AppToken($appId, $appKey, $timestamp);
$appToken->setService($service);
$appToken->setOptions($options);

$token = $appToken->build();

echo $token . PHP_EOL;

$appTokenX = AppToken::parse($token);

if ($appTokenX->validate($appKey)) {
    echo 'true' . PHP_EOL;
} else {
    echo 'false' . PHP_EOL;
}
