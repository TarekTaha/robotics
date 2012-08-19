#include <NewSoftSerial.h>
#include <TinyGPS.h>

/* 
 This sample code demonstrates the normal use of a TinyGPS object.
 It requires the use of NewSoftSerial, and assumes that you have a
 4800-baud serial GPS device hooked up on pins 2(rx) and 3(tx).
*/

TinyGPS gps;
NewSoftSerial gpsSerial(4,5);
char inchar;                //Will hold the incoming character from the Serial Port.
NewSoftSerial cell(2,3);    //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.

int relayLine = 8;
int pulseLedPin = 13;
bool pulseLedStatus = false;
void gpsdump(TinyGPS &gps);
bool feedgps();
void printFloat(double f, int digits = 2);
// Top left     Lat :33°52'41.59"S Long:151° 6'5.22"E
// Bottom right Lat :33°52'43.65"S Long:151° 6'8.53"E
void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(4800);
  //Initialize GSM module serial port for communication.
  Serial.println("Setting up Cell module"); 
  cell.begin(9600);
  Serial.println("Waiting for the module to initialize ...");   
  delay(30000); // give time for GSM module to register on network etc.
  cell.println("AT+CMGF=1"); // set SMS mode to text
  delay(200);
  // set module to send SMS data to serial out upon receipt 
  cell.println("AT+CNMI=3,3,0,0"); 
  delay(200);
  Serial.println("Cell Module Initialized");       
  // prepare the digital output pins
  pinMode(relayLine, OUTPUT);
  pinMode(pulseLedPin, OUTPUT);
  digitalWrite(relayLine, LOW);
  digitalWrite(pulseLedPin, LOW);
  Serial.print("Testing TinyGPS library v. "); 
  Serial.println(TinyGPS::library_version());
  Serial.println();
  Serial.print("Sizeof(gpsobject) = "); 
  Serial.println(sizeof(TinyGPS));
  Serial.println();
}

void loop()
{
  bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update
  while (millis() - start < 5000)
  {
    if (feedgps())
      newdata = true;
    // While waiting Serve SMS
    //If a character comes in from the cellular module...
    if(cell.available() >0)
    {
      Serial.println("Recieved SMS");
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
          cell.println("AT+CMGD=1,4"); // delete all SMS
        }
      }
      else
      {
        do
        {
          inchar=cell.read();
          Serial.print(inchar);
        }while(inchar!=-1);
        Serial.println("");
      }      
    }
    if (feedgps())
      newdata = true;
  }
  pulseLedStatus = !pulseLedStatus;
  digitalWrite(pulseLedPin, pulseLedStatus);
  if (newdata)
  {
    Serial.println("Acquired Data");
    Serial.println("-------------");
    gpsdump(gps);
    Serial.println("-------------");
    Serial.println();
  }

}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
    Serial.print('-');
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint; 
  } 
}

void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;

  gps.get_position(&lat, &lon, &age);
  Serial.print("Lat/Long(10^-5 deg): "); 
  Serial.print(lat); 
  Serial.print(", "); 
  Serial.print(lon); 
  Serial.print(" Fix age: "); 
  Serial.print(age); 
  Serial.println("ms.");

  feedgps(); // If we don't feed the gps during this long routine, we may drop characters and get checksum errors

    gps.f_get_position(&flat, &flon, &age);
  Serial.print("Lat/Long(float): "); 
  printFloat(flat, 5); 
  Serial.print(", "); 
  printFloat(flon, 5);
  Serial.print(" Fix age: "); 
  Serial.print(age); 
  Serial.println("ms.");

  feedgps();

  gps.get_datetime(&date, &time, &age);
  Serial.print("Date(ddmmyy): "); 
  Serial.print(date); 
  Serial.print(" Time(hhmmsscc): "); 
  Serial.print(time);
  Serial.print(" Fix age: "); 
  Serial.print(age); 
  Serial.println("ms.");

  feedgps();

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  Serial.print("Date: "); 
  Serial.print(static_cast<int>(month)); 
  Serial.print("/"); 
  Serial.print(static_cast<int>(day)); 
  Serial.print("/"); 
  Serial.print(year);
  Serial.print("  Time: "); 
  Serial.print(static_cast<int>(hour)); 
  Serial.print(":"); 
  Serial.print(static_cast<int>(minute)); 
  Serial.print(":"); 
  Serial.print(static_cast<int>(second)); 
  Serial.print("."); 
  Serial.print(static_cast<int>(hundredths));
  Serial.print("  Fix age: ");  
  Serial.print(age); 
  Serial.println("ms.");

  feedgps();

  Serial.print("Alt(cm): "); 
  Serial.print(gps.altitude()); 
  Serial.print(" Course(10^-2 deg): "); 
  Serial.print(gps.course()); 
  Serial.print(" Speed(10^-2 knots): "); 
  Serial.println(gps.speed());
  Serial.print("Alt(float): "); 
  printFloat(gps.f_altitude()); 
  Serial.print(" Course(float): "); 
  printFloat(gps.f_course()); 
  Serial.println();
  Serial.print("Speed(knots): "); 
  printFloat(gps.f_speed_knots()); 
  Serial.print(" (mph): ");  
  printFloat(gps.f_speed_mph());
  Serial.print(" (mps): "); 
  printFloat(gps.f_speed_mps()); 
  Serial.print(" (kmph): "); 
  printFloat(gps.f_speed_kmph()); 
  Serial.println();

  feedgps();

  gps.stats(&chars, &sentences, &failed);
  Serial.print("Stats: characters: "); 
  Serial.print(chars); 
  Serial.print(" sentences: "); 
  Serial.print(sentences); 
  Serial.print(" failed checksum: "); 
  Serial.println(failed);
}

bool feedgps()
{
  while (gpsSerial.available())
  {
    if (gps.encode(gpsSerial.read()))
      return true;
  }
  return false;
}

