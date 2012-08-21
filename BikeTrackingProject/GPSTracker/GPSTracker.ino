#include <TinyGPS.h>
#include <PString.h>

/*****************************************
* GSM SM5100b-d Variables
*****************************************/
#define GPSRATE 57600
#define RESPONCE_DELAY 500
#define cell Serial1  
/*****************************************/

/*****************************************
* Error codes
*****************************************/
const int     ERROR_GPS_UNAVAIL = 0;
const int     ERROR_GPS_STALE = 1;
const int     ERROR_SIM_UNAVAIL = 2;
const int     ERROR_GPRS_FAIL = 3;
const int     ERROR_NETWORK_FAIL = 4;
const int     ERROR_HOST = 5;
const int     ERROR_GPRS_UNKNOWN = 6;
const int     ERROR_GSM_FAIL = 7;
bool          TCP_CONNECTION_READY = false;
const boolean DEBUG = true;
/*****************************************/

/*****************************************
* Update rate Variables
*****************************************/ 
#define MINUTES_TO_UPDATE 0.5
unsigned long timeOut              = 10000;
unsigned long gpsUpdateRate        = MINUTES_TO_UPDATE*60000;
unsigned long pulseAliveUpdateRate = 30000;
unsigned long smsUpdateRate        = MINUTES_TO_UPDATE*60000;
unsigned long time2Reset           = 120000;
unsigned long rebootTime;
unsigned long lastGPSUpdate;
unsigned long lastPulseUpdate;
unsigned long lastSMSCheck ;
/****************************************/

/*****************************************
* GPS Variables
*****************************************/ 
int GPRS_registered = 0;
int GPRS_AT_ready = 0;
float flat, flon,speed;
unsigned long fix_age;
TinyGPS gps;
#define gpsSerial Serial2
/****************************************/

/*****************************************
* String Variables
*****************************************/ 
#define BUFFSIZ 512
char    inchar=0;
char    buffer[BUFFSIZ];
PString myString(buffer,sizeof(buffer));
char    inputBuffer[BUFFSIZ]; 
char    buffidx=0;
/****************************************/

#define POWERPIN 4
int statusLed = 13;
int errorLed  = 12;
int relayLine = 8;
char mobilenumber[]="0433292432";

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
}

/**
* Flash an LED X amount of times and write to console based on the error code given
*/
static void error(const int errorCode) 
{
  int flashTimes = 0;
  int i = 0;
  boolean severity = true;
  
  switch(errorCode) 
  {
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
      Serial.println(inputBuffer);
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
        inputBuffer[buffidx] = '\0';
        buffidx = 0;
        parsedCorrectly = true;
        break;
      }
      inputBuffer[buffidx++]= c;
    }
  }
  if(parsedCorrectly)
  {
    processATString();
  }
  else
  {
    Serial.print("Timed out:");
    Serial.println(inputBuffer);
    inputBuffer[0]='\0'; // clear the buffer
  }
}

/* Processes the AT String to determine if GPRS is registered and AT is ready */
void processATString() 
{
  if( strcmp(inputBuffer, "+SIND: 1") == 0 ) 
  {
    Serial.println("SIM card inserted");
    blinkLed(statusLed,5,100);
  }  
  else if( strstr(inputBuffer, "+SIND: 3") != 0 ) 
  {
    Serial.println("AT Module is partially ready");
    blinkLed(statusLed,5,100);
  }
  else if( strstr(inputBuffer, "+SIND: 4") != 0 ) 
  {
    GPRS_AT_ready=1;
    Serial.println("AT Module is totally ready");
    blinkLed(statusLed,5,100);
  }
  else if( strstr(inputBuffer, "+SIND: 7") != 0 ) 
  {
    Serial.println("Service is available for Emergency call only");
  }  
  else if( strstr(inputBuffer, "+SIND: 8") != 0 ) 
  {
    GPRS_registered = 0;
    Serial.println("GPRS Network Not Available");
  }
  else if( strstr(inputBuffer, "+SIND: 11") != 0 ) 
  {
    GPRS_registered = 1;
    Serial.println("GPRS Registered");
    blinkLed(statusLed,5,100);
  }  
  else if( strstr(inputBuffer, "+SOCKSTATUS:  1,1") != 0 ) 
  {
    TCP_CONNECTION_READY = 1;
    Serial.println("TCP Connection Ready");
    Serial.print("Responce:"); Serial.println(inputBuffer);
  }  
  else if( strstr(inputBuffer, "+SOCKSTATUS:  1,0") != 0 ) 
  {
    TCP_CONNECTION_READY = 0;
    error(ERROR_HOST);
    Serial.print("Responce:"); Serial.println(inputBuffer);    
  }    
  else if (strstr(inputBuffer, "+SIND: 10,\"SM\",0,\"FD\",0,\"LD\",0,\"MC\",0,\"RC\",0,\"ME\",0") != 0 || strstr(inputBuffer, "+SIND: 0") != 0) 
  {
    error(ERROR_SIM_UNAVAIL);
  }  
  else if ( (strstr(inputBuffer, "+SIND: 10,\"SM\",1,\"FD\",1,\"LD\",1,\"MC\",1,\"RC\",1,\"ME\",1") != 0) || (strstr(inputBuffer, "+SIND: 10,\"SM\",1,\"FD\",1,\"LD\",1,\"MC\",1,\"RC\",1,\"ME") != 0)) 
  {
    Serial.println("SIM card Detected");
    successLED();
  }
  else if (strstr(inputBuffer, "+CME ERROR: 29") != 0) 
  {
    return;
  }
  else if (strstr(inputBuffer, "+CME ERROR") != 0) 
  {
    error(ERROR_GPRS_UNKNOWN);
  }
  else if (strstr(inputBuffer, "OK") != 0 || strstr(inputBuffer, "NO CARRIER") != 0) 
  {
    successLED();
  }
  else
  {
    Serial.print("Responce:");
    Serial.println(inputBuffer);
  }
}
void (*softReset) (void) = 0; //declare reset function @ address 0

void checkForSMS()
{
  cell.println("at+cmgl=\"ALL\"");
  readAndProcessATString();
  delay(100);
  bool noResponce = true;
  unsigned long startTiming = millis();
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
  while(noResponce && ((millis()-startTiming) < timeOut))
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

static void sendData(const char* dataString) 
{
  Serial.println("-------------------- Sending TCP Data ------------------");
  digitalWrite(statusLed, HIGH);

  Serial.println("Setting up PDP Context");
  sendATCommand("AT+CGDCONT=1,\"IP\",\"purtona.net\"");
   
  Serial.println("Configure APN");
  sendATCommand("AT+CGPCO=0,\"\",\"\", 1");
  
  Serial.println("Activate PDP Context");
  sendATCommand("AT+CGACT=1,1");
  
  // Change 0.0.0.0 to reflect the server you want to connect to
  Serial.println("Configuring TCP connection to server");
  sendATCommand("AT+SDATACONF=1,\"TCP\",\"yaz.dyndns.org\",80");
    
  Serial.println("Starting TCP Connection");
  sendATCommand("AT+SDATASTART=1,1");
  delay(1000);
  /*
  unsigned long startingTime=millis();
  while(!TCP_CONNECTION_READY && (millis()-startingTime) < timeOut)
  {
    sendATCommand("AT+SDATASTATUS=1");
    delay(1000);
  }  
 */
  Serial.println("Sending data");
  sendATCommand(dataString);

  blinkLed(statusLed,5,200);   
  /*
  Serial.println("Getting status");
  sendATCommand("AT+SDATASTATUS=1");
  */
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

void registerGRPS()
{
  while (GPRS_registered == 0 || GPRS_AT_ready == 0) 
  {
    Serial.println("-------------------- Checking for Module Responce------------------");
    readAndProcessATString();
    if((millis()-rebootTime)>time2Reset)
    {
      Serial.println("Resetting unit");
      softReset();
    }
  }
  pinMode(13, OUTPUT);
  Serial.println("Cell Module Initialized");
  delay(RESPONCE_DELAY);  
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
  gpsSerial.begin(GPSRATE);
  //Let's get started!
  Serial.println("Starting SM5100B Communication...");
  rebootTime = millis();
  delay(5000);
  /* Currently GPRS is not registered and AT is not ready */
  GPRS_registered = 0;
  GPRS_AT_ready = 0;
  TCP_CONNECTION_READY = 0;
  lastGPSUpdate   = millis();
  lastPulseUpdate = millis();
  lastSMSCheck    = millis();
  Serial.println("-------------------- Registering GPRS ------------------");
  registerGRPS();
  delay(1000);
  initializeRecievingSMS();  
}
void loop() 
{
  if((millis() - lastSMSCheck)>smsUpdateRate)
  {
    Serial.println("-------------------- Checking for SMS ------------------");
    //checkForSMS();    
    lastSMSCheck = millis();
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
          Serial.print("Lat:"); Serial.print(flat,DEC); Serial.print(" Lng:"); Serial.println(flon,DEC);       
          myString.begin();   
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
          //sendSMS(myString);
          blinkLed(statusLed,5,200);
          lastGPSUpdate = millis();
          sendData(myString);
        }
      }
    }
  }
  if( (millis() - lastPulseUpdate)>  pulseAliveUpdateRate)
  {
    Serial.println("-------------------- Sending Pulse ------------------");
    myString.begin();
    myString.print("AT+SSTRSEND=1,\"");
    myString.print("Still Alive.");
    myString.print("\"");
    lastPulseUpdate = millis();
    sendData(myString);    
  }
  blinkLed(statusLed,1,50);
}




