#include "DHT.h"
#include <SdFat.h>
#include <Wire.h>
#include <DS3231.h>

SdFat sdCard;
SdFile meuArquivo;

DS3231 rtc;              //Criação do objeto do tipo DS3231
RTCDateTime dataehora;   //Criação do objeto do tipo RTCDateTime

const int chipSelect = 10; // what digital pin sdcard is connected to


int valor_analogico;

float valorLido;

byte byteRead;


#define umidade_solo A0

#define DHTPIN A1
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


// constante para configuração do prescaler
//const unsigned char PS_16 = (1 << ADPS2);
//const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
//const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
 
void setup() {
  
 Serial.begin(9600);
 rtc.begin();
 dht.begin();
 pinMode(umidade_solo, INPUT);
 pinMode(chipSelect, OUTPUT); 
 

 rtc.setDateTime(__DATE__, __TIME__);   //Configurando valores iniciais do RTC DS3231
                                         
if(!sdCard.begin(chipSelect,SPI_HALF_SPEED))sdCard.initErrorHalt();
  // Abre o arquivo planta.txt
  if (!meuArquivo.open("planta.txt", O_RDWR | O_CREAT | O_AT_END))
    sdCard.errorHalt("Erro na abertura do arquivo planta.txt!");
 
 // configura o preescaler do ADC
 ADCSRA &= ~PS_128;  //limpa configuração da biblioteca do arduino
 
 // valores possiveis de prescaler só deixar a linha com prescaler desejado
 // PS_16, PS_32, PS_64 or PS_128
 ADCSRA |= PS_128; // 64 prescaler
    //ADCSRA |= PS_64; // 64 prescaler
   //ADCSRA |= PS_32; // 32 prescaler
  //ADCSRA |= PS_16; // 16 prescaler
 
}
 
void loop() {  


  dataehora = rtc.getDateTime();     

  Serial.print(dataehora.year);     //Imprimindo o Ano   
  Serial.print("-");
  Serial.print(dataehora.month);    //Imprimindo o Mês
  Serial.print("-");
  Serial.print(dataehora.day);      //Imprimindo o Dia
  Serial.print(" ");
  Serial.print(dataehora.hour);     //Imprimindo a Hora
  Serial.print(":");
  Serial.print(dataehora.minute);   //Imprimindo o Minuto
  Serial.print(":");
  Serial.print(dataehora.second);   //Imprimindo o Segundo
  Serial.println("");

valor_analogico = analogRead(umidade_solo);
valorLido = map(valor_analogico,1023,0,100,0); //EXECUTA A FUNÇÃO "map" DE ACORDO COM OS PARÂMETROS PASSADOS

Serial.print("Umidade do solo: "); 
Serial.print(valorLido); 
Serial.println("%"); 


   // A leitura da temperatura e umidade pode levar 250ms!
   float h = dht.readHumidity();
   float t = dht.readTemperature();

   if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else
  {
    Serial.print("Umidade do ar: "); 
    Serial.print(h);
    Serial.print("      Temperatura do ar: ");
    Serial.print(t);
    Serial.println(" *C");
  }


  // Grava dados do dht11, umidade do solo além de data e hora em planta.txt
  meuArquivo.print(dataehora.year);
  meuArquivo.print("-");
  meuArquivo.print(dataehora.month); 
  meuArquivo.print("-");
  meuArquivo.print(dataehora.day);
  meuArquivo.print(" ");
  meuArquivo.print(dataehora.hour); 
  meuArquivo.print(":");
  meuArquivo.print(dataehora.minute);
  meuArquivo.print(":");
  meuArquivo.print(dataehora.second);
  meuArquivo.println("");

  meuArquivo.print("Umidade(ar): ");
  meuArquivo.print(h);
  meuArquivo.print("        Temperatura(ar*C): ");
  meuArquivo.print(t);
  meuArquivo.print("        Umidade(solo): ");
  meuArquivo.print(valorLido);
  meuArquivo.println("%");
  
  
   if (Serial.available()) //se byte pronto para leitura
   {
    switch(Serial.read())      //verifica qual caracter recebido
    {
      case 's':            
        Serial.println("Processo de gravacao interrompido. Retire o SD!");
        meuArquivo.println("-------------------------------------------------------------------------------");
        meuArquivo.close();
      break;
    }
  }
   
   delay(300); 
}


