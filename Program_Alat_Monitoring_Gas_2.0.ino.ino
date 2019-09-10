//Program NodeMCu Pada Penelitian Tugas Akhir 
//Sistem Realisasi Integrasi dan Monitoring Gas Online Dengan Menggunakan Aplikasi
//Web dan Android Berbasis Internet Of Things
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include "HX711.h"
#define DOUT  5
#define CLK  4
HX711 scale;
float calibration_factor = -22;
int GRAM;

int setupDone;
int counter = 0;
int data=0;
int leakSum = 0;
boolean setupAwal = false;
int i=0;
int rst = 0;
String ssid="";
String password="";
String idAlat="";
String token="";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(1024);
  Serial.print("Setup .");
  Serial.println("Memulai Alat Dalam ");
  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");

  scale.begin(DOUT, CLK);
  setupDone = EEPROM.read(0);
  Serial.println(setupDone);
  if(setupDone){
      Serial.println("");
    }else{
      Serial.println("Menkonfigurasi Alat");
      configFirst();   
   }
  //Read
  int address=1;
  int ssidByte = EEPROM.read(address);
  while(ssidByte != 126){  
    ssid = ssid + char(ssidByte);
    address++;
    ssidByte = EEPROM.read(address);
    delay(50);
    }  
  int passByte = EEPROM.read(address);
  address++;
  passByte = EEPROM.read(address);
  Serial.println(passByte);
  while(passByte != 126){
    password = password + char(passByte);
    address++;
    passByte = EEPROM.read(address);
    delay(50);
    }
  address++;
  int idByte = EEPROM.read(address);
  while(idByte != 126){
    idAlat = idAlat + char(idByte);
    address++;
    idByte = EEPROM.read(address);
    delay(50);
  }
  address++;
  int tokenByte = EEPROM.read(address);
  while(tokenByte != 126){
    token = token + char(tokenByte);
    address++;
    tokenByte = EEPROM.read(address);
    } 
    
  char ssidChar[ssid.length()+1];
  char passwordChar[password.length()+1];

  ssid.toCharArray(ssidChar ,ssid.length()+1);
  password.toCharArray(passwordChar,password.length()+1);

  Serial.println(ssidChar);
  Serial.println(passwordChar);
  Serial.println(idAlat);
  Serial.println(token);
  Serial.println("Setup Selesai");  
  delay(1000);
  Serial.println(".");
  delay(1000);
  Serial.println(".");
  delay(1000);
  Serial.println(".");  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssidChar, passwordChar);

  while (WiFi.status() != WL_CONNECTED) {
    if(Serial.available()){
    rst=Serial.read();
    Serial.println(rst);
   if(rst == 82 || rst == 114){
    int confirmRst = Serial.read();
    while(!Serial.available()){
      Serial.println("Do you want to reset ? Y/N ");
      delay(5000);
      }
      confirmRst = Serial.read();
      if(confirmRst == 89 || confirmRst == 121){
      clearEEPROM();
      Serial.println("Berhasil di reset, silahkan restart alat dengan menekan tombol reset");
      
      delay(20000);
      }else{
      Serial.println("Reset dibatalkan");
        }
    }else{
      
      }
   }
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  scale.set_scale();
  scale.tare();
}  

void loop() {
  scale.set_scale(calibration_factor);
  int leak = analogRead(A0);
  leakSum = leakSum + leak;
  GRAM = scale.get_units(), 4;
  data = data + GRAM;
  if(leak>180){
    kirim(idAlat,data,leak);
   }
  
  Serial.print("Berat : ");  
  Serial.println(GRAM);
  Serial.println(data);
  Serial.print("Kebocoran : ");
  Serial.println(leak);
  Serial.println(leakSum);
  if(counter >= 4){
    counter = 0;
    int avData = data / 5;
    int avLeak = leakSum /5;
    data=0;
    leakSum = 0;
    Serial.print("Berat Rata - rata = ");
    Serial.println(avData);
    Serial.print("Nilai ADC kebocoran = ");
    Serial.println(avLeak);
    kirim(idAlat,avData,avLeak); 
    // put your main code here, to run repeatedly:
  }else{
    counter++;
    }
    if(Serial.available()){
    rst=Serial.read();
    Serial.println(rst);
      if(rst == 82 || rst == 114){
        int confirmRst = Serial.read();
        while(!Serial.available()){
          Serial.println("Do you want to reset ? Y/N ");
          delay(5000);
          }
      confirmRst = Serial.read();
      if(confirmRst == 89 || confirmRst == 121){
        clearEEPROM();
        Serial.println("Berhasil di reset, silahkan restart alat dengan menekan tombol reset");
      delay(20000);
      }else{
        Serial.println("Reset Dibatalkan");
        }
    }
   }
  delay(1000);
}


void saveInformation(String ssid,String password,String idAlat, String token){
  Serial.println("inside saveInformation ");
  int indString=0;
  int addr=1;
  int passAddr,idAddr,tokenAddr;
  ssid = ssid + "~";
  password = password + "~";
  idAlat = idAlat + "~";
  token = token + "~";
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(idAlat);
  Serial.println(token);
  
  for(int i=addr;i<ssid.length()+addr;i++){
    Serial.println(ssid[indString]);
    EEPROM.write(i,ssid[indString]);
    indString++;
    passAddr = i+1;
    }
    indString =0;
  for(int i=passAddr;i<password.length()+passAddr;i++){
    Serial.println(password[indString]);
    EEPROM.write(i,password[indString]);
    indString++;
    idAddr = i+1;
    }
    indString = 0;
   for(int i=idAddr;i<idAlat.length()+idAddr;i++){
    Serial.println(idAlat[indString]);
    EEPROM.write(i,idAlat[indString]);
    indString++;
    tokenAddr = i+1;
    }
    indString =0;
   for(int i=tokenAddr;i<token.length()+tokenAddr;i++){
     Serial.println(token[indString]);
     EEPROM.write(i,token[indString]);
     indString++; 
    } 
  setupDone = 1;
  EEPROM.write(0,setupDone);
  EEPROM.commit();
  delay(1000);
  }

void configFirst(){
    String ssid="";
    String password="";
    String idAlat="";
    String token="";
    
    int buff,buffPass,buffId,buffToken;
    buff = Serial.read();
    while(buff< 0){
      Serial.println("Masukan SSID");
      buff = Serial.read();
      delay(1000);
      }
    while(buff != 10){
      ssid = ssid + char(buff);
      buff = Serial.read();
      }    
    while(!Serial.available()){
      Serial.println("Masukan Password");
      delay(1000);
      }
    buffPass = Serial.read();
    while(buffPass != 10){
      password = password + char(buffPass);
      buffPass = Serial.read();
      }
      
    while(!Serial.available()){
      Serial.println("Masukan Id Alat");
      delay(1000);
      }  
    buffId = Serial.read();
    while(buffId != 10){
      idAlat = idAlat + char(buffId);
      buffId = Serial.read();
      }
      
    while(!Serial.available()){
      Serial.println("Masukan Token");
      delay(1000);
     }
    buffToken = Serial.read();
    while(buffToken != 10){
      token = token + char(buffToken);
      buffToken = Serial.read();
      }
      
    Serial.println(ssid);
    Serial.println(password);
    Serial.println(idAlat);
    Serial.println(token);    
    delay(1000);
    saveInformation(ssid,password,idAlat,token);
  }

void clearEEPROM(){
    for(int i=0;i<EEPROM.length();i++){
      EEPROM.write(i,0);
      }  
    EEPROM.commit(); 
    }

void kirim(String idAlat, int avData, int avLeak){
  Serial.println("Kirim");
  Serial.println(idAlat);
  Serial.println(avData);
  Serial.println(avLeak);
   if(WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      char tokenChar[token.length()+1];
      token.toCharArray(tokenChar,token.length()+1);
      http.begin("http://sistem-informasi-gas.herokuapp.com/data/"+idAlat+"/"+avData+"/"+avLeak+"");
      Serial.println(tokenChar);
      http.setAuthorization(tokenChar);
      Serial.println(http.GET());
      Serial.println(http.getString());
      http.end();  
    }else{
      Serial.println("Tidak Terhubung Ke Internet");
      }
  }
