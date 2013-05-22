 If this data was useful, you can support me and my projects. Here is my WebMoney purse: Z205751064101 Thanks!

 #include <Servo.h> 
 
  const int motor1APin = 11;   // H-bridge leg 1 
  const int motor2APin = 6;    // H-bridge leg 2 
  const int motor3APin = 10;   // H-bridge leg 1 
  const int motor4APin = 5;    // H-bridge leg 2 
  const int enablePin = 12;   // H-bridge enable pin
 
  const int Trig = 3; //Ultrasonic
  const int Echo = 2;
  
  Servo myservo;



  void setup()
  {
    // set all the other pins you're using as outputs:
    pinMode( enablePin, OUTPUT);
    pinMode( motor1APin, OUTPUT);
    pinMode( motor2APin, OUTPUT);
    pinMode( motor3APin, OUTPUT);
    pinMode( motor4APin, OUTPUT);
    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
  
    // set enablePin high so that motor can turn on:
    digitalWrite(enablePin, HIGH);
    
    myservo.attach(8);
    
  }
  
  unsigned int time_us=0;
  unsigned int distance_sm=0;
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
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);
    time_us=pulseIn(Echo, HIGH);
    distance_sm=time_us/58;
    return distance_sm;
  }  
     
    
  void forward ()
  {
    digitalWrite(motor1APin, HIGH);
    digitalWrite(motor2APin, LOW);
    digitalWrite(motor3APin, HIGH);
    digitalWrite(motor4APin, LOW);
  }
  
   void back ()
  {
    digitalWrite(motor1APin, LOW);
    digitalWrite(motor2APin, HIGH);
    digitalWrite(motor3APin, LOW);
    digitalWrite(motor4APin, HIGH);
  }
  
   void right ()
  {
    digitalWrite(motor1APin, LOW);
    digitalWrite(motor2APin, HIGH);
    digitalWrite(motor3APin, HIGH);
    digitalWrite(motor4APin, LOW);
  }
  
   void left ()
  {
    digitalWrite(motor1APin, HIGH);
    digitalWrite(motor2APin, LOW);
    digitalWrite(motor3APin, LOW);
    digitalWrite(motor4APin, HIGH);
  }
  
  void wait()
 {
  digitalWrite(motor1APin, LOW); 
  digitalWrite(motor2APin, LOW); 
  digitalWrite(motor3APin, LOW); 
  digitalWrite(motor4APin, LOW); 
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

 

  
      


