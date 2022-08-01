#include <SoftwareSerial.h>
#include<LiquidCrystal.h>
#include <Servo.h>
LiquidCrystal lcd (8, 7, 6, 5, 4, 3);
float duration;
float dist;
float Rain;
int temp;
float length = 100; //(98 measurement scale + 2 least measurement of US
sensor)
float db[2];
int ind=0; // to indicate index
float change;
int msg;
char aux_str[129];
int8_t answer;
int t1=0,t2=0;
int date = 3; //user initialize
double month_data[31];
int month =12; //user initialize
double year_data[12];
double annual=0;
double overflow=0;
Servo servo;
String ssid = "Simulator Wifi";
String password = "";
String host = "api.thingspeak.com";
const int httpPort = 80;
String rain_uri = "/update?api_key=95I44JUKBF0S3H5Q&field1=";
//String temp_uri = "/update?api_key=95I44JUKBF0S3H5Q&field2=";
/*const char smtp_server[ ] = "mail.smtp2go.com"; // SMTP server
const char smtp_user_name[ ] = "manojkumar.ts2018@vitstudent.ac.in";
// SMTP user name
const char smtp_password[ ] = "cml0NTY0emdzcDcw"; // SMTP password
const char smtp_port[ ] = "2525"; // SMTP server port
const char sender_address[ ] = "manojkumar.ts2018@vitstudent.ac.in";
// Sender address
const char sender_name[ ] = "Final ProjectGT1"; // Sender name
const char to_address[ ] = "manojscmr333@gmail.com"; // Recipient
address
const char to_name[ ] = "Manoj"; // Recipient name
//Write here the subject and body of the email
char subject[ ] = "INTRUDER ALERT";
const char body1[ ] = "Your area is experiencing Light rainfall with
measurement of ";
const char body2[ ] = "Your area is experiencing moderate rainfall with
measurement of ";
const char body3[ ] = "Your area is experiencing heavy rainfall with
measurement of ";
const char body4[ ] = "Your area is experiencing violent rainfall with
measurement of ";//message to be sent as email.*/
void setup()
{
setupESP8266();
lcd.begin(16,2);
servo.attach(12);
servo.write(0); //close
//Serial.begin(9600);
}
void loop()
{
//Rain Measurement
dist=0.01723 * readUltrasonicDistance(9, 10);
Rain = (length - dist)*10;
db[ind] = Rain;
ind = (ind == 0) ? 1 : 0;
//configEmail();
lcd.setCursor(0, 0);
lcd.print("Rainfall :");
lcd.setCursor(0, 1);
lcd.print(Rain);
Serial.print("Rainfall: ");
Serial.println(Rain);
//writeRain(Rain);
//Temperature Measurement
float val = analogRead(A0);
float voltage = val * 5000.0/1023.0; // converting A0 to mV
if(voltage >500){
temp = round((voltage - 500.0) / 10.0);
Serial.print("Temperature: ");
Serial.println(temp);
//writeTemp(temp);
Serial.println("--------------");
//delay(2000);
}
if(ind==1 && db[ind]>0){
change = db[0] - db[1];
sendMsg(change, Rain);
}else if(ind==0){
change = db[1] - db[0];
sendMsg(change, Rain);
}
//writeData(Rain, temp, change);
//Storage
t2 = millis();
if( month<=12 && (t2-t1) >= 8,64,00,000){ // if it completed 1 day
t1 = t2;
month_data[date] = overflow+Rain;
date++;
overflow=0;
if(month == 2){ // feb
if(date==29){
year_data[month] = addArr(month_data);
month++;
date = 1;
for(int i=0; i<31; i++){
month_data[i] = 0;
}
}
}else if(month == 4 || month == 6 || month == 9|| month == 11){ //
month with 30 days
if(date==31){
year_data[month] = addArr(month_data);
month++;
date = 1;
for(int i=0; i<31; i++){
month_data[i] = 0;
}
}
}else{
if(date==32){
year_data[month] = addArr(month_data);
month++;
date = 1;
for(int i=0; i<31; i++){
month_data[i] = 0;
}
}
}
}
//servo motor
if(dist<20){
overflow += Rain;
servo.write(180); //open
while(Rain >=0){
dist=0.01723 * readUltrasonicDistance(9, 10);
Rain = (length - dist)*10;
}
servo.write(0); //close
}
delay(10000);
//delay(300000)// 5 minutes
}
int setupESP8266(void) {
Serial.begin(115200);
//Serial.println("AT+UART_DEF=9600,8,1,0,0");
//Serial.begin(9600);
Serial.println("AT");
delay(1000);
if (!Serial.find("OK")) return 1;
Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
delay(10);
if (!Serial.find("OK")) return 2;
Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
delay(50);
if (!Serial.find("OK")) return 3;
return 0;
}
/*int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned
int timeout){
uint8_t x=0, answer=0;
char response[100];
unsigned long previous;
memset(response, '\0', 100); // Initialice the string
delay(100);
while( Serial.available() > 0) Serial.read(); // Clean the input
buffer
Serial.println(ATcommand); // Send the AT command
x = 0;
previous = millis();
// this loop waits for the answer
do{
// if there are data in the UART input buffer, reads it and checks
for the answer
if(Serial.available() != 0){
response[x] = Serial.read();
x++;
// check if the desired answer is in the response of the module
if (strstr(response, expected_answer) != NULL)
{
answer = 1;
}
}
// Waits for the answer with time out
}
while((answer == 0) && ((millis() - previous) < timeout));
return answer;
}
void configEmail(){
// sets the SMTP server and port
sprintf(aux_str, "AT+SMTPSRV=\"%s\",%s", smtp_server, smtp_port);
sendATcommand(aux_str, "OK", 2000);
// sets user name and password
sprintf(aux_str, "AT+SMTPAUTH=1,\"%s\",\"%s\"", smtp_user_name,
smtp_password);
sendATcommand(aux_str, "OK", 2000);
// sets sender adress and name
sprintf(aux_str, "AT+SMTPFROM=\"%s\",\"%s\"", sender_address,
sender_name);
sendATcommand(aux_str, "OK", 2000);
// sets sender adress and name
sprintf(aux_str, "AT+SMTPRCPT=1,0,\"%s\",\"%s\"", to_address,
to_name);
sendATcommand(aux_str, "OK", 2000);
// subjet of the email
sprintf(aux_str, "AT+SMTPSUB=\"%s\"", subject);
sendATcommand(aux_str, "OK", 2000);
}*/
void writeData(float data, int temp, float change){
String httpPacket = "GET " + rain_uri + String(data) + "&field2="+
String(temp)+ "&field3="+ String(change)+ " HTTP/1.1\r\nHost: " + host
+ "\r\n\r\n";
int length = httpPacket.length();
// Send our message length
Serial.print("AT+CIPSEND=");
Serial.println(length);
delay(10);
Serial.print(httpPacket);
delay(10);
if (!Serial.find("SEND OK\r\n")) return;
}
int sendMsg(float change,float Rain ){
if(change < 0.208 && msg != 1){
msg = 1;
writeData(Rain, temp, 1);
}else if(change >= 0.208 && change < 0.833 && msg != 2){
//Serial.print("Your area is experiencing Moderate rainfall with
measurement of ");
//Serial.println(Rain);
msg = 2;
writeData(Rain, temp, 2);
}else if(change >= 0.833 && change < 4.167 && msg != 3){
//Serial.print("Your area is experiencing Heavy rainfall with
measurement of ");
//Serial.println(Rain);
msg = 3;
writeData(Rain, temp, 3);
}else if(change >= 4.167 && msg != 4){
//Serial.print("Your area is experiencing violent rainfall with
measurement of ");
//Serial.println(Rain);
msg = 4;
writeData(Rain, temp, 4);
}else{
writeData(Rain, temp, 0);
}
}
double addArr(double* arr){
double sum=0;
for(int i=0; i<sizeof(arr); i++){
sum += arr[i];
}
return sum;
}
long readUltrasonicDistance(int triggerPin, int echoPin)
{
pinMode(triggerPin, OUTPUT); // Clear the trigger
digitalWrite(triggerPin, LOW);
delayMicroseconds(2);
// Sets the trigger pin to HIGH state for 10 microseconds
digitalWrite(triggerPin, HIGH);
delayMicroseconds(10);
digitalWrite(triggerPin, LOW);
pinMode(echoPin, INPUT);
// Reads the echo pin, and returns the sound wave travel time in
microseconds
return pulseIn(echoPin, HIGH);
}