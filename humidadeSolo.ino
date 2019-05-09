#if defined(ESP8266)
#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library         
#else
#include <WiFi.h>      //ESP32 Core WiFi Library    
#endif
 
#if defined(ESP8266)
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#else
#include <WebServer.h> //Local WebServer used to serve the configuration portal ( https://github.com/zhouhan0126/WebServer-esp32 )
#endif
 
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal ( https://github.com/zhouhan0126/DNSServer---esp32 )
#include <WiFiManager.h>   // WiFi Configuration Magic ( https://github.com/zhouhan0126/WIFIMANAGER-ESP32 ) >> https://github.com/tzapu/WiFiManager (ORIGINAL)

#include <IOXhop_FirebaseESP32.h>
#define FIREBASE_HOST "esp32-higronometro.firebaseio.com/"
#define FIREBASE_AUTH "SzIyGCHCyd54SA60PcM5edEKkZUF0HF93gMFCzft"
const int PIN_AP = 2; 
#define pino_sinal_analogico 35
const int ledgreen = 33; 
const int ledred = 26;
const int ledyellow = 27;
const int piscaledred=14;
const int piscaledyellow= 13;
const int sequencialed = 25;
const int ledaltobrilho = 32;
int valor_analogico;
int porcentagem;
int i =5; 
String data;
String espaco ="  ", simbolo = "%";
void setup()
{
  pinMode(PIN_AP, INPUT);
  Serial.begin(115200);
  pinMode(ledgreen,OUTPUT);
  pinMode(ledyellow,OUTPUT);
  pinMode(ledred,OUTPUT);
  pinMode(piscaledred,OUTPUT);
  pinMode(piscaledyellow,OUTPUT);
  pinMode(sequencialed,OUTPUT);
  pinMode(ledaltobrilho,OUTPUT);

  //declaração do objeto wifiManager
  WiFiManager wifiManager;
 
  //utilizando esse comando, as configurações são apagadas da memória
  //caso tiver salvo alguma rede para conectar automaticamente, ela é apagada.
//  wifiManager.resetSettings();
 
//callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback); 
//callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback); 
 
//cria uma rede de nome ESP_AP com senha 12345678
  wifiManager.autoConnect("ESP_AP","12345678"); 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}


 
void loop()
{
  if ( digitalRead(PIN_AP) == HIGH ) 
   {
      WiFiManager wifiManager;
      if(!wifiManager.startConfigPortal("ESP_AP", "12345678") )
      {
        delay(2000);
        ESP.restart();
      }  
    }
    if (Firebase.failed()) { 
      return;
   }
  if(Firebase.getInt("/State")==1){

     
   if(Firebase.getInt("/StateLed")==1)
  {
    digitalWrite(ledaltobrilho,HIGH);
  }else
  {
    digitalWrite(ledaltobrilho,LOW);
  }
    if(touchRead(4)<15)
 {
  digitalWrite(sequencialed,HIGH); 
 }else
 {
  digitalWrite(sequencialed,LOW);
 }                                       
  //Le o valor do pino A0 do sensor
  valor_analogico = analogRead(pino_sinal_analogico);
 
  //Mostra o valor da porta analogica no serial monitor
 
  //Solo umido, acende o led verde
  data = Firebase.getString("/Date");
  if (valor_analogico== 4095)
  { 
    AcenderRed();
    porcentagem=0;
    
  }
  if(valor_analogico<3800 && valor_analogico>3300)
  {
    AcenderRed();
    porcentagem=25;
  }
 
  //Solo com umidade moderada, acende led amarelo
  if (valor_analogico <3300 && valor_analogico>2800)
  {
    AcenderYellow();
    porcentagem=50;
  }
  if (valor_analogico <2800 && valor_analogico>2550)
  {
    AcenderGreen();
    porcentagem=75;
  }
 
  //Solo seco, acende led vermelho
  if (valor_analogico <2550)
  {
    AcenderGreen();
    porcentagem=100;
  }
   
  Firebase.setInt("HumidityValue",porcentagem);
  Firebase.setInt("AnalogValue", valor_analogico);
  Firebase.push("HumidityList",porcentagem + simbolo + espaco + data);
  Firebase.push("AnalogList", valor_analogico + espaco + data);
  
  PiscaLedRed();
  delay(200);
  PiscaLedYellow();
  }else
  {
    digitalWrite(ledgreen,LOW);
    digitalWrite(ledyellow,LOW);
    digitalWrite(ledred,LOW);
    digitalWrite(piscaledred,LOW);
    digitalWrite(piscaledyellow,LOW);
    digitalWrite(sequencialed,LOW);
    digitalWrite(ledaltobrilho,LOW);
  }
}

//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager *myWiFiManager) {  
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
 
}
//callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  Serial.println("Configuração salva");
}
void AcenderGreen()
{
  digitalWrite(ledgreen,HIGH);
  digitalWrite(ledred,LOW);
  digitalWrite(ledyellow,LOW);
}
void AcenderRed()
{
digitalWrite(ledred,HIGH);
digitalWrite(ledgreen,LOW);
digitalWrite(ledyellow,LOW);
}
void AcenderYellow()
{
  digitalWrite(ledyellow,HIGH);
  digitalWrite(ledgreen,LOW);
  digitalWrite(ledred,LOW);
}
void PiscaLedYellow()
{
  digitalWrite(piscaledred,LOW);
  delay(200);
  digitalWrite(piscaledyellow,HIGH);
}
void PiscaLedRed()
{
  digitalWrite(piscaledred,HIGH);
  delay(200);
  digitalWrite(piscaledyellow,LOW);
}

 
