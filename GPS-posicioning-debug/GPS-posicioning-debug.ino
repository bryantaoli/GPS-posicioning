//#include "OneButton.h"
//--------------------
String reply;
bool pressed = false;
short int counter = 0;
unsigned long timer = 0;
//OneButton button1(PC13, true);
//--------------------

void setup()
{
  pinMode(PC13, OUTPUT);
  pinMode(PA7, INPUT);
  //button1.attachDoubleClick(UART_reading_blinking);
  //button1.attachLongPressStop(UART_reading_blinking);
  digitalWrite(PC13, LOW);
  Serial.begin(9600);
  Serial2.begin(115200);
  delay(5000);
  digitalWrite(PC13, HIGH);
  Serial2.print("sip get_ver");
  Serial.println("starting process");
  delay(1000);
  if (Serial2.available())
  {
    reply = Serial2.readString();
    Serial.println(reply);
    Serial.println("Starting setting up GPS...");
    delay(100);
    set_GPS_settings("gps", "5000", "20");
    Serial.println("Starting setting up MAC...");
    delay(100);
    set_MAC_settings("5000");
    Serial.println("S76Gxb now is on sleep mode...");
  }
  else
  {
    while (!Serial2.available())
    {
      UART_error();
    }
    Serial.flush();
    Serial2.flush();
    reply = "";
  }
}
//--------------------------------------------------------------

void loop()
{
  if (analogRead(PA7) == HIGH)
  {
    delay(1500);
    pressed = 1;
    counter++;
    counter = counter % 2;
  }
  else
  {
    pressed = 0;
  }
  if (pressed && counter)
  {
    Serial.println("Emergency button pressed \n Getting GPS coordinates");
    digitalWrite(PC13, LOW);
    timer = millis();
    get_GPS_location();
  }
  else if (pressed && !counter)
  {
    Serial.println("Closing uplink");
    digitalWrite(PC13, HIGH);
    set_sleep_mode();
  }
  if (counter && (millis() - timer >= 5000))
  {
    Serial.println(get_data("dd"));
    UART_reading_blinking();
    timer = millis();
    Serial.flush();
    Serial2.flush();
    reply = "";
  }
}

//---------------------------------------------------------------
void UART_error()
{
  digitalWrite(PC13, LOW);
  delay(100);
  digitalWrite(PC13, HIGH);
  delay(100);
  Serial.println("NO UART CONNECTION");
}

void UART_reading_blinking()
{
  digitalWrite(PC13, LOW);
  delay(50);
  digitalWrite(PC13, HIGH);
}

void set_GPS_settings(String satellite_mode, String cycle, String port_uplink)
{
  Serial2.print("gps set_level_shift on");
  Serial.print("gps set_level_shift on");
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_start hot");
  Serial.print("gps set_start hot");
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_satellite_system " + satellite_mode);
  Serial.print("gps set_satellite_system " + satellite_mode);
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_positioning_cycle " + cycle);
  Serial.print("gps set_positioning_cycle " + cycle);
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_format_uplink ipso");
  Serial.print("gps set_format_uplink ipso");
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_port_uplink " + port_uplink);
  Serial.print("gps set_port_uplink " + port_uplink);
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_mode idle");
  Serial.print("gps set_mode idle");
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps sleep on 0");
  Serial.print("gps sleep on 0");
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  if (positiveReply(reply))
  {
    Serial.println("GPS ready");
  }
  else
  {
    Serial.println("Error in GPS settings");
    UART_error();
  }
  reply = "";
  Serial.flush();
  Serial2.flush();
}

void set_MAC_settings(String interval)
{
  Serial2.print("mac set_tx_mode cycle");
  Serial.print("mac set_tx_mode cycle");
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("mac set_tx_interval " + interval);
  Serial.print("mac set_tx_interval " + interval);
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("mac set_tx_confirm off");
  Serial.print("mac set_tx_confirm off");
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("mac save");
  Serial.print("mac save");
  delay(100);
  reply = Serial2.readString();
  Serial.println(reply);
  if (positiveReply(reply))
  {
    Serial.println("MAC ready");
  }
  else
  {
    Serial.println("error in MAC settings");
    UART_error();
  }
  Serial2.print("sip sleep 604800 uart_on");
  Serial.print("sip sleep 604800 uart_on");
  delay(100);
  reply = Serial.readString();
  Serial.println(reply);
  reply = "";
  Serial.flush();
  Serial2.flush();
}

void get_GPS_location()
{
  Serial2.print("gps sleep off");
  Serial.println("gps sleep off");
  delay(500);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_mode auto");
  Serial.println("gps set_mode auto");
  delay(500);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_level_shift on");
  Serial.print("gps set_level_shift on");
  delay(500);
  reply = Serial2.readString();
  Serial.println(reply);
  //  Serial2.print("mac join otaa");
  //  Serial.println("mac join otaa");
  //  delay(500);
  //  reply = Serial2.readString();
  //  Serial.println(reply);
  //  Serial2.print("gps get_mode");
  //  Serial.println("gps get_mode");
  //  delay(500);
  //  reply = Serial2.readString();
  //  Serial.println(reply);
  delay(500);
}

String get_data(String data_format)
{
  Serial2.print("gps get_data " + data_format);
  Serial.println("gps get_data " + data_format);
  delay(100);
  reply = Serial2.readString();
  return reply;
}

void set_sleep_mode()
{
  Serial2.print("gps set_mode idle");
  Serial.print("gps set_mode idle");
  delay(500);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps sleep on 0");
  Serial.print("gps sleep on 0");
  delay(500);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("sip sleep 604800 uart_on");
  Serial.print("sip sleep 604800 uart_on");
  delay(500);
  reply = Serial.readString();
  Serial.println(reply);
  reply = "";
  Serial.flush();
  Serial2.flush();
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
