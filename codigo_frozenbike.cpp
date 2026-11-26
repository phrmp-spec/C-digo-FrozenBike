
#include <LiquidCrystal.h>
#include <DHT.h>

// PINOS DOS COMPONENTES
#define DHTPIN 6
#define DHTTYPE DHT11
#define BOTAO 7
#define RELE 8
#define BACKLIGHT 9

// CONFIGURACAO DO LCD
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// CONFIGURACAO DO SENSOR
DHT dht(DHTPIN, DHTTYPE);

// LIMITES DE TEMPERATURA
const float TEMP_LIGA = 25.0;
const float TEMP_DESLIGA = 23.0;

// CONFIGURACAO DO RELE
const int RELE_LIGADO = LOW;
const int RELE_DESLIGADO = HIGH;

// CONTROLE DO SISTEMA
bool sistemaAtivo = false;
bool refrigeracaoLigada = false;

// INTERVALO DE LEITURA
unsigned long ultimaLeitura = 0;
const unsigned long INTERVALO = 2000;

void desligarSistema() {

  // DESLIGA A REFRIGERACAO
  refrigeracaoLigada = false;
  digitalWrite(RELE, RELE_DESLIGADO);

  // DESLIGA A ILUMINACAO DO LCD
  digitalWrite(BACKLIGHT, LOW);

  // APAGA O DISPLAY
  lcd.noDisplay();

  sistemaAtivo = false;
}

void setup() {

  // CONFIGURACAO DOS PINOS
  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(RELE, OUTPUT);
  pinMode(BACKLIGHT, OUTPUT);

  // INICIA COM TUDO DESLIGADO
  digitalWrite(RELE, RELE_DESLIGADO);
  digitalWrite(BACKLIGHT, LOW);

  // INICIALIZACAO DO SENSOR
  dht.begin();

  // INICIALIZACAO DO LCD
  lcd.begin(16, 2);
  lcd.noDisplay();

  Serial.begin(9600);
}

void loop() {

  // VERIFICA O BOTAO
  bool botaoPressionado = (digitalRead(BOTAO) == LOW);

  // SE O BOTAO NAO ESTIVER PRESSIONADO
  if (!botaoPressionado) {

    if (sistemaAtivo) {
      desligarSistema();
    }

    return;
  }

  // SE O BOTAO ESTIVER PRESSIONADO
  if (!sistemaAtivo) {

    sistemaAtivo = true;

    // LIGA A ILUMINACAO DO LCD
    digitalWrite(BACKLIGHT, HIGH);

    // LIGA O DISPLAY
    lcd.display();
    lcd.clear();

    // MENSAGEM INICIAL
    lcd.setCursor(0, 0);
    lcd.print("SISTEMA DE");

    lcd.setCursor(0, 1);
    lcd.print("REFRIGERACAO");

    delay(1000);

    // FORCA UMA LEITURA IMEDIATA
    ultimaLeitura = millis() - INTERVALO;
  }

  // CONTROLE DO INTERVALO DE LEITURA
  if (millis() - ultimaLeitura >= INTERVALO) {

    ultimaLeitura = millis();

    // LEITURA DO SENSOR
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();

    // VERIFICA ERRO DO SENSOR
    if (isnan(temperatura) || isnan(umidade)) {

      refrigeracaoLigada = false;
      digitalWrite(RELE, RELE_DESLIGADO);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro no sensor!");

      lcd.setCursor(0, 1);
      lcd.print("Verifique DHT11");

      Serial.println("Erro na leitura do DHT11!");

      return;
    }

    // CONTROLE AUTOMATICO DA TEMPERATURA
    if (temperatura >= TEMP_LIGA) {

      refrigeracaoLigada = true;

    }

    else if (temperatura <= TEMP_DESLIGA) {

      refrigeracaoLigada = false;

    }

    // ACIONAMENTO DO RELE
    if (refrigeracaoLigada) {

      digitalWrite(RELE, RELE_LIGADO);

    }

    else {

      digitalWrite(RELE, RELE_DESLIGADO);

    }

    // EXIBICAO DA TEMPERATURA
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatura, 1);
    lcd.print((char)223);
    lcd.print("C   ");

    // EXIBICAO DA UMIDADE
    lcd.setCursor(0, 1);
    lcd.print("Umid: ");
    lcd.print(umidade, 1);
    lcd.print("%    ");

    // MONITOR SERIAL
    Serial.print("Temperatura: ");
    Serial.print(temperatura);

    Serial.print(" C | Umidade: ");
    Serial.print(umidade);

    Serial.print("% | Refrigeracao: ");

    if (refrigeracaoLigada) {
      Serial.println("LIGADA");
    }

    else {
      Serial.println("DESLIGADA");
    }
  }
}

