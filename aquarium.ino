#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(5,4);
LiquidCrystal_I2C lcd(0x27, 16, 2);
int buzzer = 12;
int sensorPin = A3;
int ONE_WIRE_BUS=13;
float Temperature;
float Turbidity;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
void setup(void) 
{
 Serial.begin(9600);
 mySerial.begin(115200);
 pinMode(buzzer,OUTPUT);
  lcd.init(); 
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("   Welcome to      ");
  lcd.setCursor(0, 1);
  lcd.print("Aquarium System");
  delay(2000);
  lcd.clear();
  sensors.begin();
}
void loop(void) {
 int sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  Turbidity = map(sensorValue, 0, 750, 100, 0);
  Serial.println(Turbidity);
  
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Turb=");
  lcd.print(Turbidity);
  lcd.setCursor(9, 0);
  lcd.print("%");
  
  
  delay(100);
  if (Turbidity >=0 && Turbidity < 20) {
    lcd.setCursor(10, 0);
    lcd.print(" CLEAR ");
    digitalWrite (buzzer,LOW ) ;
    Serial.println("Maji Masafi");
    delay(1000);
  }
  if ((Turbidity > 20) && (Turbidity < 50)) {
    digitalWrite (buzzer,LOW ) ;
    lcd.setCursor(10, 0);
    lcd.print("CLOUDY ");
    Serial.println("Maji Yana Ukungu");
      mySerial.println("AT"); 
  updateSerial();
  mySerial.println("AT+CMGF=1"); 
  updateSerial();
  mySerial.println("AT+CMGS=\"+255623970020\"");
  updateSerial();
  mySerial.print("Maji Yana Ukungu"); 
  updateSerial();
  mySerial.write(26);
    delay(1000);
  }
  if (Turbidity > 50 && Turbidity < 100 ) {
    digitalWrite (buzzer,HIGH );
    lcd.setCursor(10, 0);
    lcd.print("DIRTY ");
    Serial.println("Maji Machafu");
  mySerial.println("AT"); 
  updateSerial();
  mySerial.println("AT+CMGF=1"); 
  updateSerial();
  mySerial.println("AT+CMGS=\"+255623970020\"");
  updateSerial();
  mySerial.print("Maji Machafu"); //text content
  updateSerial();
  mySerial.write(26);
    delay(1000);
  }
    sensors.requestTemperatures(); 
  Temperature = sensors.getTempCByIndex(0);
  Serial.print(Temperature);
  Serial.println("ºC");
    lcd.setCursor(0, 1);
    lcd.print("Temperature=");
    lcd.print(Temperature);
    if(Temperature>30){
  mySerial.println("AT"); 
  updateSerial();
  mySerial.println("AT+CMGF=1"); 
  updateSerial();
  mySerial.println("AT+CMGS=\"+255623970020\"");
  updateSerial();
  mySerial.print("Temperature Ipo "); 
  mySerial.print(Temperature);
   mySerial.print("ºC");  
  updateSerial();
  mySerial.write(26);
  digitalWrite (buzzer,HIGH ) ; 
      }
  delay(1000); 
   if (mySerial.available())
    Serial.write(mySerial.read());
  mySerial.println("AT");
  delay(100);
  mySerial.println("AT+CPIN?");
  delay(100);
  mySerial.println("AT+CREG?");
  delay(100);
  mySerial.println("AT+CGATT?");
  delay(100);
mySerial.println("AT+CIPSHUT");
  delay(100);
  mySerial.println("AT+CIPSTATUS");
  delay(100);
  mySerial.println("AT+CIPMUX=0");
  delay(100);
  ShowSerialData();
  mySerial.println("AT+CSTT=\"tigoweb\"");//start task and setting the APN,
  delay(100);
  ShowSerialData();
  mySerial.println("AT+CIICR");//bring up wireless connection
  delay(100);
  ShowSerialData();
  mySerial.println("AT+CIFSR");//get local IP adress
  delay(100);
 
  ShowSerialData();
  mySerial.println("AT+CIPSPRT=0");
  delay(100);
  ShowSerialData(); 
  mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(100);
  ShowSerialData();
  mySerial.println("AT+CIPSEND");//begin send data to remote server
  delay(100);
  ShowSerialData();
  String str="GET https://api.thingspeak.com/update?api_key=VI99DIRMX9KNZZSX&field1=" + String(Temperature) +"&field2="+String(Turbidity);
  Serial.println(str);
  mySerial.println(str);//begin send data to remote server
  delay(100);
  ShowSerialData();
  mySerial.println((char)26);//sending
  delay(100);//waitting for reply, important! the time is base on the condition of internet 
  mySerial.println();
  ShowSerialData();
  mySerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
  toCloud();
}
void updateSerial(){
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read());
  }
  while(mySerial.available()) {
    Serial.write(mySerial.read());
  }
}
void ShowSerialData()
{
  while(mySerial.available()!=0)
  Serial.write(mySerial.read());
  delay(100); 
}
void toCloud()
{

   // initialize http service
   mySerial.println("AT+HTTPINIT");
   delay(2000); 
   toSerial();

   // set the HTTP session
  mySerial.println("AT+HTTPSSL=1");
  delay(2000);
  toSerial();


   // set the HTTP session
  mySerial.println("AT+HTTPPARA=\"CID\",1");
  delay(2000);
  toSerial();

   // set http param value
  mySerial.print("AT+HTTPPARA=\"URL\",\"");
   mySerial.print("&Temperature=");  
   mySerial.print(Temperature); 
   mySerial.print("&Turbidity=");
  mySerial.print(Turbidity);
 
    
 mySerial.println("\"");
   delay(2000);
   toSerial();
  
   // set http action type 0 = GET, 1 = POST, 2 = HEAD
   mySerial.println("AT+HTTPACTION=0");
   delay(6000);
   toSerial();


    // read server response
mySerial.println("AT+HTTPREAD"); 
   delay(6000);
   toSerial();

   mySerial.println("");
  mySerial.println("AT+HTTPTERM");
   toSerial();
   delay(3000);

   mySerial.println("");
   delay(10000);
  
}
void toSerial()
{
  while(mySerial.available()!=0)
  Serial.write(mySerial.read());
  delay(5000); 
  
}
