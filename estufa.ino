#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "Classes.h"
#define buzzer  8
#define porta  9
#define res0  10  
#define res1  11  
#define res2  12  
#define lamp  13
#define CMD HIGH  //facilita a logica dependendo do hardware

#define Normal 1
#define PVC 0
#define serial 0

#if Normal && !PVC //impede que os dois modos sejam ativados juntos

#define Med 70          //temperatura média
#define Max (Med*1.05)  //temperatura maxima
#define Min (Med*0.95)  //temperatura minima
#define Mmin (Med*0.85) //temperatura limite das duas resistências ativas
#define Emg (Med*1.20)  //temperatura fora do normal
#define Seg 40          //temperatura de segurança para se aproximar da estufa

#endif

#if PVC &&!Normal //impede que os dois modos sejam ativados juntos

#define Med 45         //temperatura média
#define Max (Med*1.05)  //temperatura maxima
#define Min (Med*0.95)  //temperatura minima
#define Mmin (Med*0.80) //temperatura limite das duas resistências ativas
#define Emg (Med*1.20)  //temperatura fora do normal
#define Seg 30          //temperatura de segurança para se aproximar da estufa

#endif

Adafruit_BMP085 bmp;
Helpful util; //declaração da classe

float temp;
int cont = 0;
bool bBuzzer, bResM, bResS, bLamp; //variaveis pra simplificar o digitalWrite

void setup() {
  pinMode(porta, INPUT_PULLUP);
  pinMode(res0, OUTPUT);
  pinMode(res1, OUTPUT);
  pinMode(res2, OUTPUT);
  pinMode(lamp, OUTPUT);
  bLamp = LOW;
  bBuzzer = !CMD; //buzzer desativado
  bResM = LOW;
  bResS = LOW;

#if serial
  Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Erro no barômetro");
  while (1) {}
  }
  else Serial.println("Barômetro funcionando");
#endif 
}

void loop() {
  if (bmp.begin())
  {
    temp = bmp.readTemperature(); //função que retorna o valor de temperatura em celsius
#if serial
    Serial.println(temp);
#endif
  }
  if (digitalRead(porta) == HIGH) //porta aberta
  {
    bResM = LOW;
    bResS = LOW;
    bLamp = HIGH;
    if (temp > Seg)
    {
      if (util.eachT(1)) //a cada 1s ele retorna verdadeiro
      {
        bBuzzer = !bBuzzer; //buzzer liga e desliga
      }
    }
    else
    {
      bBuzzer = !CMD; //buzzer desativado
    }
  }
  else if (temp >= Emg)
  {
    bResM = LOW;
    bResS = LOW;
    bLamp = LOW;
    bBuzzer = CMD; //buzzer ativado
  }
  else if (temp >= Max)
  {
    bResM = LOW;
    bResS = LOW;
    bLamp = LOW;
    bBuzzer = !CMD; //buzzer desativado
  }
  else if (temp <= Mmin)
  {
    bResM = HIGH;
    bResS = HIGH;
    bLamp = LOW;
    bBuzzer = !CMD; //buzzer desativado
  }
  else if (temp <= Min)
  {
    bResM = HIGH;
    bResS = LOW;
    bLamp = LOW;
    bBuzzer = !CMD; //buzzer desativado
  }
  if(util.eachT(600)) cont = ++cont%3;
    digitalWrite(res0, (cont == 0) ? bResS : bResM); 
    digitalWrite(res1, (cont == 1) ? bResS : bResM);  
    digitalWrite(res2, (cont == 2) ? bResS : bResM);
    digitalWrite(buzzer, bBuzzer); 
    digitalWrite(lamp, bLamp);  
}

