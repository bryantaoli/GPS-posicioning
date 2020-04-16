//16.04.2020 Author: Horoshavin Alexandr, SUAI, Saint-Petersburg.
//---------------------------------------------------------------
//GLOBALS
String reply = "";
bool pressed = false;
short int counter = 0;
unsigned long timer = 0;
String gps_setup_commands[] = {"sip reset", "gps reset", 
                               "gps set_level_shift on", "gps set_start hot",
                               "gps set_satellite_system hybrid",
                               "gps set_positioning_cycle 5000",
                               "gps set_format_uplink ipso",
                               "gps set_port_uplink 20"};
String mac_setup_commands[] = {"mac set_tx_mode cycle", "mac set_tx_interval 5000", "mac set_tx_confirm off", "mac save"};
String gps_uplink_commands[] = {"gps sleep off", "gps set_mode auto", "mac join otaa"};
String sleep_commands[] = {"sip sleep 604800 uart_on","gps set_mode idle", "gps sleep on 0"};
//---------------------------------------------------------------
void setup()
{
  delay(5000);
  pinMode(PC13, OUTPUT);
  pinMode(PA7, INPUT);
  digitalWrite(PC13, HIGH);
  Serial2.begin(115200);
//----------------------------------------------------------------
// UART connection cheking
//----------------------------------------------------------------
  Serial2.print("sip get_ver");
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
  Serial2.flush();
  reply = "";
//----------------------------------------------------------------
// If UART ok led will blink fast 3 times -> starting setup
//---------------------------------------------------------------- 
  bool check = sent_commands(gps_setup_commands);
  while(!check)
  {
    ping(1, 100);
  }
}

void loop()
{
  Serial.println("in loop");
  delay(1000);
}

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

void ping(int n, int _delay)
{
  for(int i = 0; i < n; i++)
  {
    digitalWrite(PC13, LOW);
    delay(_delay);
    digitalWrite(PC13, HIGH);
    delay(_delay);
  }
}

boolean sent_commands(String commands[])
{
  for (int i = 0; i < sizeof(commands)/sizeof(String); i++)
  {
    Serial2.print(commands[i]);
    delay(50);
    if (Serial2.available())
    {
      reply = Serial.readString();
      if(positiveReply(reply))
      {
        ping(1, 300);
      }
      else
      {
        ping(50, 100);
        i = 0;
      }
    }
    else
    {
      while(!Serial2.available())
      {
        ping(1, 100);
      }
      return 0;
    }
    return 1;
  }
}
