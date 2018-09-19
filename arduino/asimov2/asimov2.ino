#include <Arduino.h>
#include <Wire.h>


/////////////////////////////
#define trigPin A4
#define echoPin A5

#define MotorDireitoPWM 3
#define MotorDireitoA 5 // frente11
#define MotorDireitoB 4 // traz10

#define MotorEsquerdoPWM 9
#define MotorEsquerdoA 11 //frente5
#define MotorEsquerdoB 10 //traz4

#define FarolVCC 13
#define FarolGND 12

#define LedVermelho 7
#define LedVerde 6

long duration = 0;
int distance = 300;

const int timeout = 30000;
unsigned long last_message = 0;
bool firstSerial = false;

void calcularDistancia(){
      // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH, 50000);
    // Calculating the distance
    int distanceAnterior = distance;
    distance= duration*0.034/2;
    if(((distanceAnterior > 50) && distance == 0) || distance > 250){
      distance = 250;
    }
    // Prints the distance on the Serial Monitor
    //Serial.print("Distance: ");
    Serial.println((char)distance);
}

void calcularMovimentacao(int Velocidade, int Direcao, int Farol, int Led){  
  // Aqui definimos para o driver a velocidade do cooler e faróis. O sentido é definido no setup.
 
  if(Led){
    digitalWrite(LedVermelho, HIGH);
    digitalWrite(LedVerde, LOW);
  }
  else{
    digitalWrite(LedVermelho, LOW);
    digitalWrite(LedVerde, HIGH);
  }
  if(Farol){
   digitalWrite(FarolVCC, HIGH);
  }
  else{
    digitalWrite(FarolVCC, LOW);
  }
  // Cálculo de rotação dos motores, baseado em ângulo e velocidade.
  float VelocidadeEsquerdo, VelocidadeDireito;

  if (Direcao <= 90 && Direcao >= 0){
    VelocidadeDireito = -cos(2*Direcao*PI/180)*Velocidade;
    VelocidadeEsquerdo = Velocidade;
    //Serial.print("90-0");
    //Serial.println();
  }
  
  else if (Direcao <= 180 && Direcao > 90){
    VelocidadeEsquerdo = -cos(2*Direcao*PI/180)*Velocidade;
    VelocidadeDireito = Velocidade;
    //Serial.print("180-91");
    //Serial.println();
  }

   else if (Direcao <= 270 && Direcao > 180){
    VelocidadeDireito = cos(2*Direcao*PI/180)*Velocidade;
    VelocidadeEsquerdo = -Velocidade;
    //Serial.print("270-181");
    //Serial.println();
  }

   else if (Direcao <= 360 && Direcao > 270){
    VelocidadeEsquerdo = cos(2*Direcao*PI/180)*Velocidade;
    VelocidadeDireito = -Velocidade;
    //Serial.print("359-271");
    //Serial.println();
  }
  
  analogWrite(MotorDireitoPWM, abs(VelocidadeDireito));
  analogWrite(MotorEsquerdoPWM, abs(VelocidadeEsquerdo));

  if (VelocidadeDireito > 0){    
    digitalWrite(MotorDireitoA, HIGH);
    digitalWrite(MotorDireitoB, LOW);
  }
  else{      
    digitalWrite(MotorDireitoA, LOW);
    digitalWrite(MotorDireitoB, HIGH);
  }  
  
  if (VelocidadeEsquerdo > 0){    
    digitalWrite(MotorEsquerdoA, HIGH);
    digitalWrite(MotorEsquerdoB, LOW);
  }
  else{      
    digitalWrite(MotorEsquerdoA, LOW);
    digitalWrite(MotorEsquerdoB, HIGH);
  }
 // Depuracao
 /*
  Serial.print("VelocidadeEsquerdo: ");
  Serial.println(VelocidadeEsquerdo);

  Serial.print("VelocidadeDireito: ");
  Serial.println(VelocidadeDireito);  
  
  Serial.print("Farois: ");
  Serial.println(Farol);

  Serial.print("Led: ");
  Serial.println(Led);
  Serial.println();
  */
}


void setup() 
{
  Serial.begin(9600);
  delay(100);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  pinMode(MotorDireitoPWM, OUTPUT);
  pinMode(MotorDireitoA, OUTPUT);
  pinMode(MotorDireitoB, OUTPUT);
  
  pinMode(MotorEsquerdoPWM, OUTPUT);
  pinMode(MotorEsquerdoA, OUTPUT);
  pinMode(MotorEsquerdoB, OUTPUT);
  
  pinMode(FarolVCC, OUTPUT);
  pinMode(FarolGND, OUTPUT);

  pinMode(LedVermelho, OUTPUT);
  pinMode(LedVerde, OUTPUT);
  
  //------------------------------------
  
  digitalWrite(LedVermelho, HIGH);
  digitalWrite(LedVerde, HIGH);

  digitalWrite(FarolVCC, LOW);
  digitalWrite(FarolGND, LOW);

  digitalWrite(MotorDireitoPWM, LOW);
  digitalWrite(MotorDireitoA, HIGH);
  digitalWrite(MotorDireitoB, LOW);
  
  digitalWrite(MotorEsquerdoPWM, LOW);
  digitalWrite(MotorEsquerdoA, HIGH);
  digitalWrite(MotorEsquerdoB, LOW);
}

void loop() {
  /* Teste de movimentacao
  int espera = 5000;
while(1){
  Serial.print("Direita");
   Serial.println();
  calcularMovimentacao(200, 0, 0, 0);
  delay(espera);
  
   Serial.print("Frente-Direita");
   Serial.println();
  calcularMovimentacao(200, 45, 0, 0);
  delay(espera);
  
  Serial.print("Frente");
   Serial.println();
  calcularMovimentacao(200, 90, 0, 0);
  delay(espera);
  
  Serial.print("Frente-Esquerda");
   Serial.println();
  calcularMovimentacao(200, 135, 0, 0);
  delay(espera);
  
  Serial.print("Esquerda");
   Serial.println();
  calcularMovimentacao(200, 180, 0, 0);
  delay(espera);

  Serial.print("Traz-Esquerda");
   Serial.println();
  calcularMovimentacao(200, 225, 0, 0);
  delay(espera);
  
  Serial.print("Traz");
   Serial.println();
  calcularMovimentacao(200, 270, 0, 0);
  delay(espera);

  Serial.print("Traz-Direita");
   Serial.println();
  calcularMovimentacao(200, 315, 0, 0);
  delay(espera);
  
}
*/

  /* 
   *  Formato dos dados recebidos: "v<velocidade>d<direção>x<servo X>y<servo Y>f<faróis>c<cooler>"
   *  
   *  Velocidade: Varia de -255 a 255 (ré a velocidade máxima)
   *  Direção: Varia de -255 a 255 (esquerda a direita)
   *  Servo X: Varia de -90 a 90
   *  Servo Y: Varia de -90 a 90
   *  Faróis: Varia de 0 a 255 (apagado a brilho máximo)
   *  Cooler: Varia de 0 a 255 (desligado a velocidade máxima)
   *  
   */

  if (Serial.available() > 0){
    firstSerial = true;
    last_message = millis();
    
    Serial.readStringUntil('v');
    int v = Serial.parseInt();          // Valor da velocidade
    
    Serial.readStringUntil('d');
    int d = Serial.parseInt();          // Valor da direção
    
    Serial.readStringUntil('f');
    int f = Serial.parseInt();          // Valor a ser mandado aos faróis
    
    Serial.readStringUntil('c');
    int c = Serial.parseInt();          // Valor a ser mandado ao cooler


    v = constrain(v, 0, 255);
    d = constrain(d, 0, 360);

    f = constrain(f, 0, 1);
    c = constrain(c, 0, 1);

    calcularMovimentacao(v, d, f, c);
    calcularDistancia();
  }


  else if (millis() - last_message > timeout){
    analogWrite(MotorDireitoPWM, 0);
    analogWrite(MotorEsquerdoPWM, 0);
    digitalWrite(FarolVCC, LOW);
    if (millis() % 2000 < 300){
      digitalWrite(LedVermelho, HIGH);
      digitalWrite(LedVerde, LOW);
    }
    else{
      digitalWrite(LedVermelho, HIGH);
      digitalWrite(LedVerde, HIGH);
    }
  }
}


