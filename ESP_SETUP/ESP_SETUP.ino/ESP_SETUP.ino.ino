#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

#define APPID       "TreeMinder"
#define GEARKEY     "BhdMgDxcvLSxtN8"
#define GEARSECRET  "uiRG8h2PAhohYr2eSqSZ2lyo4"
#define ALIAS       "tree"

#define TRIGGER 16
#define ECHO    5
#define ssid  "NhaMhee"//"CE-502"//"RY"//"CE-501" //"NetworkLab" //เธญเธขเน�เธฒเธฅเธทเธกเน�เธ�เน�เน€เธ�เน�เธ�เธ�เธทเน�เธญ SSID "CE-ESL"
#define password  "12345678"//"CELab#502"//"rujiraveeyiwha"//"CELab#501"//"NwLabOnly"//เธญเธขเน�เธฒเธฅเธทเธกเน�เธ�เน�เน€เธ�เน�เธ� password "ceeslonly"
#define LINE_TOKEN "jCQlVX06m8Faa46xt0mOXjbcI6A6AA5LQxZ91pTFhQe"
#define APP_TOKEN "VMK+KjxScanu5wbQOsLgjjny3xWEz/FRwauAHmEi/7/b7KzSuPa/yC5WDWnhKU52I4OsGPh9CIzoIggzFAd08M9LjnMOs+mXsS9lbgxavmoM2pz9SaZQcJpjyGS9103e88DvuzOCIuAaFTRtdBJQtAdB04t89/1O/w1cDnyilFU="


bool Notified = false,Placed = true, wantname = false;
float cmFull=5,cmEmpty=15,cmNoGlass=15;
int nowThres = 0;
int kg = 65;
int pass = 0;
float drinkPerDay = 2.2*30*kg;
float drinkPerGlass = drinkPerDay/8;
float distance=0,lastdistance=0;
String replyToken = "",uname="";

WiFiClient client;
AuthClient *authclient;
MicroGear microgear(client);

void getDistance() {
    long duration;
    digitalWrite(TRIGGER, LOW);
    delayMicroseconds(2);
  
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);
  
    digitalWrite(TRIGGER, LOW);
    duration = pulseIn(ECHO, HIGH);
    distance = ((float)duration/2) / 29.1;
}



void Line_Push(String message) {
  //if(replyToken=="") return;
  message = "{\"messages\":[ {\"type\":\"text\",\"text\":\"" + message + "\"} ],\"to\":\"" + replyToken + "\"}";
  WiFiClientSecure client;

  if (!client.connect("api.line.me", 443)) {
    Serial.print("connect line fail  \n");
    delay(500);
  }

  String req = "";
  req += "POST /v2/bot/message/push HTTP/1.1\r\n";
  req += "Host: api.line.me\r\n";
  req += "Authorization: Bearer " + String(APP_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/json\r\n";
  req += "Content-Length: " + String(message.length()) + "\r\n";
  req += "\r\n";
  req += message;
  // Serial.println(req);
  client.print(req);

  delay(20);

  // Serial.println("-------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

}
void Line_Notify(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.print("connect line fail  \n");
    return;
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  client.print(req);

  delay(20);

  // Serial.println("-------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void cmdOpen() {
  if(distance>cmFull) {
      digitalWrite(LED_BUILTIN, HIGH);   // LED on
      Serial.write("O                   ");
  } else {
      Serial.write("C                   ");
      Line_Push("The cup is already filled!");
  }
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) { //
  
  msg[msglen] = '\0';
  String text = (char *)msg;
  replyToken = getValue(text, '|', 1);
  text = getValue(text, '|', 0);
  //Serial.print("LINE: ");
  //Serial.println(text);
  String cmd = "";
  if(wantname) {
    uname = text;
    wantname = false;
  }
  else if (text == "1") {
    cmdOpen();
  } else if (text == "0") {
    digitalWrite(LED_BUILTIN, LOW);  // LED off
    Serial.print("C                   ");
  }
  else {
    if(text[0] == 'K') {
      kg = text.substring(1).toInt();
    }
    
    cmd = text;
    int i =cmd.length();
    for(;i<20;i++) {
      cmd += " "; 
    }
  Serial.print(cmd);
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE\n");
  microgear.setName(ALIAS);
}



void setup() {
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);


  Serial.begin(9600); //เธ•เธฑเน�เธ�เธ�เน�เธฒเน�เธ�เน�เธ�เธฒเธ� serial เธ—เธตเน� baudrate 115200
  while (!Serial) {}; // wait for serial port to connect.
  Serial1.begin(9600);
  while (!Serial1) {};

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(CONNECTED, onConnected);


  delay(10);
  Serial.print("Connecting to WIFI  "); 
  //Serial.println(ssid); 
  //WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    //Serial.print("."); 
  }
  Serial.print("WiFi connected     \n"); 
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP()); 


  //microgear.resetToken();
  microgear.init(GEARKEY, GEARSECRET, ALIAS);
  microgear.connect(APPID);
//
//  long cmLast=0;
//  int acc=0;
//  while(acc<5) {
//    long cm = getDistance();
//    if(cmLast==getDistance()) {
//      acc++;
//    } else {
//      acc = 0;
//      cmLast = cm;
//    }
//    delay(100);
//  }
//  cmEmpty = cmLast;

}



void loop() {

    lastdistance = distance-1;
    getDistance();


    if(distance-1 <= lastdistance && lastdistance <= distance+1) {
      nowThres++;
      //Serial.print("nowThres: ");
      //Serial.println(nowThres);
    } else {
      nowThres = 0;
    }

    if(distance <= cmFull) {
        if(!Notified) {
          Line_Push("The cup is filled! Don't forget to Drink");
          //Line_Notify("The cup is filled! Don't forget to Drink");
          Serial.write("C                   ");
          Notified = true;
        }
      } 

      if(!Placed) {
           Placed = true;
           int drinkRatio = 10-((float)cmEmpty-distance)/(cmEmpty-cmFull)*10;
           Serial.write("V");
           if(drinkRatio>10) drinkRatio = 10;
           else if(drinkRatio<0) drinkRatio = 0;
           Serial.write(drinkRatio+'0');
           Serial.write(pass+'0');
           Serial.write("                 ");
      }

    if(nowThres == 3) {
      nowThres = 0;

    //Serial.write("M");
    //Serial.write(distance/10+'0');
    //Serial.write(distance%10+'0');
    //Serial.write("                \n");
    
      
      if(distance >= cmEmpty) {
        Notified = false;
        
      }
     else if(distance <= cmFull) {

      }

      

    }
    
  
    if (microgear.connected()) {
      microgear.loop();
      //Serial.println("connect...");
    } else {
      //Serial.print("connection lost, re\n");
      //microgear.connect(APPID);
    }
  
  if(Serial.available()>0) {
     char ask = Serial.read();
     if(ask=='C') {
        cmdOpen();
     } else if(ask>='a') {
        cmFull = 8-(2.2*30*(ask-'a')/2/8/100);
        //Serial.write("cmFull=            ");
        //Serial.write(cmFull+'0');
     }
     else if(ask>='V') {
        pass = ask-'V';
        Placed = false;
     } 
     
  }
  delay(1000);

}



