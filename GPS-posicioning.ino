String reply;
void setup() {
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, HIGH);
  Serial.begin(9600);
  Serial2.begin(115200);
  delay(5000);
  Serial.println("Starting setting up GPS...");
  set_GPS_settings("gps", "5000", "20");
}

void loop() {
  /*Serial2.print("sip get_hw_model");
  delay(100);
  bool _ready = Serial2.available();
  if (_ready){
    reply = Serial2.readString();
    Serial.println(reply);
    UART_reading_blinking();
  }
  else{
    UART_error();
  }
  Serial2.flush();*/
}

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
  String positiveReply = reply.substring(5,7);
  Serial.println(positiveReply);
  if (positiveReply.equalsIgnoreCase("ok")){
    Serial.println("GPS ready");
  }
  Serial.flush();
  Serial2.flush();
}

void get_GPS_location(){
  
}
