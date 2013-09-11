#include <Servo.h> 
#include <ros.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Float32.h>
#include <std_msgs/UInt32.h>
#include <SoftwareSerial.h>

// Values of 0 being sent using serial.write() 
// have to be casted to a byte to stop them 
// being misinterpereted as NULL
// This is a bug with arduino 1

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

#define M_RX                0x02    // RX and TX pins used for motorController
#define M_TX                0x03
#define STEPS_PER_REV       360
#define WHEEL_DIAMETER      10      // 10 cm
#define DIST_PER_REV        WHEEL_DIAMETER*PI
#define DIST_PER_STEP       DIST_PER_REV/double(STEPS_PER_REV)
#define STEPS_PER_CM        STEPS_PER_REV/double(DIST_PER_REV)
#define MOTOR_DELAY         5       // delay in ms to allow the motor controller to react

//Other General 
SoftwareSerial motorController = SoftwareSerial(M_RX, M_TX);
Servo servo;
const int pingPin = 11;
long encValue     = 0;
byte softwareRev  = 0;

// ROS Related
std_msgs::Float32 sonar_msg;
std_msgs::Float32 motorVolt_msg;
ros::Publisher pub_sonar("sonar", &sonar_msg);
ros::Publisher pub_motorVolt("motorVolt", &motorVolt_msg);
ros::NodeHandle  nh;
/*
void servo_cb( const std_msgs::UInt16& cmd_msg)
{
  //set servo angle, should be from 0-180  
  servo.write(cmd_msg.data); 
  //toggle led  
  digitalWrite(13, HIGH-digitalRead(13));
}
ros::Subscriber<std_msgs::UInt16> sub_servo("servo", servo_cb);
*/

byte readVolts()
{
  // Function reads current for both motors and battery voltage
  byte batteryVolts, mot1_current, mot2_current = 0;
  motorController.write(CMD);
  // Send byte to readbattery voltage and motor currents
  motorController.write(GET_VI);                                          
  // Wait for the 3 bytes to become available then get them
  while(motorController.available() < 3){}              
  batteryVolts = motorController.read();
  mot1_current = motorController.read();
  mot2_current = motorController.read();
  return batteryVolts;
}

// Function to read the value of both encoders, returns value of first encoder
long readEncoder()
{
  long result1 = 0; 
  long result2 = 0;
  motorController.write(CMD);
  motorController.write(GET_ENCODERS);
  // Wait for 8 bytes, first 4 encoder 1 values second 4 encoder 2 values 
  while(motorController.available() < 8)
  {
  }
  // First byte for encoder 1, HH.
  result1 = motorController.read();                 
  result1 <<= 8;
  // Second byte for encoder 1, HL
  result1 += motorController.read();                
  result1 <<= 8;
  // Third byte for encoder 1, LH
  result1 += motorController.read();                
  result1 <<= 8;
  // Fourth byte for encoder 1, LL
  result1  += motorController.read();               
  result2 = motorController.read();
  result2 <<= 8;
  result2 += motorController.read();
  result2 <<= 8;
  result2 += motorController.read();
  result2 <<= 8;
  result2 += motorController.read();
  return result1;                                   
}

// Reset the encoder registers to 0
void resetEncoders()
{
  motorController.write(CMD);     
  motorController.write(RESET_ENCODERS);
  delay(MOTOR_DELAY);
}

void stopMotors()
{
  int stopSpeed = 0;
  motorController.write(CMD);
  motorController.write(SET_SPEED2_TURN);
  motorController.write(stopSpeed);

  motorController.write(CMD);
  motorController.write(SET_SPEED1);
  motorController.write(stopSpeed);  
}

void move(double distance,int speed)
{
  resetEncoders();
  // Distance is in cm 
  double stepsPerDistance  = STEPS_PER_CM*distance;
  //Serial.write("Steps to move:");  
  //Serial.print(stepsPerDistance,DEC);
  //Serial.write("Encoder Value:");  
  //Serial.println(encValue,DEC);
  if(speed<0)
  {
    stepsPerDistance*=-1;
    while(encValue > stepsPerDistance)
    {
      // Set motors to drive forward at specified speed
      motorController.write(CMD);                  
      motorController.write(SET_SPEED1);
      motorController.write(speed);
      encValue = readEncoder();
      //Serial.print("Encoder Value:");  
      //Serial.print(encValue,DEC); 
      //Serial.print(" distance in cm:"); 
      //Serial.println(DIST_PER_STEP*encValue,DEC);
      delay(MOTOR_DELAY);
    }
  }
  else
  {
    while(encValue < stepsPerDistance)
    {
      // Set motors to drive backwards at specified speed
      motorController.write(CMD);                  
      motorController.write(SET_SPEED1);
      motorController.write(speed);
      encValue = readEncoder();
      //Serial.print("Encoder Value:");  
      //Serial.print(encValue,DEC); 
      //Serial.print(" distance in cm:"); 
      //Serial.println(DIST_PER_STEP*encValue,DEC);
      delay(MOTOR_DELAY);
    }
  }
  int stopSpeed = 0;
  motorController.write(CMD);
  motorController.write(SET_SPEED1);
  motorController.write(stopSpeed);  
}

void rotate(int rotationalSpeed, int angle2Rotate)
{
  resetEncoders();
  double axisDistance = 0.343; // meters
  double prevAngle = 0;
  double angle = 0;
  double oldEncoder = 0;
  double encoderDiff;

  while(abs(angle*180/PI)<angle2Rotate)
  {
    motorController.write(CMD);
    motorController.write(SET_SPEED2_TURN);
    motorController.write(rotationalSpeed);  
    encValue = readEncoder();
    encoderDiff = encValue - oldEncoder;
    oldEncoder = encValue;    
    //angle = prevAngle + 2*velocity*deltaT/axisDistance;
    angle = prevAngle + 2*DIST_PER_STEP*encoderDiff/(100*axisDistance);
    prevAngle = angle;
    //Serial.write("Encoder Value:");  
    //Serial.print(encValue,DEC); 
    //Serial.print(" Angle: "); 
    //Serial.println(angle*180/PI,DEC);    
    delay(MOTOR_DELAY);    
  }
  int stopSpeed = 0;
  motorController.write(CMD);
  motorController.write(SET_SPEED2_TURN);
  motorController.write(stopSpeed);
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
  delayMicroseconds(5);              // with 5 microsecond burst
  digitalWrite(pingPin, LOW);        // End ranging
  pinMode(pingPin, INPUT);           // Set pin to INPUT
  long duration = pulseIn(pingPin, HIGH); // Read echo pulse
  distanceCm    = microsecondsToCentimeters(duration);
  //Serial.println(distanceCm);      // Display result
  //delay(200);                      // Short delay  
  return distanceCm;
}

void setup()
{
  pinMode(13, OUTPUT);
  nh.getHardware()->setBaud(115200);
  nh.initNode();
  //nh.subscribe(sub_servo);
  nh.advertise(pub_sonar);
  nh.advertise(pub_motorVolt);
  servo.attach(8);
  
  motorController.begin(19200);  
  stopMotors();
  
  // Set MD25 accelleration value
  motorController.write(CMD);                                            
  motorController.write(SET_ACCELERATION);
  motorController.write(10);

  // Wait for this to be processed
  delayMicroseconds(10);                                        
  resetEncoders();
  // Wait for this to be processed
  delayMicroseconds(10);                                        
  
  // Set mode to 2, Both motors controlled by writing to speed 1
  motorController.write(CMD);                                            
  motorController.write(SET_MODE);
  motorController.write(3);    

  // Get software version of MD25
  motorController.write(CMD);                                            
  motorController.write(GET_VER);

  // Wait for byte to become available
  while(motorController.available() < 1){}                                        
  softwareRev = motorController.read();  
}

long int lastMillis = millis();
void loop()
{
  if( (millis() - lastMillis) > 20)
  {
    lastMillis = millis();    
    sonar_msg.data = sonarDist();
    nh.spinOnce();
    motorVolt_msg.data = readVolts();
    nh.spinOnce();
    pub_sonar.publish(&sonar_msg);
    nh.spinOnce();
    pub_motorVolt.publish(&motorVolt_msg);
    nh.spinOnce();
  }
  nh.spinOnce();
  delay(1);
}
