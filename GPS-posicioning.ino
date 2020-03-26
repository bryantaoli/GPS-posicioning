
String reply;
bool pressed = false;

void setup() {
  pinMode(PC13, OUTPUT);
  pinMode(PA1, INPUT);
  digitalWrite(PC13, HIGH);
  Serial.begin(9600);
  Serial2.begin(115200);
  delay(5000);
  Serial2.print("sip get_ver");
  delay(1000);
  if(Serial2.available()){
    Serial.println("Starting setting up GPS...");
    set_GPS_settings("gps", "5000", "20");
    Serial.println("Starting setting up MAC...");
    set_MAC_settings("5000");
    Serial.println("S76Gxb now is on sleep mode...");
  }
  else{
    while(!Serial2.available()){
      UART_error();
    }
    
  } 
}
//--------------------------------------------------------------

void loop() {
  if (analogRead(PA1) == HIGH){
    pressed = !pressed;
    Serial.println(pressed);
    digitalWrite(PC13, LOW);
  }
  else{
    digitalWrite(PC13, HIGH);
  }
}

//---------------------------------------------------------------
void UART_error(){
    digitalWrite(PC13,LOW);
    delay(100);
    digitalWrite(PC13, HIGH);
    delay(100);
    Serial.println("NO UART CONNECTION");
}

void UART_reading_blinking(){
    digitalWrite(PC13,LOW);
    delay(50);
    digitalWrite(PC13,HIGH);
}

void set_GPS_settings(String satellite_mode, String cycle, String port_uplink){
  Serial2.print("gps set_level_shift on");
  Serial.print("gps set_level_shift on");
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_start hot");
  Serial.print("gps set_start hot");
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_satellite_system " + satellite_mode);
  Serial.print("gps set_satellite_system " + satellite_mode);
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_positioning_cycle " + cycle);
  Serial.print("gps set_positioning_cycle " + cycle);
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_format_uplink ipso");
  Serial.print("gps set_format_uplink ipso");
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_port_uplink " + port_uplink);
  Serial.print("gps set_port_uplink " + port_uplink);
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps set_mode idle");
  Serial.print("gps set_mode idle");
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("gps sleep on 0");
  Serial.print("gps sleep on 0");
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  if (positiveReply(reply)){
    Serial.println("GPS ready");
  }
  else{
    Serial.println("Error in GPS settings");
    UART_error();
  }
  reply = "";
  Serial.flush();
  Serial2.flush();
}

void set_MAC_settings(String interval){
  Serial2.print("mac set_tx_mode cycle");
  Serial.print("mac set_tx_mode cycle");
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("mac set_tx_interval " + interval);
  Serial.print("mac set_tx_interval " + interval);
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("mac set_tx_confirm off");
  Serial.print("mac set_tx_confirm off");
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  Serial2.print("mac save");
  Serial.print("mac save");
  delay(10);
  reply = Serial2.readString();
  Serial.println(reply);
  if (positiveReply(reply)){
    Serial.println("MAC ready");
  }
  else{
    Serial.println("error in MAC settings");
    UART_error();
  }
  Serial2.print("sip sleep 604800 uart_on");
  Serial.print("sip sleep 604800 uart_on");
  delay(10);
  reply = Serial.readString();
  Serial.println(reply);
  reply = "";
  Serial.flush();
  Serial2.flush();
}

void get_GPS_location(){
  Serial2.print("---");
  Serial.println("---");
  delay(10);
  reply = Serial.readString();
  Serial.println(reply);
}

bool positiveReply(String reply){
  String positiveReply = reply.substring(5, 7);
  if (positiveReply.equalsIgnoreCase("ok")){
    return true;
  }
  else{
    return false;
  }
}
