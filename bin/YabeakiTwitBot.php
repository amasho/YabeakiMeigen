<?php
/**
 * yabeaki_meigen bot
 * つぶやき元のネタ抽出 -> 厳選した１件をつぶやく
 */
require_once('../lib/HttpRequestCommon.inc');
require_once('../lib/TwitterOAuth.inc');

	$mem_usage = memory_get_usage(true);
	error_log("Start memory usage: " . (memory_get_usage(true) - $mem_usage));

	$twitter_setting = parse_ini_file('../conf/twitter.conf');
	$consumer_key = $twitter_setting['consumer_key'];
	$consumer_secret = $twitter_setting['consumer_secret'];
	$oauth_access_token = $twitter_setting['oauth_access_token'];
	$oauth_access_token_secret = $twitter_setting['oauth_access_token_secret'];

	$count = 100;
	$ret = array(); $timeline = array();
	for($i=0; $i<5;$i++){
		$page = rand(1, 32);
		$url = "http://twitter.com/statuses/user_timeline/yabeaki.xml?count=$count&page=$page";
		$data = array(
			$url => array(
				CURLOPT_RETURNTRANSFER => TRUE,
				CURLOPT_TIMEOUT => 20 
			)
		);

		$curl_request = new HttpRequestCommon();
		$curl_request->setRequestData($data);
		$curl_request->httpRequest();
		$ret = $curl_request->getUnserializedResult($url);

		if(!is_array($ret)){
			error_log("##### Tweet feed get error.");
			continue;
		}

		$timeline = array_merge($timeline, $ret['statuses']['status']);
	}

	if(!is_array($timeline)){
		error_log("##### Timeline error. Maybe API access error.");
		continue;
	}

	$i = rand(0, count($timeline));
	do {
		shuffle($timeline);

		$tweet = mb_convert_encoding($timeline[$i]['text'], 'EUC-JP', 'UTF-8');

		if(preg_match("/(\@|RT|ぷれいなう。|http:\/\/|うしほー|よるほー|\#4ji|\#nowplaying|Yahoo|震度|地震|停電|原発|放射|揺れ|余震)/", $tweet)){
			error_log("##### Filter match. Not tweet -> $tweet");
			continue;
		}
		break;
	} while(true);
	
	if($tweet == "") exit;

	$tweet = "再渇：$tweet #followme #followmejp #followdaibosyu";

	$twitter_oauth = new TwitterOAuth($consumer_key, $consumer_secret);
	$result = $twitter_oauth->postTweet(
		$oauth_access_token, $oauth_access_token_secret, $tweet);

	error_log("Done memory usage: " . (memory_get_usage(true) - $mem_usage));

	exit;

