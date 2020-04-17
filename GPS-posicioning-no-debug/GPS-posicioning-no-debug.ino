//16.04.2020 Author: Horoshavin Alexandr, SUAI, Saint-Petersburg.
//---------------------------------------------------------------
//GLOBALS
String reply = "";
boolean pressed = false;
boolean check = false;
short int counter = 0;
String gps_setup_commands[] = {"gps reset", 
                               "gps set_level_shift on",
                               "gps set_start hot",
                               "gps set_satellite_system hybrid",
                               "gps set_positioning_cycle 5000",
                               "gps set_format_uplink ipso",
                               "gps set_port_uplink 20"};
String mac_setup_commands[] = {"mac set_tx_mode cycle", "mac set_tx_interval 5000",
                               "mac set_tx_confirm off", "mac save"};
String gps_uplink_commands[] = {"gps set_mode auto", "gps set_level_shift on", "mac join otaa"};
String sleep_commands[] = {"gps set_mode idle", "gps sleep on 0", "sip sleep 604800 uart_on"};
//---------------------------------------------------------------
void setup()
{
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
//----------------------------------------------------------------
//End setup. Now STM will wait for the button to be pressed
//----------------------------------------------------------------
}

void loop()
{
  String trash = "";
  if (Serial2.available())
    trash = Serial2.readString();
  if(analogRead(PA7) == HIGH)
  {
    delay(1500);
    pressed = true;
    counter = (counter + 1) % 2;
  }
  else
    pressed = false;
  if (pressed && counter)
  {
    Serial2.print("gps sleep off");
    delay(700);
    check = sent_commands(gps_uplink_commands, 3);
    if(!check)
    {
      ping(20, 100);
      counter = 0;
    }
    else
      digitalWrite(PC13, LOW);
  }
  else if (pressed && !counter)
  {
    check = sent_commands(sleep_commands, 3);
    if(!check)
    {
      ping(1, 100);
    }
    else
      digitalWrite(PC13, HIGH);
  }
}
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
