#include <Servo.h> 
#include <ros.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Float32.h>
#include <std_msgs/UInt32.h>
#include <SoftwareSerial.h>
//  MD25 command byte of 0
#define CMD                 (byte)0x00              
#define GET_SPEED           0x21
#define GET_SPEED           0x22
#define GET_ENCODER1        0x23
#define GET_ENCODER2        0x24
#define GET_ENCODERS        0x25
#define GET_VOLTS           0x26
#define GET_CURRENT1        0x27
#define GET_CURRENT2        0x28
#define GET_VER             0x29
#define GET_ACCELERATION    0x2A    //returns the current acceleration level
#define GET_MODE            0x2B    //returns the currently selected mode
#define GET_VI              0x2C    //returns battery volts, motor1 current and then motor2 current 
#define SET_SPEED1          0x31    //set new speed1
#define SET_SPEED2_TURN     0x32    //set new speed2 or turn
#define SET_ACCELERATION    0x33    //set new acceleration
#define SET_MODE            0x34    //set the mode
#define RESET_ENCODERS      0x35    //zero both of the encoder counts
#define DISABLE_REGULATOR   0x36    //power output not changed by encoder feedback
#define ENABLE_REGULATOR    0x37    //power output is regulated by encoder feedback
#define DISABLE_TIMEOUT     0x38    //MD25 will continuously output with no regular commands
#define ENABLE_TIMEOUT      0x39    //MD25 output will stop after 2 seconds without communication

//Other General 
Servo servo;
SoftwareSerial mySerial(2,3); // RX, TX
const int pingPin = 11;

// ROS Related
std_msgs::Float32 sonar_msg;
std_msgs::Float32 motorVolt_msg;
ros::Publisher pub_sonar("sonar", &sonar_msg);
ros::Publisher pub_motorVolt("motorVolt", &motorVolt_msg);
ros::NodeHandle  nh;
void servo_cb( const std_msgs::UInt16& cmd_msg)
{
  servo.write(cmd_msg.data); //set servo angle, should be from 0-180  
  digitalWrite(13, HIGH-digitalRead(13));  //toggle led  
}
ros::Subscriber<std_msgs::UInt16> sub_servo("servo", servo_cb);

byte readVolts()
{
  // Function reads current for both motors and battery voltage
  byte batteryVolts, mot1_current, mot2_current = 0;
  mySerial.write(CMD);
  mySerial.write(GET_VI);                                          // Send byte to readbattery voltage and motor currents
  while(mySerial.available() < 3){}                                 // Wait for the 3 bytes to become available then get them
  batteryVolts = mySerial.read();
  mot1_current = mySerial.read();
  mot2_current = mySerial.read();
  delay(5);
  return batteryVolts;
}

double microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29.0 / 2.0;
}

double sonarDist()
{
  double distanceCm = 0;
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

void setup()
{
  pinMode(13, OUTPUT);
  nh.getHardware().(115200);
  nh.initNode();
  nh.subscribe(sub_servo);
  nh.advertise(pub_sonar);
  nh.advertise(pub_motorVolt);
  servo.attach(8); //attach it to pin 9
  mySerial.begin(19200);  
}

long int lastMillis = millis();
void loop()
{
  if( (millis() - lastMillis) > 100)
  {
    sonar_msg.data = sonarDist();
    nh.spinOnce();
    motorVolt_msg.data = readVolts();
    nh.spinOnce();
    pub_sonar.publish(&sonar_msg);
    nh.spinOnce();
    pub_motorVolt.publish(&motorVolt_msg);
    nh.spinOnce();
    lastMillis = millis();
  }
  nh.spinOnce();
  delay(1);
}
