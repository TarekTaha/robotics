#include <Servo.h> 

const int pingPin = 11;
long duration, distanceCm;
Servo myservo;



void setup()
{
  myservo.attach(8);
  Serial.begin(9600);
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
    myservo.write(0);  //initial parameters
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

  if(dist_f>=25)
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
    if (dist_f>=25)
      goto a;
  }

  else
  {
    if(dist_f<5)
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
  Serial.println(distanceCm);            // Display result
  delay(200);		             // Short delay  
  return distanceCm;
}  


void forward ()
{

}

void back ()
{
}

void right ()
{
}

void left ()
{
}

void wait()
{
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
      myservo.write(i);
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
      myservo.write(i);
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
      myservo.write(i);
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
      myservo.write(i);
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
      myservo.write(i);
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





