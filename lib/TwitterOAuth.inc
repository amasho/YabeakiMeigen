<?php
require_once("/usr/share/pear/HTTP/OAuth/Consumer.php");

class TwitterOAuth
{

	const REQUEST_TOKEN_URL = 'https://twitter.com/oauth/request_token'; 
	const AUTHORIZE_URL		= 'https://twitter.com/oauth/authorize';
	const STATUS_UPDATE_URL = 'https://twitter.com/statuses/update.xml';

	private $_oauth_consumer;

	public function __construct($key, $secret){
		$this->_oauth_consumer = new HTTP_OAuth_Consumer($key, $secret);
		$this->_consumer_token = array();
		$this->_oauth_consumer_token = array();
	}

	public function postTweet($oauth_token, $oauth_secret, $twit){
		$http_request = new HTTP_Request2();
		$http_request->setConfig('ssl_verify_peer', false);

		$consumer_request = new HTTP_OAuth_Consumer_Request();
		$consumer_request->accept($http_request);
		$this->_oauth_consumer->accept($consumer_request);

		session_start();

		$this->_oauth_consumer->getRequestToken(self::REQUEST_TOKEN_URL, "");  

		$auth_url = $this->_oauth_consumer->getAuthorizeUrl(self::AUTHORIZE_URL);  

		$this->_oauth_consumer->setToken($this->_oauth_consumer->getToken());
		$this->_oauth_consumer->setTokenSecret($this->_oauth_consumer->getTokenSecret());
		$this->_oauth_consumer->setToken($oauth_token);
		$this->_oauth_consumer->setTokenSecret($oauth_secret);

		$status = mb_convert_encoding($twit, 'UTF-8');
		$response = $this->_oauth_consumer->sendRequest(
						self::STATUS_UPDATE_URL, array('status' => $status), "POST");

		return $response->getBody(); 
	}

}
