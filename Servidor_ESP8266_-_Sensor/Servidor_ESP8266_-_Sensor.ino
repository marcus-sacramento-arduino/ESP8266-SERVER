#define TIMEOUT 5000 // mS
#include <dht.h>
#include<stdlib.h>
#define dht_dpin A1 //Pino DATA do Sensor ligado na porta Analogica A1
dht DHT; //Inicializa o sensor
int pin_CO2 = A2;

void setup() 
{
 
 Serial.begin(115200);
 Serial1.begin(115200);

 int CommandStep = 1;

 //This initializes the Wifi Module as a server 

 SendCommand("AT+RST", "Ready", true);

 CommandStep++;


 SendCommand("AT+GMR", "OK", true);

 CommandStep++;

 delay(3000);

 SendCommand("AT+CIFSR", "OK", true);
 CommandStep++;

delay(3000);

 SendCommand("AT+CIPMUX=1","OK",true);
 CommandStep++;

 
 SendCommand("AT+CIPSERVER=1,80","OK",true);
 
  SendCommand("AT+CIFSR", "OK", true);

}

void loop(){
 String IncomingString="";
 char SingleChar;
 boolean StringReady = false;


 //*** Handle each character that is coming in from the ESP8266 ****
 while(Serial1.available()) 
 {
 IncomingChar (Serial1.read ());
 } 
 
 while(Serial.available())
 {
 Serial1.write(Serial.read());
 }

}



void ProcessCommand (const char * data)
{
 Serial.println (data);

 String IncomingLine = String(data);

 if (IncomingLine.indexOf("GET / ") != -1) {
 char ClientIdChar = IncomingLine.charAt(5);
 echoFind("OK"); 
 delay(100);
 SendHTML(String(ClientIdChar));
 }

} 


void SendHTML(String ClientId){
 Serial.println("Someone requested a HTML Page on Client Id:" + ClientId );

 SendClient("<HTML><HEAD><TITLE>Marcus ESP8266 + Arduino Server&copy;</TITLE></HEAD><BODY><H1>Projeto de Leitura de Sensores</H1>",ClientId);
 SendClient("<BR><BR> Temperatura: "+leituraDHT(0),ClientId);
 SendClient("<BR><BR> Umidade: "+leituraDHT(1),ClientId);
 SendClient("<BR><BR> N&iacute;vel CO2: "+leituraCO2(),ClientId);
 
 SendClient("<BR><BR> Up time: ",ClientId);
 SendClient(String(millis()),ClientId);
 SendClient("</BODY></HTML>",ClientId);
 SendCommand("AT+CIPCLOSE="+ ClientId,"OK",true);
}

void SendClient(String ToClientString,String ClientID){
 SendCommand("AT+CIPSEND=" + ClientID + ","+ (ToClientString.length() +2) ,">",false);
 SendCommand(ToClientString,"OK",false);
}



void IncomingChar (const byte InChar)
{
 static char InLine [500]; //Hope we don't get more than that in one line
 static unsigned int Position = 0;

 switch (InChar)
 {
 case '\r': // Don't care about carriage return so throw away.
 break;
 
 case '\n': 
 InLine [Position] = 0; 
 ProcessCommand (InLine);
 Position = 0; 
 break;

 default:
 InLine [Position++] = InChar;
 } 

} 


//************** This section specific to simple AT command with no need to parse the response ************
// Warning: Will drop any characters coming in while waiting for the indicated response.
boolean SendCommand(String cmd, String ack, boolean halt_on_fail)
{
 Serial1.println(cmd); // Send command to module

 // Otherwise wait for ack.
 if (!echoFind(ack)) // timed out waiting for ack string
 if (halt_on_fail)
 errorHalt(cmd+" failed");// Critical failure halt.
 else
 return false; // Let the caller handle it.
 return true; // ack blank or ack found
}

// Read characters from WiFi module and echo to serial until keyword occurs or timeout.
boolean echoFind(String keyword)
{
 byte current_char = 0;
 byte keyword_length = keyword.length();

 // Fail if the target string has not been sent by deadline.
 long deadline = millis() + TIMEOUT;
 while(millis() < deadline)
 {
 if (Serial1.available())
 {
 char ch = Serial1.read();
 Serial.write(ch);
 if (ch == keyword[current_char])
 if (++current_char == keyword_length)
 {
 Serial.println();
 return true;
 }
 }
 }
 return false; // Timed out
}

// Print error message and loop stop.
void errorHalt(String msg)
{
 Serial.println(msg);
 Serial.println("HALT");
 while(true){
 };
}

String leituraDHT(int sensor){
  float temperatura,umidade;
  char charValUmidade[6]="";    
  char charValTemperatura[6]="";      
  String stringValUmidade = "";  
  String stringValTemperatura = "";  
  DHT.read11(dht_dpin); //Lê as informações do sensor 
  temperatura=(float)DHT.temperature;
  umidade=(float)DHT.humidity;
  dtostrf((float)umidade, 5, 2, charValUmidade); 
  for(int i=0;i<sizeof(charValUmidade);i++){
    stringValUmidade+=charValUmidade[i];
  }
  dtostrf((float)temperatura, 5, 2, charValTemperatura); 
  for(int i=0;i<sizeof(charValTemperatura);i++){
    stringValTemperatura+=charValTemperatura[i];
  }
   
  if(sensor==0)
  return stringValTemperatura;
  else
  return stringValUmidade;
}

String leituraCO2(){
  float leituraCO = analogRead(pin_CO2);
  String strMensagem = "";
  char auxleituraCO[10];
  dtostrf(leituraCO,1,2,auxleituraCO);
  return String(auxleituraCO);
}


