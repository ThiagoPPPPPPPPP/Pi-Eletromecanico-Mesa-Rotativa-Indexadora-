#include <Arduino.h>

// ======================================================
//                    MESA INDEXADORA 
// ======================================================


//V18 CAPACITIVO SAIDA
//V15 INDUTIVO ENTRADA
//V17 CAPACITIVO ENTRADA
//V16 INDUTIVO SAIDA





  
// ================================
//       SENSORES IND + CAP 
// ================================
#define SENIND1 66
#define SENIND2 67
#define SENC1   68
#define SENC2   69

// ================================
//               LEDS
// ================================
#define LED1 34
#define LED2 36
#define LED3 38
#define LED4 40

// ================================
//         SOLENOIDE 
// ================================
#define SOLENOIDE 43   // CH1 

// ================================
//             MOTOR
// ================================
#define STEP_PIN   41
#define DIR_PIN    39
#define PASSOS_POR_POSICAO 1333
#define VELOCIDADE_US 800

// ================================
//           VARIÁVEIS
// ================================
bool pecas[6] = {0,0,0,0,0,0};
bool mesaLigada = false;
bool cicloAtivo = false;

unsigned long marcaTempo = 0;
const unsigned long tempoEtapa = 4000;

int v15 = 0, v16 = 0, v17 = 0, v18 = 0;

// forward declarations
void desligarTudo();
bool detectarPecaEntrada();
void ligarMesa();
void cicloIndexador();
int leituraMaterialEntrada();
int leituraMaterialSaida();
void expulsarPeca();
void finalizarCiclo();
void moverPosicao();
void moverEtapa(int pos);
void exibirStatus();
void IHM();

// ======================================================
//                     SETUP
// ======================================================
void setup() {
  Serial.begin(9600);
  delay(50);

  pinMode(SENIND1, INPUT);
  pinMode(SENIND2, INPUT);
  pinMode(SENC1, INPUT);
  pinMode(SENC2, INPUT);

  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);

  pinMode(SOLENOIDE, OUTPUT);

  pinMode(STEP_PIN,OUTPUT);
  pinMode(DIR_PIN,OUTPUT);

  desligarTudo();
  Serial.println("==== SISTEMA PRONTO ====");
}

// ======================================================
//                        LOOP
// ======================================================
void loop(){  

  IHM();

  if(!mesaLigada && detectarPecaEntrada()){
      ligarMesa();
  }

  if(mesaLigada && cicloAtivo){
      cicloIndexador();
  }
}

// ======================================================
//              DETECTA PEÇA NA ENTRADA
// ======================================================
bool detectarPecaEntrada() {

  bool cap = digitalRead(SENC1);        // capacitivo
  bool ind = !digitalRead(SENIND1);      // indutivo

  if(cap == HIGH || ind == HIGH){       // qualquer um detectou → peça presente
      pecas[0] = 1;

      Serial.print("\n📥 Peça detectada na ENTRADA: ");

      if(ind == HIGH){
          Serial.println("METAL");
      } 
      else if(ind == LOW && cap == HIGH){
          Serial.println("NAO METAL");
      }
      else {
          Serial.println("SEM LEITURA");
      }

      return true;
  }

  return false;
}

// ======================================================
//                   LIGAR MESA
// ======================================================
void ligarMesa(){
  Serial.println("\n▶ Iniciando ciclo...");
  mesaLigada = true;
  cicloAtivo = true;
  marcaTempo = millis();
}

// ======================================================
//                   CICLO COMPLETO 
// ======================================================
void cicloIndexador(){
  static int etapa = 0;

  if(millis()-marcaTempo < tempoEtapa) return;
  marcaTempo = millis();

  switch(etapa){
    case 0:
      moverEtapa(1); 
      break; 
    case 1:
      moverEtapa(2); 
      break; 
    case 2:
      moverEtapa(3); 
      break;
    case 3:
      moverEtapa(4);
      break;
    case 4:
      moverEtapa(5);

      // → POSIÇÃO 6 (Saída)
      expulsarPeca();  
      finalizarCiclo(); 
      etapa = 0;
      return;

    default:
      etapa=0; 
      return;
  }

  etapa++;
}

// ======================================================
//       LEITURA DO MATERIAL NA SAÍDA (POSIÇÃO 6)
// ======================================================
int leituraMaterialSaida(){

  bool cap = digitalRead(SENC2);
  bool ind = !digitalRead(SENIND2);

  if(ind == HIGH){
     Serial.println("📦 Saída → METAL");
     return 1;
  } 
  else if(ind == LOW && cap == HIGH){
     Serial.println("📦 Saída → NAO METAL");
     return 2;
  }
  else {
     Serial.println("📦 Saída → SEM PEÇA");
     return 0;
  }
}

// ======================================================
//                EXPULSÃO FINAL
// ======================================================
void expulsarPeca(){

  Serial.println("\n⏳ Checando peça na posição final...");

  int material = leituraMaterialSaida();

  bool cap = digitalRead(SENC2);
  bool ind = !digitalRead(SENIND2);

  if(cap == HIGH || ind == HIGH){
    Serial.println("✔ Peça confirmada na SAÍDA. Acionando solenoide...");

     digitalWrite(SOLENOIDE, HIGH);
     delay(375);                  // tempo de ativação Solenoide
     digitalWrite(SOLENOIDE, LOW);

    Serial.println("➡ Peça expulsa.");
  } 
  else {
    Serial.println("⚠ Nenhuma peça encontrada para expulsar.");
  }
}

// ======================================================
//                   FINALIZAR CICLO
// ======================================================
void finalizarCiclo(){
  Serial.println("\n✔ Ciclo completo. Sistema aguardando nova peça.");
  cicloAtivo=false;
  mesaLigada=false;
  desligarTudo();
}

// ======================================================
//                   MOTOR — 1 POSIÇÃO
// ======================================================
void moverPosicao(){
  digitalWrite(DIR_PIN,LOW);
  for(int i=0;i<PASSOS_POR_POSICAO;i++){
    digitalWrite(STEP_PIN,HIGH); delayMicroseconds(VELOCIDADE_US);
    digitalWrite(STEP_PIN,LOW);  delayMicroseconds(VELOCIDADE_US);
  }
}

// desloca vetor de posição
void moverEtapa(int pos){
  moverPosicao();
  for(int i=5;i>0;i--) pecas[i] = pecas[i-1];
  pecas[0]=0;
  Serial.print("\n➡ Mesa avançou para posição ");
  Serial.println(pos+1);
  exibirStatus();
}

// ======================================================
//              MOSTRAR STATUS NO SERIAL
// ======================================================
void exibirStatus(){
  Serial.print("Estado da mesa: ");
  for(int i=0;i<6;i++){ Serial.print(pecas[i]); Serial.print(" "); }
  Serial.println();
}

// ======================================================
//                       IHM
// ======================================================
void IHM() {
  v15 = digitalRead(SENIND1);
  v16 = digitalRead(SENIND2);
  v17 = digitalRead(SENC1);
  v18 = digitalRead(SENC2);

  if (v15 != 1 && v16 == 1 && v17 == 0 && v18 == 0) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
  } else if (v15 != 1 && v16 != 1 && v17 == 0 && v18 == 0) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
  } else if (v15 == 1 && v16 == 1 && v17 != 0 && v18 == 0) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, HIGH);
  } else if (v15 == 1 && v16 == 1 && v17 == 0 && v18 != 0) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, LOW);
  } else {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
  }
}

// ======================================================
//             RESET EM TODAS SAIDAS/LEDS
// ======================================================
void desligarTudo(){
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);
  digitalWrite(LED3,HIGH);
  digitalWrite(LED4,HIGH);
}

// ======================================================
//           MÓDULOS EM BRANCO PARA UPGRADES FUTUROS
// ======================================================
void moduloExtra1() {}
void moduloExtra2() {}
void moduloExtra3() {}
void moduloExtra4() {}
// ======================================================
// ======================================================