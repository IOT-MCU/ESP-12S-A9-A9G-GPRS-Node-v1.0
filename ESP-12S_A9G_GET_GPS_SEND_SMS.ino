
#include<stdio.h>
#include<string.h>
#define DEBUG true

#include <SoftwareSerial.h>

#define BAUD_RATE 115200
#define A9_BAUD_RATE 115200
String phonenum="XXXXXXXX";          //Please change the xxxxxxx to your phone number

/***********************************/
#define A9G_PON     16  //ESP12 GPIO16 A9/A9G POWON
#define A9G_POFF    15  //ESP12 GPIO15 A9/A9G POWOFF
#define A9G_WAKE    13  //ESP12 GPIO13 A9/A9G WAKE
#define A9G_LOWP    2  //ESP12 GPIO2 A9/A9G ENTER LOW POWER MODULE

void handleInterrupt();
void SendTextMessage(String message,String phonenum);
int A9GPOWERON();

/***********************************/
SoftwareSerial swSer(14, 12, false, 256);
void setup() {
  Serial.begin(BAUD_RATE);
  swSer.begin(A9_BAUD_RATE);

  for (char ch = ' '; ch <= 'z'; ch++) {
    swSer.write(ch);
  }
  swSer.println("");

/************************************/
  pinMode(A9G_PON, OUTPUT);//LOW LEVEL ACTIVE
  pinMode(A9G_POFF, OUTPUT);//HIGH LEVEL ACTIVE
  pinMode(A9G_LOWP, OUTPUT);//LOW LEVEL ACTIVE

  digitalWrite(A9G_PON, HIGH); 
  digitalWrite(A9G_POFF, LOW); 
  digitalWrite(A9G_LOWP, HIGH); 

  Serial.println("After 2s A9G will POWER ON.");
  delay(2000);
  if(A9GPOWERON()==1)
  {
       Serial.println("A9G POWER ON.");
  }


  Serial.println("Enale the GPS");
  sendData("AT+GPS=1",1000,DEBUG);
  pinMode(A9G_WAKE, INPUT);//interruptPin
  attachInterrupt(digitalPinToInterrupt(A9G_WAKE), handleInterrupt, RISING);


/***********************************/


}

void loop() {

  while (swSer.available() > 0) {
    Serial.write(swSer.read());
 //   yield();
  }
  while (Serial.available() > 0) {
    swSer.write(Serial.read());
//    yield();
  }


  String GPS_position=sendData("AT+LOCATION=2",1000,DEBUG);
  if( GPS_position.indexOf("OK") >= 0 ){
     Serial.println("GET OK");
    String LG=GPS_position.substring(14,37);
    Serial.print("lg**********:");
    Serial.println(LG);  
    SendTextMessage(LG,phonenum);
    
    }
    delay(10000);
  
}




void SendTextMessage(String message,String phonenum)
{ 
 // sendData("AT+CMGF=1",1000,DEBUG);            //Set the SMS in text mode

 String ATCMD="AT+CMGS=";
 String cammand=ATCMD+"\""+ phonenum + "\"";
// Serial.println(cammand);
//  sendData(cammand,1000,DEBUG);
 // sendData(message,1000,DEBUG);                //the content of the message
   swSer.println("AT+CMGF=1");
   swSer.println(cammand);
   swSer.println(message); 
   swSer.write(26); 

}




void handleInterrupt() {
  Serial.println("An interrupt has occurred.");
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";    
    swSer.println(command); 
    long int time = millis();   
    while( (time+timeout) > millis())
    {
      while(swSer.available())
      {       
        char c = swSer.read(); 
        response+=c;
      }  
    }    
    if(debug)
    {
      Serial.print(response);
    }    
    return response;
}

int A9GPOWERON()
{
      digitalWrite(A9G_PON, LOW);
      delay(3000);
      digitalWrite(A9G_PON, HIGH);
      delay(5000);
      String msg = String("");
      msg=sendData("AT",1000,DEBUG);
      if( msg.indexOf("OK") >= 0 ){
          Serial.println("GET OK");
          return 1;
      }
      else {
         Serial.println("NOT GET OK");
         return 0;
      }
}

int A9GPOWEROFF()
{
      digitalWrite(A9G_POFF, HIGH);
      delay(3000);
      digitalWrite(A9G_POFF, LOW);
      delay(5000);
      String msg = String("");
      msg=sendData("AT",1000,DEBUG);
      if( msg.indexOf("OK") >= 0 ){
          Serial.println("GET OK");
          return 1;
      }
      else {
          Serial.println("NOT GET OK");
          return 0;
      }
}

int A9GENTERLOWPOWER()
{
      String msg = String("");
      msg=sendData("AT+SLEEP=1",1000,DEBUG);
      if( msg.indexOf("OK") >= 0 ){
          digitalWrite(A9G_LOWP, LOW);
          return 1;
      }
      else {
         return 0;
      }
}
