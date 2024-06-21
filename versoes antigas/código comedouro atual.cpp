#include <LiquidCrystal.h>
#include <Servo.h>
#include <EEPROM.h>

#define motor 9
#define botao_MENOS 11
#define botao_MAIS 10
#define botao_ENTER 8

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
Servo Servo1;

double tempoHora, tempoMinuto, tempoSegundo; // Tempo que vai demorar para cair ração
int qtdRefeicoes = 1; // Controla a quantidade de refeições diárias no menu
int qtdRacao = 10; // Controla a quantidade de ração diária no menu
int racaoPorRefeicao;
int rotacoesMotor; // Quantas vezes o motor roda a cada refeição
int tela = 1; // Variável para controlar a mudança de tela no menu
int S, M, H; // Variáveis do cronômetro
bool sentido = true; // Variável para mudar o sentido do motor
const int posicaoEEPROM = 0; // Armazena a posição do servo para não se mexer ao reiniciar

bool L_botao_MAIS; // Verifica se o botão mais está apertado
bool L_botao_ENTER; // Verifica se o botão enter está apertado
bool L_botao_MENOS; // Verifica se o botão enter está apertado


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
    pinMode(botao_MENOS, INPUT_PULLUP);

    // Primeiros comandos do LCD (tela de iniciação, sempre que o programa for reiniciado isso aparece)
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Comedouro"));
    delay(3000);
    lcd.clear();
}

void loop() {
    if (tela == 1) {
        // Primeiro menuzinho no LCD
        lcd.setCursor(0, 0);
        lcd.print(F("RACAO DIARIA:   "));
        lcd.setCursor(0, 1);
        lcd.print(qtdRacao);
        lcd.print(F(" GRAMAS"));

        // O que cada botão faz nessa seção
        L_botao_MAIS = digitalRead(botao_MAIS); // Verifica se o botão mais está apertado
        L_botao_ENTER = digitalRead(botao_ENTER); // Verifica se o botão enter está apertado
        L_botao_MENOS = digitalRead(botao_MENOS);

        if (L_botao_MAIS) {
            qtdRacao += 10;
            delay(500);
        }
        if (L_botao_MENOS) {
            if(qtdRacao > 10){
              qtdRacao -= 10;
              delay(500);
            }
        }
        if (L_botao_ENTER) {
          lcd.clear();  
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
        L_botao_MENOS = digitalRead(botao_MENOS); // Verifica se o botão menos está apertado
        if (L_botao_MAIS) {
            qtdRefeicoes++;
            delay(500);
        }
        if (L_botao_MENOS) {
          if(qtdRefeicoes > 1){
            qtdRefeicoes--;
            delay(500);
          }
        }
        if (L_botao_ENTER) {
            tela++;
            tempoHora = 24 / (double)qtdRefeicoes; // Tempo até cair ração
            tempoMinuto = (tempoHora - (int)tempoHora) * 60;
            tempoSegundo = (tempoMinuto - (int)tempoMinuto) * 60;
            racaoPorRefeicao = qtdRacao / qtdRefeicoes;
            for (int i = 0; racaoPorRefeicao % 10 == 0; i++) {
                ++racaoPorRefeicao;
            }
            rotacoesMotor = racaoPorRefeicao / 10;
          	rodarMotor();
            lcd.clear();
          
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
        cronometro(tempoHora, tempoMinuto, tempoSegundo); // A variável está em segundos por questão de teste
    }
}

void rodarMotor() { // ESSA FUNÇÃO RODA O MOTOR
    for (int i = 1; i <= rotacoesMotor; i++) {
        if (sentido) {
            delay(1000);
            Servo1.write(Servo1.read()+90);
        } else {
            delay(1000);
            Servo1.write(Servo1.read()-90);
        }
        if(Servo1.read() == 180){
            sentido = false;
        }
        else if(Servo1.read() == 0){
            sentido = true;
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
