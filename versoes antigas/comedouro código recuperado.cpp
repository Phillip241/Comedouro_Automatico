//NÃO, ESSE CÓDIGO NÃO TA OBSOLETO, EU SÓ NÃO SEI RENOMEAR O ARQUIVO (PFV DAMASCENO AJUDA COM ISSO)
#include <LiquidCrystal.h>
#include <Servo.h>
#include <EEPROM.h>

#define motor 9
#define botaoligar 11
#define botao_MAIS 13
#define botao_ENTER 12

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int tempoCronometro; // Tempo que vai demorar para cair ração
int qtdRefeicoes = 1; // Controla a quantidade de refeições diárias no menu
int qtdRacao = 30; // Controla a quantidade de ração diária no menu
int racaoPorRefeicao;
int rotacoesMotor; // Quantas vezes o motor roda a cada refeição
int tela = 1; // Variável para controlar a mudança de tela no menu
int S, M, H; // Variáveis do cronômetro
int situacaoBotaoligar = 0; // Verifica se o botão ligar foi apertado
const int posicaoEEPROM = 0; // Armazena a posição do servo para não se mexer ao reiniciar

Servo Servo1;

bool L_botao_MAIS; // Verifica se o botão mais está apertado
bool L_botao_ENTER; // Verifica se o botão enter está apertado

int ligado = 0; // Verifica se o comedouro foi ligado
int sentido = 0; // Variável para mudar o sentido do motor

void limparEEPROM() {
    for (int i = 0; i < 2; i++) {
        EEPROM.write(i, 0);
    }
}

int lerPosicaoEEPROM() {
    int posicao;
    EEPROM.get(posicaoEEPROM, posicao);
    return posicao;
}

void gravarPosicaoEEPROM(int posicao) {
    EEPROM.update(posicaoEEPROM, posicao);
}

void setup() {
    Servo1.attach(motor);

    

    int posicaoInicial = lerPosicaoEEPROM();
    Servo1.write(posicaoInicial);

    pinMode(botao_MAIS, INPUT_PULLUP);
    pinMode(botao_ENTER, INPUT_PULLUP);
    pinMode(botaoligar, INPUT);

    // Primeiros comandos do LCD (tela de iniciação, sempre que o programa for reiniciado isso aparece)
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Comedouro"));
    delay(3000);
    lcd.clear();
}

void loop() {
    if (ligado == 0) { // Condicional para só rodar o programa se o botão ligar for apertado
        lcd.clear();
        situacaoBotaoligar = digitalRead(botaoligar);
        if (situacaoBotaoligar == HIGH) {
            ligado++;
        }
    } else { // Aqui a brincadeira começa
        if (tela == 1) {
            // Primeiro menuzinho no LCD
            lcd.setCursor(0, 0);
            lcd.print(F("RACAO DIARIA:   "));
            lcd.setCursor(0, 1);
            lcd.print(qtdRacao);
            lcd.setCursor(4, 1);
            lcd.print(F("GRAMAS"));

            // O que cada botão faz nessa seção
            L_botao_MAIS = digitalRead(botao_MAIS); // Verifica se o botão mais está apertado
            L_botao_ENTER = digitalRead(botao_ENTER); // Verifica se o botão enter está apertado

            if (L_botao_MAIS) {
                qtdRacao += 30;
                delay(500);
            }
            if (L_botao_ENTER) {
                tela++;
                delay(500);
            }
        } else if (tela == 2) {
            // Segundo menuzinho no LCD
            lcd.setCursor(0, 0);
            lcd.print(F("QTD DE REFEICOES"));
            lcd.setCursor(0, 1);
            lcd.print(qtdRefeicoes);
            if (qtdRefeicoes == 1) {
                lcd.print(F(" REFEICAO"));
            } else {
                lcd.print(F(" REFEICOES"));
            }

            // O que cada botão faz nessa seção
            L_botao_MAIS = digitalRead(botao_MAIS); // Verifica se o botão mais está apertado
            L_botao_ENTER = digitalRead(botao_ENTER); // Verifica se o botão enter está apertado
            if (L_botao_MAIS) {
                qtdRefeicoes++;
                delay(500);
            }
            if (L_botao_ENTER) {
                tela++;
                tempoCronometro = 24 / qtdRefeicoes; // Tempo até cair ração
                racaoPorRefeicao = qtdRacao / qtdRefeicoes;
                for(int i=0; racaoPorRefeicao%10 == 0; i++){
                  ++racaoPorRefeicao;
                }
                rotacoesMotor = racaoPorRefeicao / 10;
                delay(500);
            }
        } else if (tela == 3) {
            // Aqui é padrão, vai contar o tempo até rodar o motor para cair ração
            lcd.setCursor(0, 0);
            lcd.print(F("COMIDA"));
            lcd.setCursor(0, 1);
            lcd.print(F("EM: "));
            lcd.setCursor(6, 1);
            lcd.print(":");
            lcd.setCursor(9, 1);
            lcd.print(":");
            cronometro(0, 0, tempoCronometro); // A variável está em segundos por questão de teste
        }
    }
}

void rodarMotor() { // ESSA FUNÇÃO RODA O MOTOR
    for (int i = 1; i <= rotacoesMotor; i++){
      
      if (sentido == 0) {
          delay(1000);
         Servo1.write(90);
          sentido++;
      } else {
          delay(1000);
          Servo1.write(0);
          sentido--;
      }
    }
    limparEEPROM();
    gravarPosicaoEEPROM(Servo1.read()); // Grava a posição atual do servo
}

void cronometro(int hora, int min, int sec) {
    S--;
    delay(1000);

    if (S < 0) {
        M--;
        S = 59;
    }
    if (M < 0) {
        H--;
        M = 59;
    }
    if (H < 0) {
        H = hora;
        M = min;
        S = sec;
    }
    if (M > 9) {
        lcd.setCursor(7, 1);
        lcd.print(M);
    } else {
        lcd.setCursor(7, 1);
        lcd.print("0");
        lcd.setCursor(8, 1);
        lcd.print(M);
        lcd.setCursor(9, 1);
        lcd.print(":");
    }

    if (S > 9) {
        lcd.setCursor(10, 1);
        lcd.print(S);
    } else {
        lcd.setCursor(10, 1);
        lcd.print("0");
        lcd.setCursor(11, 1);
        lcd.print(S);
        lcd.setCursor(12, 1);
        lcd.print(" ");
    }

    if (H > 9) {
        lcd.setCursor(4, 1);
        lcd.print(H);
    } else {
        lcd.setCursor(4, 1);
        lcd.print("0");
        lcd.setCursor(5, 1);
        lcd.print(H);
        lcd.setCursor(6, 1);
        lcd.print(":");
    }
    if ((H == 0) && (M == 0) && (S == 0)) {
      rodarMotor();   
    }
}
