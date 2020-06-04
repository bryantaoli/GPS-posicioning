//16.04.2020 Author: Horoshavin Alexandr, SUAI, Saint-Petersburg.
//---------------------------------------------------------------
//GLOBALS
#include <math.h>
const float R = 6371.210;
String reply = "";
boolean pressed = false;
boolean check = false;
float LAT = 0;
float LONG = 0;
float NLAT = 0;
float NLONG = 0;
unsigned long force_T = 10;//days
unsigned long sleep_T = 1;//days
unsigned long time_1;
unsigned long time_2;
unsigned long time;
short int counter = 0;
float delta = 0;
unsigned int infelicity = 100;//meters
String gps_setup_commands[] = {"gps reset", 
                               "gps set_level_shift on",
                               "gps set_start hot",
                               "gps set_satellite_system hybrid",
                               "gps set_positioning_cycle 5000",
                               "gps set_format_uplink ipso",
                               "gps set_port_uplink 20"};
String mac_setup_commands[] = {"mac set_tx_mode cycle", "mac set_tx_interval 5000",
                               "mac set_tx_confirm off", "mac save"};
String gps_uplink_commands[] = {"gps set_mode auto", "gps set_level_shift on","mac set_tx_confirm on", "mac join otaa"};
String gps_check_commands[] =  {"gps set_mode manual", "gps set_level_shift on"};
String sleep_commands[] = {"gps set_mode idle", "gps sleep on 0", "sip sleep 604800 uart_on"};
//---------------------------------------------------------------
void setup()
{
  force_T = force_T*24*60*60*1000;//to milisecs
  sleep_T = sleep_T*24*60*60*1000;//to milisecs
  pinMode(PC13, OUTPUT);
  pinMode(PA7, INPUT);
  digitalWrite(PC13, HIGH);
  Serial2.begin(115200);
//----------------------------------------------------------------
// UART connection check
//----------------------------------------------------------------
  Serial2.print("sip reset");
  delay(100);
  if(!Serial2.available())
  {
    while(!Serial2.available()){
      ping(1, 100);
      reply = "";
      String trash = Serial2.readString();
      Serial2.print("sip reset");
    }
  }
  else
  {
    ping(3, 300);
  }
  reply = "";
//----------------------------------------------------------------
// If UART ok led will blink fast 3 times -> starting setup
//
// Setting up GPS and put it into sleep mode
//---------------------------------------------------------------- 
  int arr_size = (sizeof(gps_setup_commands)/sizeof(String));
  check = sent_commands(gps_setup_commands, arr_size);
  while(!check)
  {
    ping(1, 100);
  }
//Led pinging if something goes wrong while setting up GPS
//----------------------------------------------------------------
// Setting up MAC and put S76G into sleep mode with UART interrapting
//----------------------------------------------------------------
  arr_size = (sizeof(mac_setup_commands)/sizeof(String));
  check = sent_commands(mac_setup_commands, arr_size);
  while(!check)
  {
    ping(1, 100);
  }
  
  arr_size = (sizeof(gps_uplink_commands)/sizeof(String));
  check = sent_commands(gps_uplink_commands, arr_size);
  while(!check)
  {
    ping(1, 100);
  }
  reply ="";
  while (!reply.equalsIgnoreCase("accepted"))
  {
    reply = Serial2.readString().substring(5,13);
    delay(500);
  }
  Serial2.print("mac set_tx_confirm off");
  reply = "";
  if (Serial2.available()){
    String trash = Serial2.readString();
  }
  
  Serial2.print("gps get_data dd");
  delay(300);
  reply = Serial2.readString();
  LAT = get_LAT(reply);
  LONG = get_LONG(reply);
 
  ping(3, 300);

  arr_size = (sizeof(sleep_commands)/sizeof(String));
  check = sent_commands(sleep_commands, arr_size);
  while(!check)
  {
    ping(1, 100);
  }
//Led pinging if something goes wrong while setting up MAC
//----------------------------------------------------------------
//Cleaning UART boofer trash
//----------------------------------------------------------------
  if (Serial2.available())
    String trash = Serial2.readString();
  reply = "";
  Serial2.flush();
  ping(3, 300);
  time_1 = millis();
  time_2 = millis();
//----------------------------------------------------------------
//End setup.
//----------------------------------------------------------------
}
//___________________________________________________________________________________
void loop()
{
  if (Serial2.available())
    String trash = Serial2.readString();
  reply = "";
  Serial2.flush();
  
  time = millis();
  if (time >= time_1 + sleep_T)
  {
    check = sent_commands(gps_check_commands, 2);
    while(!check)
    {
      ping(1, 300);
      check = sent_commands(gps_check_commands, 2);
    }
    Serial2.print("gps get_data dd");
    delay(300);
    reply = Serial2.readString();
    NLAT = get_LAT(reply);
    NLONG = get_LONG(reply);
    delta = 1000 * fabs(R*acos(sin(LAT)*sin(NLAT) + cos(LAT)*cos(NLAT)*cos(fabs(LONG - NLONG))));//to meters
    if (delta >= infelicity)
    {
        if (Serial2.available())
          String trash = Serial2.readString();
        reply = "";
        int arr_size = (sizeof(gps_uplink_commands)/sizeof(String));
        check = sent_commands(gps_uplink_commands, arr_size);
        while(!check)
        {
          ping(1,300);
        }
        if (Serial2.available())
          String trash = Serial2.readString();
        reply = "";
        while (!reply.equalsIgnoreCase("accepted"))
          {
            reply = Serial2.readString().substring(5,13);
            delay(500);
          }
        arr_size = (sizeof(sleep_commands)/sizeof(String));
        check = sent_commands(sleep_commands, arr_size);
        while(!check)
        {
          ping(1, 100);
        }
        LONG = NLONG;
        LAT = NLAT;
        time_1 = millis();
    }
  }
//  else if(time >= time_2 + force_T)
//  {
//    if (Serial2.available())
//      String trash = Serial2.readString();
//    reply = ""; 
//  }
}
//__________________________________________________________________________________________
//-----------------------------------------------------------------
// Return 1 if reply is Ok, 0 if not.
//-----------------------------------------------------------------
bool positiveReply(String reply)
{
  String positiveReply = reply.substring(5, 7);
  if (positiveReply.equalsIgnoreCase("ok"))
  {
    return true;
  }
  else
  {
    return false;
  }
}
//------------------------------------------------------------------
//Ping n-times, with d delay
//------------------------------------------------------------------
void ping(int n, int d)
{
  for(int i = 0; i < n; i++)
  {
    digitalWrite(PC13, LOW);
    delay(d);
    digitalWrite(PC13, HIGH);
    delay(d);
  }
}
//------------------------------------------------------------------
//Sends the specified set of commands, returns 1 if everything is Ok,
//0 if the UART is unavailable or one of the commands is not executed
//------------------------------------------------------------------
boolean sent_commands(String commands[], int array_size)
{
  if(Serial2.available())
  {
    String trash = Serial2.readString();
  }
  for (int i = 0; i < array_size; i++)
  {
    Serial2.print(commands[i]);
    delay(210);
    if(commands[i].equals("gps sleep on 0") || commands[i].equals("gps sleep off") || commands[i].equals("mac join otaa") || commands[i].equals("gps set_mode idle") || commands[i].equals("gps set_mode auto"))
      delay(3000);
    if (Serial2.available() > 0)
    {
      reply = Serial2.readString();
      if((positiveReply(reply)) || reply.substring(5, 10).equalsIgnoreCase("sleep") || reply.substring(5, 16).equalsIgnoreCase("gps_in_auto"))
      {
        ping(1, 30);
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }
  }
  return 1;
}

float get_LONG(String reply){
  String strLONG = reply.substring(3,20);
  float LONG = strLONG.toFloat();
  return LONG;
}

float get_LAT(String reply){
  String strLAT = reply.substring(25,43);
  float LAT = strLAT.toFloat();
  return LAT;
}
