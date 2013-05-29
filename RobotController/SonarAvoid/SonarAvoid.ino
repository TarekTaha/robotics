#include <Servo.h> 
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3); // RX, TX
/*
0x21	GET SPEED 1 	2 	1 	returns the current requested speed of motor 1
0x22	GET SPEED 2 	2 	1 	returns the current requested speed of motor 2
0x23	GET ENCODER 1 	2 	4 	motor 1 encoder count, 4 bytes returned high byte first (signed)
0x24	GET ENCODER 2 	2 	4 	motor 2 encoder count, 4 bytes returned high byte first (signed)
0x25	GET ENCODERS 	2 	8 	returns 8 bytes -  encoder1 count, encoder2 count
0x26	GET VOLTS 	2 	1 	returns the input battery voltage level
0x27 	GET CURRENT 1 	2 	1 	returns the current drawn by motor 1
0x28	GET CURRENT 2 	2 	1 	returns the current drawn by motor 1
0x29 	GET VERSION 	2 	1 	returns the MD25 software version
0x2A 	GET ACCELERATION 	2 	1 	returns the current acceleration level
0x2B 	GET MODE 	2 	1 	returns the currently selected mode
0x2C 	GET VI 	2 	3 	returns battery volts, motor1 current and then motor2 current 
0x31 	SET SPEED 1 	3 	0 	set new speed1
0x32 	SET SPEED 2 / TURN 	3 	0 	set new speed2 or turn
0x33 	SET ACCELERATION 	3 	0 	set new acceleration
0x34 	SET MODE 	3 	0 	set the mode
0x35 	RESET ENCODERS 	2 	0 	zero both of the encoder counts
0x36 	DISABLE REGULATOR 	2 	0 	power output not changed by encoder feedback
0x37 	ENABLE REGULATOR 	2 	0 	power output is regulated by encoder feedback
0x38 	DISABLE TIMEOUT 	2 	0 	MD25 will continuously output with no regular commands
0x39 	ENABLE TIMEOUT 	2 	0 	MD25 output will stop after 2 seconds without communication
*/
#define CMD                 (byte)0x00              //  MD25 command byte of 0
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

long encValue               = 0;
byte softwareRev            = 0;
const int pingPin           = 11;
const int motorSpeed        = 20;
const int avoidanceDistance = 25; //cm
long duration, distanceCm;
Servo myservo;

int reMapAngle(int angle)
{
  return map(angle,0,220,0,180);
}

long readEncoder()
{
  // Function to read and display the value of both encoders, returns value of first encoder
  long result1 = 0; 
  long result2 = 0;
  mySerial.write(CMD);
  mySerial.write(GET_ENCODERS);
  while(mySerial.available() < 8){}          // Wait for 8 bytes, first 4 encoder 1 values second 4 encoder 2 values 
  result1 = mySerial.read();                 // First byte for encoder 1, HH.
  result1 <<= 8;
  result1 += mySerial.read();                // Second byte for encoder 1, HL
  result1 <<= 8;
  result1 += mySerial.read();                // Third byte for encoder 1, LH
  result1 <<= 8;
  result1  += mySerial.read();               // Fourth byte for encoder 1, LL
  result2 = mySerial.read();
  result2 <<= 8;
  result2 += mySerial.read();
  result2 <<= 8;
  result2 += mySerial.read();
  result2 <<= 8;
  result2 += mySerial.read();
  delay(5);                                // Delay for LCD03 to process data
  return result1;                                   
}
  
void readVolts()
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
}

void setup()
{
  myservo.attach(8);
  Serial.begin(9600);
  mySerial.begin(19200);
  
  mySerial.write(CMD);                                            // Set MD25 accelleration value
  mySerial.write(SET_ACCELERATION);
  mySerial.write(10);
  delayMicroseconds(10);                                          // Wait for this to be processed

  mySerial.write(CMD);                                            // Reset the encoder registers to 0
  mySerial.write(RESET_ENCODERS);         
  mySerial.write(CMD);                                            // Set mode to 2, Both motors controlled by writing to speed 1
  mySerial.write(SET_MODE);
  mySerial.write(3);    
  
  mySerial.write(CMD);                                            // Get software version of MD25
  mySerial.write(GET_VER);
  while(mySerial.available() < 1){}                               // Wait for byte to become available         
  softwareRev = mySerial.read();  
  
}

unsigned int time_us=0;
unsigned int circle=0;
unsigned int dist_f=0;
unsigned int dist_l=0;
unsigned int dist_r=0;
unsigned int dist_45=0;
unsigned int dist_135=0;
unsigned int t=15;

void loop() 
{
  if (circle==0)
  {
    myservo.write(reMapAngle(0));
    dist_r=sonar();
    motion('w',0,45,t);
    dist_45=sonar();
    motion('w',45,90,t);
    dist_f=sonar();
    motion('w',90,135,t);
    dist_135=sonar();
    motion('w',135,180,t);
    dist_l=sonar();
  }
  circle++; 

  if(dist_f>=avoidanceDistance)
  {
a:
    motion('f',180,135,t);
    dist_135=sonar();
    front_motion(t);
    motion('f',135,90,t);  
    dist_f=sonar();
    front_motion(t);
    motion('f',90,45,t);
    dist_45=sonar();
    front_motion(t);
    motion('f',45,0,t);
    dist_r=sonar();
    front_motion(t);
    motion('f',0,45,t);
    dist_45=sonar();
    front_motion(t);
    motion('f',45,90,t);
    dist_f=sonar();
    front_motion(t);
    motion('f',90,135,t);
    dist_135=sonar();
    front_motion(t);
    motion('f',135,180,t);
    dist_l=sonar();
    front_motion(t);
    if (dist_f>=avoidanceDistance)
      goto a;
  }
  else
  {
    if(dist_f<avoidanceDistance)
    {
      motion_back(t);
      dist_f=sonar();
    } 

    if(dist_l>=dist_r || dist_135>dist_r)
    {
      motion('l',180,90,t);
      dist_f=sonar();
    }
    if(dist_l<dist_r)
    {
      motion('r',180,90,t);
      dist_f=sonar();
    }
  } 

}

int sonar()
{
  pinMode(pingPin, OUTPUT);          // Set pin to OUTPUT
  digitalWrite(pingPin, LOW);        // Ensure pin is low
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);       // Start ranging
  delayMicroseconds(5);              //   with 5 microsecond burst
  digitalWrite(pingPin, LOW);        // End ranging
  pinMode(pingPin, INPUT);           // Set pin to INPUT
  duration = pulseIn(pingPin, HIGH); // Read echo pulse
  distanceCm   = microsecondsToCentimeters(duration);
  Serial.println(distanceCm);        // Display result
  delay(200);		             // Short delay  
  return distanceCm;
}  

void forward ()
{
    Serial.println("Moving Forward");
    int turn = 0; 
    mySerial.write(CMD);                
    mySerial.write(SET_SPEED1);
    mySerial.write(-10);
    
    mySerial.write(CMD);                 
    mySerial.write(SET_SPEED2_TURN);
    mySerial.write(turn); 
}

void back ()
{
    Serial.println("Moving BackWard");
    int turn = 0;
    mySerial.write(CMD);                
    mySerial.write(SET_SPEED1);
    mySerial.write(10);
    
    mySerial.write(CMD);                 
    mySerial.write(SET_SPEED2_TURN);
    mySerial.write(turn); 
}

void right ()
{
    Serial.println("Moving Right");
    int turn = 25;
    mySerial.write(CMD);                
    mySerial.write(SET_SPEED1);
    mySerial.write(-10);
    
    mySerial.write(CMD);                 
    mySerial.write(SET_SPEED2_TURN);
    mySerial.write(turn);   
}

void left ()
{
    Serial.println("Moving Left");
    int turn = 25;
    mySerial.write(CMD);                
    mySerial.write(SET_SPEED1);
    mySerial.write(10);
    
    mySerial.write(CMD);                 
    mySerial.write(SET_SPEED2_TURN);
    mySerial.write(turn);
}

void wait()
{
  delay(50);
}

void motion (char dimention, int prev_angle, int next_angle, int time)
{
  int a;
  if(next_angle>=prev_angle)
    a=15;
  else
    a=-15; 

  if (dimention=='f')
  { 
    int i=prev_angle;
    while( i!=next_angle)
    { 
      i+=a;
      myservo.write(reMapAngle(i));
      forward();
      delay(time);
    }
  }
  if (dimention=='l')
  { 
    int i=prev_angle;
    while( i!=next_angle)
    { 
      i+=a;
      myservo.write(reMapAngle(i));
      left();
      delay(time);
    }
  }
  if (dimention=='r')
  { 
    int i=prev_angle;
    while( i!=next_angle )
    { 
      i+=a;
      myservo.write(reMapAngle(i));
      right();
      delay(time);
    }
  }
  if (dimention=='b')
  { 
    int i=prev_angle;
    while( i!=next_angle )
    { 
      i+=a;
      myservo.write(reMapAngle(i));
      back();
      delay(time);
    }
  }
  if (dimention=='w')
  {
    int i=prev_angle; 
    while( i!=next_angle )
    { 
      i+=a;
      myservo.write(reMapAngle(i));
      wait();
      delay(time);
    }
  }

}

void front_motion( int time )
{
  if(dist_45<=9)
  {
    left();
    delay(3*time);
  }
  if(dist_135<=9)
  {
    right();
    delay(3*time);
  } 
}

void motion_back( int time )
{
  motion('b',180,90,2*time);
  /*if(dist_l>=dist_r)
   {
   do
   {
   left();
   delay(time);
   dist_f=sonar();
   }while(dist_f<=20);
   } 
   
   if(dist_l<dist_r)
   {
   do
   {
   right();
   delay(time);
   dist_f=sonar();
   }while(dist_f<=20); 
   } */
} 


long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

