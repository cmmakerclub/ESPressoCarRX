
/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <ESP_Adafruit_SSD1306.h>


#define buffer_size 100
//#define OLED_RESET 4

#define period_udp 10
#define period_led 1000
#define period_control_motor 80

#define motorF 12
#define motorB 13
#define motorL 14
#define motorR 15


//Adafruit_SSD1306 display(OLED_RESET);

/* Set these to your desired credentials. */
const char *ssid = "kkao_server";
const char *password = "12345678";

unsigned int localPort = 12345;
unsigned long time_now, time_prev_udp, time_prev_led, time_prev_control_motor, timeCheckDisconnect;
int cb = 1;
int8_t sumCheck;
int tempCh1, tempCh2;
int outputControlForward, outputControlBackward, outputControlLeft, outputControlRight;

boolean dataTrue = true;

char packetBuffer[buffer_size] = {0}; //buffer to hold incoming and outgoing packets
WiFiUDP udp;
int count = 0;

IPAddress local_ip(192, 168, 5, 1);
IPAddress gateway(192, 168, 5, 1);
IPAddress subnet(255, 255, 255, 0);


void Read_UDP();
void setup_oled();
void controlMotor();

void setup()
{
  WiFi.disconnect();
  pinMode(motorF, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(motorL, OUTPUT);
  pinMode(motorR, OUTPUT);
  pinMode(16, OUTPUT);
  digitalWrite(16, 1);


  //delay(3000);


  Serial.begin(115200);



  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  delay(500);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(500);
  IPAddress myIP = WiFi.softAPIP();


  if ( udp.begin(localPort) > 0) {
    //Serial.println(myIP);
  } else {

  }

  //delay(1000);

  time_now = millis();
  time_prev_udp = time_now;
  time_prev_led = time_now;
  time_prev_control_motor = time_now;
  timeCheckDisconnect = time_now;

}

void loop()
{
  time_now = millis();
  Read_UDP();// stand by read UDP
  //controlMotor();
//  if (time_now - time_prev_control_motor >= period_control_motor)
//  {
//    time_prev_control_motor = time_now;
//
//    //digitalWrite(12, 1);    // debug
//    //Read_UDP();
//    // digitalWrite(12, 0);    // debug
//    //do
//    if (dataTrue) {
//      controlMotor();
//    }
//
//
//
//  }//end control motor loop

  //  if (time_now - time_prev_led >= period_led)    // 10Hz
  //  {
  //    time_prev_led = time_now;
  //
  //    //   display.clearDisplay();
  //    //   display.setCursor(0, 0);
  //    //   display.println("CMMC.Espresso_lite");
  //    // display.println("");
  //    //   display.print((int8_t)packetBuffer[0]);
  //    //   display.print("  ");
  //    //   display.print((int8_t)packetBuffer[1]);
  //    //   display.print("  ");
  //    //   display.print((int8_t)packetBuffer[2]);
  //    //   display.print("  ");
  //    //   display.print((int8_t)packetBuffer[3]);
  //    //   display.print("                            ");
  //    //   display.display();
  //
  //
  //  }

  //delay(1);
}//end void loop

void Read_UDP()
{
  cb = udp.parsePacket();
  if (!cb) {
    if (time_now - timeCheckDisconnect > 1000) {// When no data from UDP > 500ms car wil stop
      timeCheckDisconnect = time_now;
      //Serial.println("Disconnect!");
      packetBuffer[1] = 0;
      packetBuffer[2] = 0;
      dataTrue = true;
      controlMotor();


    }
  }
  else {
    //Serial.print("length=");
    //Serial.println(cb);
    timeCheckDisconnect = time_now;

    memset(packetBuffer, 0, buffer_size); // clear mem

    udp.read(packetBuffer, cb); // read the packet into the buffer
    // String temp = packetBuffer;
    // Serial.println(packetBuffer);

    // if (count > 8)
    // {
    //   count = 0;
    //   display.clearDisplay();
    //   display.setCursor(0, 0);
    // }
    // count++;

    // display.print((int)packetBuffer[0]);
    // display.print("  ");
    // display.print((int)packetBuffer[1]);
    // display.print("  ");
    // display.print((int)packetBuffer[2]);
    // display.print("  ");
    // display.print((int)packetBuffer[3]);
    // display.println("  ");
    // display.display();
    sumCheck = (int8_t)packetBuffer[1] + (int8_t)packetBuffer[2] + (int8_t)packetBuffer[3] + (int8_t)packetBuffer[4];

    if (sumCheck == (int8_t)packetBuffer[5]) {
      dataTrue = true;
      //      Serial.print(" check bit ");
      //      Serial.print((int8_t)packetBuffer[0]);
      //      Serial.print("  ch1:");
      //      Serial.print((int8_t)packetBuffer[1]);
      //      Serial.print("  ch2:");
      //      Serial.print((int8_t)packetBuffer[2]);
      //      Serial.print("  ch3:");
      //      Serial.print((int8_t)packetBuffer[3]);
      //      Serial.print("  ch4:");
      //      Serial.print((int8_t)packetBuffer[4]);
      //      Serial.print("  sumAll:");
      //      Serial.print((int8_t)packetBuffer[5]);
      //      Serial.print("  sum check:");
      //      Serial.println(sumCheck);
      controlMotor();
    } else {
      dataTrue = false;
    }

    //    packetBuffer[0] = (packetBuffer[0] + 85);
    //    packetBuffer[1] = (packetBuffer[1] + 85);
    //    packetBuffer[3] = (packetBuffer[3] + 85);

    //    int ch3Temp = packetBuffer[2];
    //    ch3Temp = map(ch3Temp, 0, 100, 0, 255);
    //
    //    int ch1 = packetBuffer[0] * 1.4825;
    //    int ch2 = packetBuffer[1] * 1.4825;
    //    int ch3 = ch3Temp ;
    //    int ch4 = packetBuffer[3] * 1.4825;
    //
    //    Serial.print("a");
    //    Serial.print(ch2);
    //    Serial.print("b");
    //    Serial.print(ch1);
    //    Serial.print("c");
    //    Serial.print(ch3);
    //    Serial.print("d");
    //    Serial.print(ch4);
    //    Serial.println("!");

    //    Serial.write((int8_t)packetBuffer[0]);  // roll
    //      Serial.write((int8_t)packetBuffer[1]);  // pitch
    //      Serial.write((int8_t)packetBuffer[2]);  // throttle
    //      Serial.write((int8_t)packetBuffer[3]);  // yaw
    //      Serial.write(0xfe);
    digitalWrite(16, !digitalRead(16));



  }
}

/*
   Function controlMotor()
   4 output  F B  L  R
   output range 0 - 1023
*/

void controlMotor() {
  //outputControlForward,outputControlBackward,outputControlLeft,outputControlRight;
  /*tempCh1,tempCh2
     use CH1 and Ch2 control car
  */
  //tempCh1 = (int8_t)packetBuffer[1];
  // tempCh2 = (int8_t)packetBuffer[2];

  //Stick center
  if ((int8_t)packetBuffer[2] == 0) {
    outputControlForward = 0;
    outputControlBackward =  0;
  }

  if ((int8_t)packetBuffer[1]  == 0) {
    outputControlRight = 0;
    outputControlLeft = 0;
  }
  //

  if ((int8_t)packetBuffer[2] > 0) { //if ch1 > 0  car will forward
    outputControlForward = map((int8_t)packetBuffer[2], 0, 60, 0, 1023);
    outputControlBackward = 0;
  }
  if ((int8_t)packetBuffer[2] < 0) { //if ch1 < 0  car will backward
    outputControlForward = 0;

    outputControlBackward =  map(abs((int8_t)packetBuffer[2]), 0, 60, 0, 1023);
  }

  if ((int8_t)packetBuffer[1] > 0) { //if ch2 > 0  car will right
    outputControlLeft = 0;
    outputControlRight = map((int8_t)packetBuffer[1], 0, 35, 0, 1023);
  }
  if ((int8_t)packetBuffer[1] < 0) { //if ch2 < 0  car will left
    outputControlRight = 0;
    outputControlLeft = map(abs((int8_t)packetBuffer[1]), 0, 35, 0, 1023);
  }


  /*
     debug
  */
    Serial.print(" F:");
    Serial.print(outputControlForward);
    Serial.print(" B:");
    Serial.print(outputControlBackward);
    Serial.print(" L:");
    Serial.print(outputControlLeft);
    Serial.print(" R:");
    Serial.println(outputControlRight);
  /*

  */

  /*
     sending output
  */
  outputControlForward = constrain(outputControlForward, 0, 1023);
  outputControlBackward = constrain(outputControlBackward, 0, 1023);
  outputControlLeft = constrain(outputControlLeft, 0, 1023);
  outputControlRight = constrain(outputControlRight, 0, 1023);
  // limits range of sensor values to between 0 and 1023
  analogWrite(motorF, outputControlForward);
  analogWrite(motorB, outputControlBackward);
  analogWrite(motorL, outputControlLeft);
  analogWrite(motorR, outputControlRight);
  /*

  */


}//end void controlMotor()





