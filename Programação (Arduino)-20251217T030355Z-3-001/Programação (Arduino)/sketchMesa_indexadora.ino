// ======================================================
//                      MESA INDEXADORA 
// ======================================================

// ================================
//      ENTRADAS (DNME B50)
// ================================
#define BTN_INICIO 22         // IN1
#define BTN_SAIDA 23          // IN2
#define SENSOR_INDUTIVO 24    // IN3
#define SENSOR_CAPACITIVO 25  // IN4

// ================================
//        SAIDAS (DNME B50)
// ================================
#define OP1 30
#define OP2 31
#define OP3 32
#define OP4 33
#define OP_SAIDA 34

#define LED_STATUS 13

// ================================
//      MOTOR DE PASSO (A4988)
// ================================
#define STEP_PIN 4
#define DIR_PIN 5
#define ENABLE_PIN 6

// ================================
//            COOLER
// ================================
#define COOLER_PIN 9                  // Pinagem (PWM)
int coolerVelocidade = 180;           // 0–255 (70% velocidade)

// Ajustar o motor real
const int PASSOS_POR_POSICAO = 533;

// posicoes[i]:
//   0 = vazio
//   1 = peça metálica
//   2 = peça não metálica
int posicoes[6] = { 0, 0, 0, 0, 0, 0 };

// ================================
//            CONTROLE
// ================================
bool mesaLigada = false;
bool cicloAtivo = false;

unsigned long tempoAnterior = 0;
const unsigned long intervalo = 3000;

// ======================================================
//                         SETUP
// ======================================================
void setup() {
  Serial.begin(9600);

  pinMode(BTN_INICIO, INPUT_PULLUP);
  pinMode(BTN_SAIDA, INPUT_PULLUP);

  pinMode(SENSOR_INDUTIVO, INPUT);
  pinMode(SENSOR_CAPACITIVO, INPUT);

  pinMode(LED_STATUS, OUTPUT);

  pinMode(OP1, OUTPUT);
  pinMode(OP2, OUTPUT);
  pinMode(OP3, OUTPUT);
  pinMode(OP4, OUTPUT);
  pinMode(OP_SAIDA, OUTPUT);

  // MOTOR DE PASSO
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);

  // COOLER
  pinMode(COOLER_PIN, OUTPUT);
  analogWrite(COOLER_PIN, 0);   // cooler inicia desligado

  desligarSaidas();

  Serial.println("==== SISTEMA PRONTO ====");
}

// ======================================================
//                      LOOP PRINCIPAL
// ======================================================
void loop() {

  if (digitalRead(BTN_INICIO) == LOW && !mesaLigada) {
    ligarMesa();
    delay(300);
  }

  if (mesaLigada && cicloAtivo) {
    executarCiclo();
  }

  if (digitalRead(BTN_SAIDA) == LOW && mesaLigada) {
    expulsarPeca();
    desligarMesa();
    delay(300);
  }
}

// ======================================================
//                     LIGAR A MESA
// ======================================================
void ligarMesa() {
  mesaLigada = true;
  cicloAtivo = true;

  digitalWrite(LED_STATUS, HIGH);

  Serial.println("Mesa ligada.");
  Serial.println("Cooler ativo.");        // <<< mensagem
  analogWrite(COOLER_PIN, coolerVelocidade);  // <<< liga cooler

  Serial.println("Inserindo peça na posição 1…");

  rotacionarMesa();

  posicoes[0] = 1;
  exibirPosicoes();

  tempoAnterior = millis();
}

// ======================================================
//                 CICLO COMPLETO
// ======================================================
void executarCiclo() {
  static int etapa = 0;

  if (millis() - tempoAnterior < intervalo) return;
  tempoAnterior = millis();

  switch (etapa) {

    case 0:
      Serial.println("Posição 2…");
      rotacionarMesa();
      break;

    case 1:
      Serial.println("Posição 3…");
      rotacionarMesa();
      break;

    case 2:
      Serial.println("Posição 4 – leitura de material…");
      rotacionarMesa();
      lerMaterial(3);
      break;

    case 3:
      Serial.println("Posição 5 – leitura de material…");
      rotacionarMesa();
      lerMaterial(4);
      break;

    case 4:
      Serial.println("Posição 6 – expulsão da peça.");
      rotacionarMesa();
      expulsarPeca();
      cicloAtivo = false;
      desligarMesa();
      etapa = 0;
      return;

    default:
      etapa = 0;
      return;
  }

  exibirPosicoes();
  etapa++;
}

// ======================================================
//                   LEITURA DOS SENSORES
// ======================================================
void lerMaterial(int indice) {

  bool metal = digitalRead(SENSOR_INDUTIVO);
  bool naoMetal = digitalRead(SENSOR_CAPACITIVO);

  if (metal) {
    posicoes[indice] = 1;
    Serial.println(" → Peça metálica identificada.");
  } else if (naoMetal) {
    posicoes[indice] = 2;
    Serial.println(" → Peça não metálica identificada.");
  } else {
    Serial.println(" → Sensor não detectou material.");
  }
}

// ======================================================
//                      EXPULSAR A PEÇA
// ======================================================
void expulsarPeca() {
  Serial.println("Expulsando peça…");

  digitalWrite(OP_SAIDA, HIGH);
  delay(800);
  digitalWrite(OP_SAIDA, LOW);

  posicoes[5] = 0;
}

// ======================================================
//                     DESLIGAR A MESA
// ======================================================
void desligarMesa() {
  mesaLigada = false;
  cicloAtivo = false;

  analogWrite(COOLER_PIN, 0);   // <<< cooler desligado

  desligarSaidas();

  Serial.println("Mesa desligada. Aguardando novo ciclo.");
  Serial.println("Cooler desativado");
}

// ======================================================
//              DESLIGAR TODAS AS SAÍDAS
// ======================================================
void desligarSaidas() {
  digitalWrite(LED_STATUS, LOW);

  digitalWrite(OP1, LOW);
  digitalWrite(OP2, LOW);
  digitalWrite(OP3, LOW);
  digitalWrite(OP4, LOW);
  digitalWrite(OP_SAIDA, LOW);
}

// ======================================================
//           MOVER MOTOR UMA POSIÇÃO REAL
// ======================================================
void moverMotorUmaPosicao() {

  digitalWrite(DIR_PIN, HIGH);

  for (int i = 0; i < PASSOS_POR_POSICAO; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(800);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(800);
  }
}

// ======================================================
//                   ROTACIONAR A MESA
// ======================================================
void rotacionarMesa() {

  moverMotorUmaPosicao();

  int ultimo = posicoes[5];
  for (int i = 5; i > 0; i--) {
    posicoes[i] = posicoes[i - 1];
  }
  posicoes[0] = ultimo;

  Serial.println("Mesa rotacionada 1 posição.");
}

// ======================================================
//           MÓDULOS EM BRANCO PARA UPGRADES FUTUROS
// ======================================================
void moduloExtra1() {}
void moduloExtra2() {}
void moduloExtra3() {}
void moduloExtra4() {}

// ======================================================
//                MOSTRAR ESTADO DAS POSIÇÕES
// ======================================================
void exibirPosicoes() {
  Serial.print("Posições: [ ");
  for (int i = 0; i < 6; i++) {
    Serial.print(posicoes[i]);
    Serial.print(" ");
  }
  Serial.println("]");
}
