<?php

do {
	system("php ./YabeakiTwitBot.php >> ../logs/yabeaki_bot.log");

	$interval = rand(10, 30) * 60;
	error_log("##### Interval time $interval sec");
	sleep($interval);

} while(true);

