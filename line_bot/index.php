<?php
$access_token = 'VMK+KjxScanu5wbQOsLgjjny3xWEz/FRwauAHmEi/7/b7KzSuPa/yC5WDWnhKU52I4OsGPh9CIzoIggzFAd08M9LjnMOs+mXsS9lbgxavmoM2pz9SaZQcJpjyGS9103e88DvuzOCIuAaFTRtdBJQtAdB04t89/1O/w1cDnyilFU=';


// Get POST body content
$content = file_get_contents('php://input');
// Parse JSON
$events = json_decode($content, true);
// Validate parsed JSON data
if (!is_null($events['events'])) {
	// Loop through each event
	foreach ($events['events'] as $event) {
		$url = "";
		$data = "";
		
		// Reply only when message sent is in 'text' format
		if ($event['type'] == 'message' && $event['message']['type'] == 'text') {
			// Get text sent
			$text = $event['message']['text'];
			$reply = "";
			
			if(strpos($text, " ")!=false) {
				$subtext = explode(" ", $text);
				if(in_array($subtext[0],array("พูดว่า","พิมพ์","ส่งข้อความ"))) {
					$text = ".";
					$reply = $subtext[1];
				} else if(strpos(' '.$subtext[0], "ชื่อ")!=false || strpos(' '.$subtext[0], "Name")!=false) {
					$text = "N".$subtext[1];
					$reply = "[เปลี่ยนชื่อเป็น ".$subtext[1]."...]";	
				} else if(strpos(' '.$subtext[0], "น้ำหนัก")!=false || strpos(' '.$subtext[0], "Weight")!=false) {
					$text = "K".$subtext[1];
					$reply = "[เปลี่ยนน้ำหนักเป็น ".$subtext[1]."...]";
				} else if(strpos(' '.$subtext[0], "อายุ")!=false || strpos(' '.$subtext[0], "Age")!=false) {
					$text = "A".$subtext[1];
					$reply = "[เปลี่ยนอายุเป็น ".$subtext[1]."...]";
				} else if(strpos(' '.$subtext[0], "ตื่น")!=false || strpos(' '.$subtext[0], "Wake")!=false) {
					$text = "W".$subtext[1];
					$reply = "[เปลี่ยนเวลาตื่นเป็น ".$subtext[1]."...]";
				} else if(strpos(' '.$subtext[0], "นอน")!=false || strpos(' '.$subtext[0], "Sleep")!=false) {
					$text = "Z".$subtext[1];
					$reply = "[เปลี่ยนเวลานอนเป็น ".$subtext[1]."...]";
				} else if(strpos(' '.$subtext[0], "เวลา")!=false || strpos(' '.$subtext[0], "นาฬิกา")!=false || strpos(' '.$subtext[0], "Time")!=false) {
					$text = "T".$subtext[1];
					$reply = "[ตั้งเวลาเป็น ".$subtext[1]."...]";
				} else if(strpos(' '.$subtext[0], "วัน")!=false || strpos(' '.$subtext[0], "ปฏิทิน")!=false || strpos(' '.$subtext[0], "Date")!=false) {
					$text = "D".$subtext[1];
					$reply = "[ตั้งวัน/เดือน เป็น ".$subtext[1]."...]";
				}
				
			}
			else if(in_array($text,array("เปิดไฟ","1")) || strpos(' '.$text, "เปิด")!=false) {
				$text = "1";
				$reply = "[สั่งเปิด...]";
			}
			else if(in_array($text,array("ปิดไฟ","0")) || strpos(' '.$text, "ปิด")!=false) {
				$text = "0";
				$reply = "[สั่งปิด...]";
			}
			
			if($text != "") {
				// Get replyToken
				$replyToken = $event['replyToken'];
				if($text == ".") {
					$resp = "Success";
				} else {
					$resp = json_decode(sendMsg($text.'|'.$event['source']['userId'].$event['source']['groupId']),true);
					$resp = $resp['message'];
				}
					
				
				
				// Build message to reply back
				$messages = [
					'type' => 'text',
					'text' => ($resp=='Success')? $reply:$resp
				];
					
				$url = 'https://api.line.me/v2/bot/message/reply';
				$data = [
					'replyToken' => $replyToken,
					'messages' => [$messages]
				];
				
				// Make a POST Request to Messaging API to reply to sender
				$post = json_encode($data);
				$headers = array('Content-Type: application/json', 'Authorization: Bearer ' . $access_token);
				
				$ch = curl_init($url);
				curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
				curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
				curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
				curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
				curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
				$result = curl_exec($ch);
				curl_close($ch);

				echo $result . "\r\n";
				
			}
			
		} 
	
		
			
		
	}
}
 
 

function sendMsg($tmsg)
{
	$APPID= 'TreeMinder'; //enter your appid
	$KEY = 'BhdMgDxcvLSxtN8'; //enter your key
	$SECRET = 'uiRG8h2PAhohYr2eSqSZ2lyo4'; //enter your secret
	$ALIAS = 'tree'; //same alias you set on NodeCMU
	
	$url = "https://api.netpie.io/microgear/".$APPID."/".$ALIAS."?retain&auth=".$KEY.":".$SECRET;
      //https://api.netpie.io/microgear/TreeMinder/tree?retain&auth=BhdMgDxcvLSxtN8:uiRG8h2PAhohYr2eSqSZ2lyo4
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS,$tmsg);
	curl_setopt($ch, CURLOPT_USERPWD, "{$KEY}:{$SECRET}");
    $response = curl_exec($ch);
    return "$response\n";
}

echo 'OK';

?>