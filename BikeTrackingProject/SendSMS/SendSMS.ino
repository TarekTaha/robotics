/*  
 Example 26.3 GSM shield sending a SMS text message
 http://tronixstuff.com/tutorials > chapter 26 
 */

#define cell Serial1  // We need to create a serial port on D2/D3 to talk to the GSM module
char mobilenumber[] = "0433292xxx";  // Replace xxxxxxxx with the recipient's mobile number
void setup()
{  
  //Initialize serial ports for communication.
  Serial.begin(9600);
  cell.begin(9600);
  Serial.println("Starting SM5100B Communication...");
  delay(35000); // give the GSM module time to initialise, locate network etc.
  // this delay time varies. Use example 26.1 sketch to measure the amount
  // of time from board reset to SIND: 4, then add five seconds just in case
}
char incoming_char;
void loop()
{ 
  if(cell.available() >0)
  {
    incoming_char=cell.read();    //Get the character from the cellular serial port.
    Serial.print(incoming_char);  //Print the incoming character to the terminal.
  }

  //If a character is coming from the terminal to the Arduino...
  if(Serial.available() >0)
  {
    incoming_char=Serial.read();  //Get the character coming from the terminal
    cell.print(incoming_char);    //Send the character to the cellular module.
  }
  
  cell.println("AT+CMGF=1"); // set SMS mode to text
  cell.print("AT+CMGS=");  // now send message...
  cell.print(34); // ASCII equivalent of "
  cell.print(mobilenumber);
  cell.println(34);  // ASCII equivalent of "
  delay(500); // give the module some thinking time
  cell.print("They call me the count... because I like to count! Ah ha ha ha");   // our message to send
  cell.println(26);  // ASCII equivalent of Ctrl-Z
  delay(15000); // the SMS module needs time to return to OK status
  do // You don't want to send out multiple SMSs.... or do you?
  {
    delay(1);
  }
  while (1>0);
}


