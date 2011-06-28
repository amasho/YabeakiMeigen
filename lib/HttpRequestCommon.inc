<?php

require_once("/usr/share/pear/XML/Unserializer.php");

class HttpRequestCommon
{
	private $_mh;
	private $_ch_array;
	private $_req_data;
	private $_result;

	/*
	 * コンストラクタ
	 */
	public function __construct(){
		$this->_mh = curl_multi_init();
		$this->_ch_array = array();
		$this->_req_data = array();
		$this->_result = array();
	}

	/**
	 * リクエストデータとcURLのオプションを設定する
	 *
	 * @param data mixed リクエストデータ
	 * @note
	 * URL配列にcURLのオプションをマッピング
	 * ex:
	 * $req_data = array(
	 * 		"http://hogehoge.com/foo.html" => array(
	 * 			CURLOPT_RETURNTRANSFER => TRUE,
	 * 			CURLOPT_TIMEOUT => 1
	 * 		),
	 * 		"http://hugahuga.com/var.html" => array(
	 * 			CURLOPT_RETURNTRANSFER => TRUE,
	 * 			CURLOPT_TIMEOUT => 2,
	 * 			CURLOPT_NOBODY => TRUE
	 * 		)
	 * );
	 */
	public function setRequestData($data = array()){
		$this->_req_data = $data;
	}

	/**
	 * cURLをマルチで実行する
	 *
	 * @retval result mixed 各サイトへcURLで取得した結果の配列
	 */
	public function httpRequest(){
		$this->_set_handlers();
		do{
			curl_multi_exec($this->_mh, $status);
		} while($status);

		$request = null; $code = 0; $err = null;
		foreach(array_keys($this->_req_data) as $url){
			$this->_result[$url] = curl_getinfo($this->_ch_array[$url]);
			$code = $this->_result[$url]["http_code"];
			if($this->_result[$url]["http_code"] !== 200){
				error_log(__FUNCTION__ . "#" . __LINE__ . ": Http Status Code = " . $code);
				error_log(__FUNCTION__ . "#" . __LINE__ . ": Error = " . curl_error($this->_ch_array[$url]));
			}
			$this->_result[$url]['contents'] = curl_multi_getcontent($this->_ch_array[$url]);
			curl_multi_remove_handle($this->_mh, $this->_ch_array[$url]);
			curl_close($this->_ch_array[$url]);
		}
		curl_multi_close($this->_mh);

		return $this->_result;
	}

	/**
	 * 配列にして返す
	 */
	public function getUnserializedResult($url){
		return $this->_unserializeXML($this->_result[$url]['contents']);
	}

	/**
	 * cURLハンドラを設定する
	 */
	private function _set_handlers(){
		foreach($this->_req_data as $url => $opts){
			$this->_ch_array[$url] = curl_init($url);
			curl_setopt_array($this->_ch_array[$url], $opts);
			curl_multi_add_handle($this->_mh, $this->_ch_array[$url]);
		}
	}

	/**
	 * XMLから配列に変換して返す
	 *
	 * @param xml string XML文字列
	 * @retval HTTPレスポンス
	 */
	private function _unserializeXML($xml){
		$unserializer = new XML_Unserializer();
		$unserializer->setOption('parseAttributes', true);
		if(($unserializer->unserialize($xml)) === true)
			return array(
				$unserializer->getRootName() => $unserializer->getUnserializedData());

		return false;
	}
}
