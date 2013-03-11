#include <TinyGPS.h>
#include <PString.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);  

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
#define MINUTES_TO_UPDATE 0.25
unsigned long timeOut              = 30000;
unsigned long gpsUpdateRate        = MINUTES_TO_UPDATE*60000;
unsigned long pulseAliveUpdateRate = 30000;
unsigned long smsUpdateRate        = MINUTES_TO_UPDATE*60000;
unsigned long time2Reset           = 120000;
unsigned long disconnectionTime;
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

const String APN       = "purtona.net";      // access-point name for GPRS  ( TPG = yesinternet )
const String ip        = "tarektaha.com";    // IP address of server we're connecting to
const String host      = "tarektaha.com";    // required in HTTP 1.1 - what's the name of the host at this IP address?
const String useragent = "Mozilla/5.0"; // for our purposes the user agent doesn't matter - if I understand correctly it's helpful to use something generic the server will recognize
const String request   = "GET /tracker.php?x=1&y=2&z=3 HTTP/1.1";

#define POWERPIN 4
int statusLed = 13;
int errorLed  = 12;
int relayLine = 8;
char mobilenumber[]="0433292432";
bool disconnected;
const int resetPin=7;
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
/*
String getMessage() 
{
  unsigned long startTiming= millis();
  bool parsedCorrectly = false;
  //Wait for  a max of X seconds until we get something
  Serial.print("reading responce:");
  String s="";
  while( (millis()-startTiming) < timeOut )
  {
    if(cell.available()>0) 
    {
      s = s+(char)cell.read();
      if (s.length()>1 && s[s.length()-2]=='\r' && s[s.length()-1]=='\n') 
      { 
        // if last 2 chars are \r\n
        if (s==" \r\n" || s=="\r\n") 
        { 
          // skip these, move on
          s="";
        }
        else 
        { 
          // we have a message!
          Serial.println(s.substring(0,s.length()-2));
          return s.substring(0,s.length()-2);
        }
      }
    }
  }
  
}
*/

// keep reading characters until we get char c
void waitFor(char c) 
{
  while(1) 
  {
    if(cell.available()>0) 
    {
      if ((char)cell.read() == c) 
      {
        delay(100);
        return;
      }
    }
  }
}

/* Reads AT String from the SM5100B GSM/GPRS Module */
void readAndProcessATString() 
{
  char c;
  unsigned long startTiming= millis();
  bool parsedCorrectly = false;
  //Wait for  a max of 10 seconds until we get something
  Serial.print("reading responce:");
  while( (millis()-startTiming) < timeOut )
  {
    if(cell.available() > 0) 
    {
      c=cell.read();
      Serial.write(c);      
      if (c == '\r') 
      {
        continue;
      }
      else if (c == '>') 
      {
        return;
      }
      else if ((buffidx == (BUFFSIZ - 1)) || (c == '\n'))
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
    Serial.print("\nTimed out:");
    Serial.println(inputBuffer);
    inputBuffer[0]='\0'; // clear the buffer
  }
}

/*
  The +SIND response from the cellular module tells the the modules status. Here's a quick run-down of the response meanings:
  0 SIM card removed
  1 SIM card inserted
  2 Ring melody
  3 AT module is partially ready
  4 AT module is totally ready
  5 ID of released calls
  6 Released call whose ID=<idx>
  7 The network service is available for an emergency call
  8 The network is lost
  9 Audio ON
  10 Show the status of each phonebook after init phrase
  11 Registered to network
*
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
    Serial.print("Responce:"); 
    Serial.println(inputBuffer);
  }
  else if( strstr(inputBuffer, "+STCPD:1") != 0 ) 
  {
    Serial.println("Server sent data");
    Serial.print("Responce:"); 
    Serial.println(inputBuffer);
  }  
  else if( strstr(inputBuffer, "+STCPC:1") != 0 ) 
  {
    Serial.println("socket closed");
    Serial.print("Responce:"); 
    Serial.println(inputBuffer);
  }    
  else if( strstr(inputBuffer, "+SOCKSTATUS:  1,0") != 0 ) 
  {
    TCP_CONNECTION_READY = 0;
    error(ERROR_HOST);
    Serial.print("Responce:"); 
    Serial.println(inputBuffer);    
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
  char smsBuffer[BUFFSIZ];
  int charIndex=0;
  while(noResponce && (millis()-startTiming) < 5000 )
  {
    while(cell.available() >0)
    {
      inchar=cell.read();
      smsBuffer[charIndex++]=inchar;    
      noResponce = false;
      delay(10);
    }
  }
  smsBuffer[charIndex]='\0';
  if(!noResponce)
  {
    // Special Command
    if(smsBuffer[0]=='#')
    {
      Serial.print("Command SMS String Received:"); Serial.println(smsBuffer);    
      if(smsBuffer[1]=='m')
      {
        if(smsBuffer[2]=='0')
        {
          Serial.println("Switching relay off");
          digitalWrite(relayLine, LOW);          
        }
        else if( smsBuffer[2]=='1')
        {
          Serial.println("Switching relay on");  
          digitalWrite(relayLine, LOW);                  
        }
      }
      // g is for get gps position
      else if(smsBuffer[1]=='g')
      {
         myString.begin();
         myString.print("Long:");
         myString.print(flon,DEC);
         myString.print(" Lat:");
         myString.print(flat,DEC);            
         myString.print(" Lat:");
         myString.print(fix_age,DEC);                        
         sendSMS(myString);
         blinkLed(statusLed,5,200);
      }      
    }
    else
    {
      Serial.print("Normal SMS String Received:"); Serial.println(smsBuffer);    
    }    
    cell.println("AT+CMGD=1,4"); // delete all SMS
    readAndProcessATString();    
  }
}

/**
 * Send an AT command to the cell interface
 */
static void sendATCommand(const char* ATCom) 
{
  cell.println(ATCom);
  Serial.print("COMMAND: ");
  Serial.println(ATCom);
  readAndProcessATString();
  delay(RESPONCE_DELAY);
}
// receive string such as "SOCKSTATUS: 1,1,0102,10,10,0"
// 0 is connection id. 1 is whether connected or not. 2 is status (0104 is connecting, 0102 is connected, others)
// 3 is sent bytes. 4 is acknowledged bytes. 5 is "received data counter"
// THIS FUNCTION WILL check that sent bytes == ack bytes, and return that value
// return 0 if they don't match or if amount of data is 0
int checkSocketString(String s) 
{
  if (socketStringSlice(3,s) == 0)
    return 0;
  else if (socketStringSlice(3,s) == socketStringSlice(4,s))
    return socketStringSlice(3,s);
  else
    return 0;
}

// returns the index of the nth instance of char c in String s
int nthIndexOf(int n, char c, String s) 
{
  int index=0;
  for (int i=0; i<=n; i++) 
  {
    index = s.indexOf(c,index+1);
  }
  return index;
}

// expects string such as "SOCKSTATUS: 1,1,0102,10,10,0"
// returns nth chunk of data, delimited by commas
int socketStringSlice(int n, String s) 
{
  String slice = s.substring(nthIndexOf(n-1,',',s)+1,nthIndexOf(n,',',s));
  char cArray[slice.length()+1];
  slice.toCharArray(cArray, sizeof(cArray));
  return atoi(cArray);
}

// keep reading the serial messages we receive from the module
// loop forever until we get a nonzero string ending in \r\n - print and return that.
// TODO: implement a timeout that returns 0?
String getMessage() {
  String s="";
  while(1) {
    if(cell.available()>0) {
      s = s+(char)cell.read();
      if (s.length()>1 && s[s.length()-2]=='\r' && s[s.length()-1]=='\n') { // if last 2 chars are \r\n
        if (s==" \r\n" || s=="\r\n") { // skip these, move on
          s="";
        }
        else { // we have a message!
          Serial.println(s.substring(0,s.length()-2));
          return s.substring(0,s.length()-2);
        }
      }
    }
  }
}

// for eating a single message we expect from the module
// prints out the next message from the module. if it's not the expected value, die
void waitFor(String s) 
{
  String message = getMessage();
  if (message != s) 
  {
    Serial.println("Wait, that's not what we were expecting. We wanted \""+s+"\"");
  }
  delay(100); // wait for a tiny bit before sending the next command
}

static void sendData(const char* dataString) 
{
  Serial.println("-------------------- Sending TCP Data ------------------");
  digitalWrite(statusLed, HIGH);

  Serial.println("Setting up PDP Context");
  // LEBARA
  sendATCommand("AT+CGDCONT=1,\"IP\",\"purtona.net\"");

  Serial.println("Configure APN");
  sendATCommand("AT+CGPCO=0,\"\",\"\", 1");

  Serial.println("Activate PDP Context");
  sendATCommand("AT+CGACT=1,1");
  
  // Change 0.0.0.0 to reflect the server you want to connect to
  Serial.println("Configuring TCP connection to server");
  sendATCommand("AT+SDATACONF=1,\"TCP\",\"tarektaha.com\",80");

  Serial.println("Starting TCP Connection");
  sendATCommand("AT+SDATASTART=1,1");
  delay(5000);
  
  unsigned long startingTime=millis();
  while(!TCP_CONNECTION_READY && (millis()-startingTime) < timeOut)
  {
   sendATCommand("AT+SDATASTATUS=1");
   delay(1000);
  }  
  /*
  char str [] = "GET /test.php?x=1&y=2&z=3 HTTP/1.0\r\n\r\n";   
  Serial.println("Sending data");
  cell.print("AT+SDATATSEND=1,3\r");
  delay(500);
  //cell.write("GET");
  cell.print(47,HEX);
  cell.print(45,HEX);
  cell.print(54,HEX);
  //cell.print(13); //Send CR
  //cell.print(10); //Send el LF
  delay(3000);        
  byte ctrlZ = 26;
  cell.write(ctrlZ);
  Serial.println("Data Sent");
  //sendATCommand(dataString);
  */
  Serial.println("Sending HTTP packet...");
  // we're now connected and can send HTTP packets!
  int packetLength = 26+host.length()+request.length()+useragent.length(); // 26 is size of the non-variable parts of the packet, see SIZE comments below  
  cell.print("AT+SDATATSEND=1,"+String(packetLength)+"\r");
  waitFor('>'); // wait for GSM module to tell us it's ready to recieve the packet
  cell.print(request+"\r\n"); // SIZE: 2
  cell.print("Host: "+host+"\r\n"); // SIZE: 8
  cell.print("User-Agent: "+useragent+"\r\n\r\n"); // SIZE: 16
  cell.write(26); // ctrl+z character: send the packet
  readAndProcessATString();
  
  blinkLed(statusLed,5,200);   

  // now we're going to keep checking the socket status forever
  // break when the server tells us it acknowledged data
  bool success = false;
  while (1) 
  {
    cell.println("AT+SDATASTATUS=1"); // we'll get back SOCKSTATUS and then OK
    String s = getMessage(); // we want s to contain the SOCKSTATUS message
    if (s == "+STCPD:1") // this means server sent data. cool, but we want to see SOCKSTATUS, so let's get next message
      s = getMessage();
    if (s == "+STCPC:1") // this means socket closed. cool, but we want to see SOCKSTATUS, so let's get next message
      s = getMessage();
    waitFor("OK");
    
    if (!s.startsWith("+SOCKSTATUS")) 
    {
      Serial.println("Wait, this isn't the SOCKSTATUS message!");
      success  = false;
      break;
    }
    if (checkSocketString(s) == packetLength) // checks that packetLength bytes have been acknowledged by server
    {
      success = true;
      break; // we're good!
    }
    else 
    {
      Serial.println("Sent data not yet acknowledged by server, waiting 1 second and checking again.");
      delay(1000);
    }
  }
  if(success)
    Serial.println("Yes! Sent data acknowledged by server!");
  /*
  Serial.println("Getting Send status");
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

void resetUnit()
{
  Serial.println("Resetting Unit");
  softReset();
  digitalWrite(7, LOW); //Pulling the RESET pin LOW triggers the reset.
}

void registerGRPS()
{
  while (GPRS_registered == 0 || GPRS_AT_ready == 0) 
  {
    //Serial.println("-------------------- Checking for Module Responce------------------");
    //sendATCommand("AT+SBAND=4");
    readAndProcessATString();
    if((millis()-rebootTime)>time2Reset)
    {
      rebootTime = millis();
      Serial.println("Resetting unit");
      lcd.print("Resetting unit");
      resetUnit();
    }
  }
  disconnected = false;
  pinMode(13, OUTPUT);
  Serial.println("Cell Module Initialized");
  Serial.println("Attaching GPRS...");
  cell.println("AT+CGATT=1");
  readAndProcessATString();
  delay(RESPONCE_DELAY);  
}

// Do system wide initialization here in this function
void setup()
{
  lcd.init();                      // initialize the lcd 
   // Print a message to the LCD.
  lcd.backlight();  
 
  digitalWrite(resetPin, HIGH); //We need to set it HIGH immediately on boot
  pinMode(resetPin,OUTPUT);     //We can declare it an output ONLY AFTER it's HIGH  
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
    checkForSMS();    
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
          Serial.print("Lat:"); 
          Serial.print(flat,DEC); 
          Serial.print(" Lng:"); 
          Serial.println(flon,DEC);       
          myString.begin();   
//          myString.print("AT+SSTRSEND=1,\"");
//          myString.print(speed);
//          myString.print(",");
//          myString.print(flat, DEC);
//          myString.print(",");
//          myString.print(flon, DEC);
//          myString.print(",");
//          myString.print(TinyGPS::cardinal(gps.f_course()));
//          myString.print("\"");
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
  // Check if we got disconnected
  if(GPRS_registered == 0 || GPRS_AT_ready == 0) 
  {
    if(!disconnected)
    {
      disconnectionTime = millis();
      disconnected = true;
    }
    else
    {
      if((millis()-disconnectionTime)>time2Reset)
      {
        resetUnit();
      }
    }
  }
  blinkLed(statusLed,1,50);
}





