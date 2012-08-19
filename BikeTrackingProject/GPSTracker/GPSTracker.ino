#include <TinyGPS.h>
#include <PString.h>

#define POWERPIN 4
#define GPSRATE 4800
#define BUFFSIZ 512
#define RESPONCE_DELAY 500
#define MINUTES_TO_UPDATE 2
/*****************************************
* Error codes
*/
const int ERROR_GPS_UNAVAIL = 0;
const int ERROR_GPS_STALE = 1;
const int ERROR_SIM_UNAVAIL = 2;
const int ERROR_GPRS_FAIL = 3;
const int ERROR_NETWORK_FAIL = 4;
const int ERROR_HOST = 5;
const int ERROR_GPRS_UNKNOWN = 6;
const int ERROR_GSM_FAIL = 7;
const boolean DEBUG = true;
/*****************************************/

int relayLine = 8;

char at_buffer[BUFFSIZ]; 
char buffidx=0;
bool TCP_CONNECTION_READY = false;
unsigned long lastTCPSend = millis();
char mobilenumber[]="0433292432";
//How long to wait to get the AT responce
unsigned long timeOut = 10000, gpsUpdateRate = MINUTES_TO_UPDATE*60000;
unsigned long lastGPSUpdate;
int firstTimeInLoop = 1;
int GPRS_registered = 0;
int GPRS_AT_ready = 0;
float flat, flon,speed;
unsigned long fix_age;

const int PRECISION = 11; // Precision of lat/long coordinates
const int DIAG_DELAY = 2000; // time to pause before replaying diagnostic codes
const int SEND_DELAY = 10000; // The time to wait before sending the next coordinates

//Will hold the incoming character from the Serial Port.
char inchar=0;
char buffer[BUFFSIZ];

PString myString(buffer,sizeof(buffer));
//Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.
#define cell Serial1  
#define gpsSerial Serial2
TinyGPS gps;

int statusLed = 13;
int errorLed  = 12;
/**
* Blink a few times so we know all is right with the world
*/
static void successLED() 
{
  blinkLed(statusLed, 5, 100);
}

/**
* Blink LED
*/
static void blinkLed(int lPin, int flashTimes, int dly) 
{
  for (int i = 0; i < flashTimes; i++) 
  {
    digitalWrite(lPin, HIGH);
    delay(dly);
    digitalWrite(lPin, LOW);
    delay(dly);
  }
  delay(DIAG_DELAY);
}

/**
* Flash an LED X amount of times and write to console based on the error code given
*/
static void error(const int errorCode) {
  int flashTimes = 0;
  int i = 0;
  boolean severity = true;
  
  switch(errorCode) {
    case ERROR_GPS_UNAVAIL:
      Serial.println("ERROR: GPS Unavailable");
      severity = false;
      break;
    case ERROR_SIM_UNAVAIL:
      flashTimes = 3;
      Serial.println("ERROR: SIM Unavailable");
      break;
    case ERROR_GPRS_FAIL:
      flashTimes = 4;
      Serial.println("ERROR: Connection to network failed");
      break;
    case ERROR_NETWORK_FAIL:
      flashTimes = 5;
      Serial.println("ERROR: Could not connect to network");
      break;
    case ERROR_HOST:
      flashTimes = 6;
      Serial.println("ERROR: Could not connect to host");
      break;
    case ERROR_GPRS_UNKNOWN:
      flashTimes = 7;
      Serial.print("ERROR: Unknown:");
      Serial.println(at_buffer);
      break;
    case ERROR_GSM_FAIL:
      flashTimes = 8;
      Serial.println("ERROR: GSM Timeout");
      break;
  }
  
  digitalWrite(statusLed, LOW);

  // Setup an infinite loop to require reset if something goes wrong
  /*
  while (severity) {
    blinkLed(errorLed, flashTimes, 500);
    delay(1000);
  }
  */
}

/* Reads AT String from the SM5100B GSM/GPRS Module */
void readAndProcessATString() 
{
  char c;
  unsigned long startTiming= millis();
  bool parsedCorrectly = false;
  //Wait for  a max of 10 seconds until we get something
  while( (millis()-startTiming) < timeOut )
  {
    if(cell.available() > 0) 
    {
      c=cell.read();
      //Serial.write(c);      
      if (c == '\r') 
      {
        continue;
      }
      if ((buffidx == (BUFFSIZ - 1)) || (c == '\n'))
      {
        if(buffidx==0)
          continue;
        at_buffer[buffidx] = '\0';
        buffidx = 0;
        parsedCorrectly = true;
        break;
      }
      at_buffer[buffidx++]= c;
    }
  }
  if(parsedCorrectly)
  {
    processATString();
  }
  else
  {
    Serial.print("Timed out:");
    Serial.println(at_buffer);
    at_buffer[0]='\0'; // clear the buffer
  }
}

/* Processes the AT String to determine if GPRS is registered and AT is ready */
void processATString() 
{
  if( strcmp(at_buffer, "+SIND: 1") == 0 ) 
  {
    Serial.println("SIM card inserted");
    blinkLed(statusLed,5,100);
  }  
  else if( strstr(at_buffer, "+SIND: 3") != 0 ) 
  {
    Serial.println("AT Module is partially ready");
    blinkLed(statusLed,5,100);
  }
  else if( strstr(at_buffer, "+SIND: 4") != 0 ) 
  {
    GPRS_AT_ready=1;
    Serial.println("AT Module is totally ready");
    blinkLed(statusLed,5,100);
  }
  else if( strstr(at_buffer, "+SIND: 7") != 0 ) 
  {
    Serial.println("Service is available for Emergency call only");
  }  
  else if( strstr(at_buffer, "+SIND: 8") != 0 ) 
  {
    GPRS_registered = 0;
    Serial.println("GPRS Network Not Available");
  }
  else if( strstr(at_buffer, "+SIND: 11") != 0 ) 
  {
    GPRS_registered = 1;
    Serial.println("GPRS Registered");
    blinkLed(statusLed,5,100);
  }  
  else if( strstr(at_buffer, "+SOCKSTATUS:  1,1") != 0 ) 
  {
    TCP_CONNECTION_READY = 1;
    Serial.println("TCP Connection Ready");
    Serial.print("Responce:"); Serial.println(at_buffer);
  }  
  else if( strstr(at_buffer, "+SOCKSTATUS:  1,0") != 0 ) 
  {
    TCP_CONNECTION_READY = 0;
    error(ERROR_HOST);
    Serial.print("Responce:"); Serial.println(at_buffer);    
  }    
  else if (strstr(at_buffer, "+SIND: 10,\"SM\",0,\"FD\",0,\"LD\",0,\"MC\",0,\"RC\",0,\"ME\",0") != 0 || strstr(at_buffer, "+SIND: 0") != 0) 
  {
    error(ERROR_SIM_UNAVAIL);
  }  
  else if ( (strstr(at_buffer, "+SIND: 10,\"SM\",1,\"FD\",1,\"LD\",1,\"MC\",1,\"RC\",1,\"ME\",1") != 0) || (strstr(at_buffer, "+SIND: 10,\"SM\",1,\"FD\",1,\"LD\",1,\"MC\",1,\"RC\",1,\"ME") != 0)) 
  {
    Serial.println("SIM card Detected");
    successLED();
  }
  else if (strstr(at_buffer, "+CME ERROR: 29") != 0) 
  {
    return;
  }
  else if (strstr(at_buffer, "+CME ERROR") != 0) 
  {
    error(ERROR_GPRS_UNKNOWN);
  }
  else if (strstr(at_buffer, "OK") != 0 || strstr(at_buffer, "NO CARRIER") != 0) 
  {
    successLED();
  }
  else
  {
    Serial.print("Responce:");
    Serial.println(at_buffer);
  }
}

void checkForSMS()
{
  cell.println("at+cmgl=\"ALL\"");
  readAndProcessATString();
  delay(100);
  bool noResponce = true;
  /*
  while(noResponce)
   {
   while(cell.available() >0)
   {
   inchar=cell.read();
   delay(10);
   Serial.print(inchar);    
   if (inchar=='#')
   {
   delay(10);
   inchar=cell.read();
   Serial.print(inchar);   
   if (inchar=='m')
   {
   delay(10);
   inchar=cell.read();
   Serial.print(inchar); 
   if (inchar=='0')
   {
   digitalWrite(relayLine, LOW);
   } 
   else if (inchar=='1')
   {
   digitalWrite(relayLine, HIGH);
   }
   delay(10);
   }
   }
   noResponce = false;
   }
   }
   */

  //If a character comes in from the cellular module...
  while(noResponce)
  {  
    if(cell.available() >0)
    {
      inchar=cell.read();    
      if (inchar=='#')
      {
        delay(10);
        inchar=cell.read();  
        if (inchar=='m')
        {
          delay(10);
          inchar=cell.read();
          if (inchar=='0')
          {
            digitalWrite(relayLine, LOW);
          } 
          else if (inchar=='1')
          {
            digitalWrite(relayLine, HIGH);
          }
          delay(10);
          Serial.println("Recieved Command SMS");
          cell.println("AT+CMGD=1,4"); // delete all SMS
          readAndProcessATString();
        }
        if (inchar=='g')
        {
          delay(10);
          myString.begin();
          myString.print("Long:");
          myString.print(flon,DEC);
          myString.print(" Lat:");
          myString.print(flat,DEC);            
          myString.print(" Lat:");
          myString.print(fix_age,DEC);                        
          sendSMS(myString);
          blinkLed(statusLed,5,200);
          cell.println("AT+CMGD=1,4"); // delete all SMS
          readAndProcessATString();
        }      
      }
      else
      {
        while((inchar=cell.read())!=-1)
        {
          Serial.print(inchar);
        }
        Serial.println("Recieved Normal SMS");      
        cell.println("AT+CMGD=1,4"); // delete all SMS
        readAndProcessATString();
      }      
    }  
    noResponce = false;
  }
}

/**
* Send an AT command to the cell interface
*/
static void sendATCommand(const char* ATCom) 
{
  cell.println(ATCom);
  Serial.print("COMMAND: ");Serial.println(ATCom);
  readAndProcessATString();
  delay(RESPONCE_DELAY);
}

void registerGRPS()
{
  while (GPRS_registered == 0 || GPRS_AT_ready == 0) 
  {
    readAndProcessATString();
  }
  if(POWERPIN) 
  {
    pinMode(POWERPIN, OUTPUT);
  }
  pinMode(13, OUTPUT);
  Serial.println("Cell Module Initialized");
  digitalWrite(POWERPIN, LOW);
  delay(RESPONCE_DELAY);  
}
/*
static void sendData2(const char* data) 
{
  digitalWrite(statusLed, HIGH);

  Serial.println("Setting up PDP Context");
  sendATCommand("AT+CGDCONT=1,\"IP\",\"purtona.net\"");
    
  Serial.println("Configure APN");
  sendATCommand("AT+CGPCO=0,\"\",\"\", 1");
  
  Serial.println("Activate PDP Context");
  sendATCommand("AT+CGACT=1,1");
  
  // Change 0.0.0.0 to reflect the server you want to connect to
  Serial.println("Configuring TCP connection to server");
  sendATCommand("AT+SDATACONF=1,\"TCP\",\"blackcoder.dyndns.org\",8081");
    
  Serial.println("Starting TCP Connection");
  sendATCommand("AT+SDATASTART=1,1");
  
  delay(1000);
  
  //Serial.println("Getting status");
  //sendATCommand("AT+SDATASTATUS=1");
  
  Serial.println("Sending data");
  sendATCommand(data);
  
  //Serial.println("Getting status");
  //sendATCommand("AT+SDATASTATUS=1");
  
  //Serial.println("Reading responce");
  //sendATCommand("AT+SDATAREAD=1");

  Serial.println("Close connection");
  sendATCommand("AT+SDATASTART=1,0");
  
  Serial.println("Disable PDP Context");
  sendATCommand("AT+CGACT=0,1");
  
  // Clear string and flash LED
  myString.begin();
  successLED();
  
  digitalWrite(statusLed, LOW);
}
*/
static void sendData(const char* dataString) 
{
  digitalWrite(statusLed, HIGH);

  Serial.println("Setting up PDP Context");
  sendATCommand("AT+CGDCONT=1,\"IP\",\"purtona.net\"");
   
  Serial.println("Configure APN");
  sendATCommand("AT+CGPCO=0,\"\",\"\", 1");
  
  Serial.println("Activate PDP Context");
  sendATCommand("AT+CGACT=1,1");
  
  // Change 0.0.0.0 to reflect the server you want to connect to
  Serial.println("Configuring TCP connection to server");
  sendATCommand("AT+SDATACONF=1,\"TCP\",\"blackcoder.dyndns.org\",8081");
    
  Serial.println("Starting TCP Connection");
  sendATCommand("AT+SDATASTART=1,1");
  delay(1000);
  unsigned long startingTime=millis();
  while(!TCP_CONNECTION_READY && (millis()-startingTime) < timeOut)
  {
    sendATCommand("AT+SDATASTATUS=1");
    delay(1000);
  }  
 
  Serial.println("Sending data");
  sendATCommand(myString);

  blinkLed(statusLed,5,200);   
   
  Serial.println("Getting status");
  sendATCommand("AT+SDATASTATUS=1");
  
  Serial.println("Close connection");
  sendATCommand("AT+SDATASTART=1,0");
  
  TCP_CONNECTION_READY = false;
  
  Serial.println("Disable PDP Context");
  sendATCommand("AT+CGACT=0,1");
  
  // Clear string and flash LED
  myString.begin();
  successLED();
  
  digitalWrite(statusLed, LOW);
}

// function to send a text message
void sendSMS(const char smsMessage[])
{
  sendATCommand("AT+CMGF=1");  // set SMS mode to text
  cell.print("AT+CMGS=");
  cell.print('"');             // ASCII equivalent of "
  cell.print(mobilenumber);
  cell.println('"');           // ASCII equivalent of "
  delay(RESPONCE_DELAY);       // give the module some thinking time
  cell.println(smsMessage);
  cell.write(26);              // ASCII equivalent of Ctrl-Z
  cell.println("");
  processATString();           // the SMS module needs time to return to OK status
  Serial.println("SMS Sent");
}

void initializeRecievingSMS()
{
  // set SMS mode to text
  sendATCommand("AT+CMGF=1"); 
  // set module to send SMS data to serial out upon receipt 
  sendATCommand("AT+CNMI=3,3,0,0"); 
}

// Do system wide initialization here in this function
void setup()
{
  // prepare the digital output pins
  pinMode(relayLine, OUTPUT);
  digitalWrite(relayLine, LOW);  
  // LED Pin are outputs. Switch the mode 
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  /* Blink the Power LED */
  blinkLed(statusLed,3,500);
  //Initialize serial ports for communication.
  Serial.begin(9600);
  cell.begin(9600);
  gpsSerial.begin(57600);
  //Let's get started!
  Serial.println("Starting SM5100B Communication...");
  delay(5000);
  /* Currently GPRS is not registered and AT is not ready */
  GPRS_registered = 0;
  GPRS_AT_ready = 0;
  TCP_CONNECTION_READY = 0;
  lastGPSUpdate = millis();
  registerGRPS();
  //sendSMS("Cell Module registered and ready.");
  delay(5000);
  initializeRecievingSMS();  
}
unsigned long timeSinceLastSMSCheck = millis();
void loop() 
{
  if((millis() - timeSinceLastSMSCheck)>30000)
  {
    Serial.println("Checking for SMS");
    //checkForSMS();    
    timeSinceLastSMSCheck = millis();
    /*
    myString.begin();
    myString.print("AT+SSTRSEND=1,\"");
    myString.print("speed=10&lat=20&lng=30&course=40 HTTP/1.0");
    //myString.print("AT+SSTRSEND=1,\"GET /e107_svn/test.php?speed=10&lat=20&lng=30&course=40 HTTP/1.0");
    //myString.print("AT+SSTRSEND=1,\"/e107_svn/test.php?speed=10&lat=20&lng=30&course=40");
    //myString.print("AT+SSTRSEND=1,\"");
    myString.print("\"");
    sendData(myString);
    */
   }
  while(gpsSerial.available()) 
  {
    int c = gpsSerial.read();
    if (gps.encode(c)) 
    {
      Serial.print("GPS data encoded.");
      blinkLed(statusLed,1,50);
      gps.f_get_position(&flat,&flon,&fix_age);
      speed = gps.f_speed_mps();
      if (flat == TinyGPS::GPS_INVALID_F_ANGLE) 
      {
        error(ERROR_GPS_UNAVAIL);
        Serial.println("No fix detected");
      }      
      else if(fix_age == TinyGPS::GPS_INVALID_AGE)
      {
        error(ERROR_GPS_UNAVAIL);
        Serial.println("No fix detected");
      }
      else if (fix_age > 5000)
        Serial.println("WARNING: Possible Stale Data!");
      else 
      {
        if((millis() - lastGPSUpdate)> gpsUpdateRate)
        {
          myString.print("AT+SSTRSEND=1,\"");
          myString.print(speed);
          myString.print(",");
          myString.print(flat, DEC);
          myString.print(",");
          myString.print(flon, DEC);
          myString.print(",");
          myString.print(TinyGPS::cardinal(gps.f_course()));
          myString.print("\"");
          //myString.print(fix_age,DEC);                        
          sendSMS(myString);
          blinkLed(statusLed,5,200);
          lastGPSUpdate = millis();
          sendData(myString);
        }
      }
    }
  }
  blinkLed(statusLed,1,50);
}




