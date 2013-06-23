#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include <WProgram.h>
#endif

#include <Servo.h> 
#include <ros.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Float32.h>
#include <std_msgs/UInt32.h>

std_msgs::Float32 sonar_msg;
const int pingPin = 11;

ros::Publisher pub_sonar("sonar", &sonar_msg);
ros::NodeHandle  nh;

Servo servo;

void servo_cb( const std_msgs::UInt16& cmd_msg)
{
  servo.write(cmd_msg.data); //set servo angle, should be from 0-180  
  digitalWrite(13, HIGH-digitalRead(13));  //toggle led  
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

long sonarDist()
{
  long distanceCm = 0;
  pinMode(pingPin, OUTPUT);          // Set pin to OUTPUT
  digitalWrite(pingPin, LOW);        // Ensure pin is low
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);       // Start ranging
  delayMicroseconds(5);              //   with 5 microsecond burst
  digitalWrite(pingPin, LOW);        // End ranging
  pinMode(pingPin, INPUT);           // Set pin to INPUT
  long duration = pulseIn(pingPin, HIGH); // Read echo pulse
  distanceCm    = microsecondsToCentimeters(duration);
  Serial.println(distanceCm);        // Display result
  delay(200);		             // Short delay  
  return distanceCm;
}

ros::Subscriber<std_msgs::UInt16> sub("servo", servo_cb);

void setup()
{
  pinMode(13, OUTPUT);
  nh.initNode();
  nh.subscribe(sub);
  servo.attach(8); //attach it to pin 9
}

void loop()
{
  nh.spinOnce();
  delay(1);
}
