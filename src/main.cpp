#include <Arduino.h>
#include <LiquidCrystal.h>

const int BTN_OPT_1 = 5;
const int BTN_OPT_2 = 4;
const int BTN_START = 6;
const int PIN_ZOOTR = 7;
const int PIN_BUZZR = 2;
const int PIN_RESET = 3;

#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978
#define REST 0

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

const int zootropeRotation = 3000; // 3 segundos
float currentStage = 0;
int debugMode = 1;

bool stillPlaying = true;

// Functions
bool IS_BUTTON_PRESSED(int BTN, float stage)
{
  return digitalRead(BTN) == HIGH && currentStage == stage;
}

void WAIT_ZOOTROPE()
{
  digitalWrite(PIN_ZOOTR, HIGH);
  delay(zootropeRotation);
  digitalWrite(PIN_ZOOTR, LOW);
}

void LONG_TEXT_LCD(const char *text, const bool useDelay = false)
{
  int lineCount = 0;
  int lineNumber = 0;
  byte stillProcessing = 1;
  byte charCount = 1;
  lcd.clear();
  lcd.setCursor(0, 0);

  while (stillProcessing)
  {
    if (++lineCount > 20)
    { // Verificando se foram impressos mais de 20 caracteres
      lineNumber += 1;
      lcd.setCursor(0, lineNumber); // Descer o cursor
      lineCount = 1;
    }

    if (lineNumber == 4 && lineCount < 20)
    // Verificando se o texto excedeu o display inteiro
    {
      delay(3000);
      lcd.clear();
      lineNumber = 0;
      lineCount = 0;
      lcd.setCursor(0, lineNumber);
    }

    // Imprime o texto com som 8 bit de diálogo
    lcd.print(text[charCount - 1]);
    tone(PIN_BUZZR, 320, 30);
    delay(200); // Atraso para criar uma sensação de fluídez

    if (!text[charCount])
    { // Verificando se há mais caracteres para imprimir
      stillProcessing = 0;
    }
    charCount += 1;
  }

  if (useDelay == true)
  {
    delay(3000);
  }
}

void PLAY_END_SONG()
{
  // change this to make the song slower or faster
  int tempo = 170;

  // notes of the moledy followed by the duration.
  // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
  // !!negative numbers are used to represent dotted notes,
  // so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
  int melody[] = {
      // game over sound
      NOTE_C5,
      -4,
      NOTE_G4,
      -4,
      NOTE_E4,
      4, // 45
      NOTE_A4,
      -8,
      NOTE_B4,
      -8,
      NOTE_A4,
      -8,
      NOTE_GS4,
      -8,
      NOTE_AS4,
      -8,
      NOTE_GS4,
      -8,
      NOTE_G4,
      8,
      NOTE_D4,
      8,
      NOTE_E4,
      -2,

  };

  // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
  // there are two values per note (pitch and duration), so for each note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;
  byte stillPlaying = 1;
  int divider = 0, noteDuration = 0;

  if (stillPlaying == true)
  {
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2)
    {

      // calculates the duration of each note
      divider = melody[thisNote + 1];
      if (divider > 0)
      {
        // regular note, just proceed
        noteDuration = (wholenote) / divider;
      }
      else if (divider < 0)
      {
        // dotted notes are represented with negative durations!!
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
      }

      // we only play the note for 90% of the duration, leaving 10% as a pause
      tone(PIN_BUZZR, melody[thisNote], noteDuration * 0.9);

      // Wait for the specief duration before playing the next note.
      delay(noteDuration);

      // stop the waveform generation before the next note.
      noTone(PIN_BUZZR);
    }
    stillPlaying = false;
  }
}

void(* resetFunc) (void) = 0;  // declare reset fuction at address 0

// Program
void setup()
{
  pinMode(BTN_OPT_1, INPUT);
  pinMode(BTN_OPT_2, INPUT);
  pinMode(BTN_START, INPUT);
  pinMode(PIN_ZOOTR, OUTPUT);
  pinMode(PIN_BUZZR, OUTPUT);
  pinMode(PIN_RESET, INPUT_PULLUP);

  Serial.begin(9600);
  digitalWrite(PIN_RESET, HIGH);

  // Mostrar tela de apresentação
  lcd.begin(20, 4);
  lcd.clear();

  lcd.setCursor(1, 1);
  lcd.print("Zootropio - v1");
  lcd.setCursor(1, 2);
  lcd.print("Maker Space");
  delay(3000);
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Colegio");
  lcd.setCursor(1, 2);
  lcd.print("Brasilia");
  delay(3000);
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Aperte START");
  lcd.setCursor(4, 2);
  lcd.print("para comecar");
  noTone(PIN_BUZZR);

  currentStage = 2.0;
}

void loop()
{
  if (digitalRead(BTN_OPT_1) == HIGH && digitalRead(BTN_OPT_2) == HIGH)
  {
    lcd.clear();
    lcd.print("Reiniciando...");
    delay(1000);
    digitalWrite(PIN_RESET, LOW);
  }

  if (IS_BUTTON_PRESSED(BTN_START, 0))
  {
    lcd.clear();
    lcd.setCursor(0, 0);

    LONG_TEXT_LCD("Coloque a fita 1");
    currentStage = 1.1;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.1))
  {
    lcd.clear();

    // iniciar o zootrópio
    WAIT_ZOOTROPE();

    lcd.setCursor(0, 0);
    LONG_TEXT_LCD("Em uma cidade distante, havia uma meninachamada Sofia.");
    delay(3000);
    lcd.clear();
    LONG_TEXT_LCD("Coloque a fita 2");
    currentStage = 1.2;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.2))
  {
    lcd.clear();
    WAIT_ZOOTROPE();

    LONG_TEXT_LCD("Enquanto caminhava pela cidade, Sofia encontra um lixo no chao", true);
    lcd.clear();
    LONG_TEXT_LCD("Coloque a fita 3");
    currentStage = 1.3;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.3))
  {
    lcd.clear();
    WAIT_ZOOTROPE();

    LONG_TEXT_LCD("Ao notar isso, Sofiadecide:", true);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1: reciclar lixo");
    lcd.setCursor(0, 1);
    lcd.print("2: jogar o lixo no");
    lcd.setCursor(0, 2);
    lcd.print("rio");
    currentStage = 1.4;
  }

  if (IS_BUTTON_PRESSED(BTN_OPT_1, 1.4))
  {
    // Sofia decide reciclar o lixo
    lcd.clear();
    LONG_TEXT_LCD("Coloque a fita 4.1");
    currentStage = 1.51;
  }

  if (IS_BUTTON_PRESSED(BTN_OPT_2, 1.4))
  {
    // Sofia decide jogar o lixo fora
    lcd.clear();
    LONG_TEXT_LCD("Coloque a fita 4.2");
    currentStage = 1.52;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.51))
  {
    lcd.clear();
    WAIT_ZOOTROPE();

    LONG_TEXT_LCD("Apos recilar o lixo,Sofia percebe que poderia fazer mais porsua cidade", true);
    LONG_TEXT_LCD("Coloque a fita 5");
    currentStage = 1.6;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.52))
  {
    lcd.clear();
    WAIT_ZOOTROPE();

    LONG_TEXT_LCD("Depois de jogar o lixo no rio, Sofia e vista pelo guarda querepreende ela por   ter jogado o lixo no rio", true);
    LONG_TEXT_LCD("O guarda chama a maede Sofia e ela explica que, o que tinha feito era errado", true);
    LONG_TEXT_LCD("Ela fala de como os peixes sofrem com o lixo em lagos, tartarugas nomar com      canudos", true);
    LONG_TEXT_LCD("Depois dessa conversa, Sofia fica ressentida com sua atitudee decide mudar      com um projeto", true);
    LONG_TEXT_LCD("Coloque a fita 5");
    currentStage = 1.6;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.6))
  {
    lcd.clear();
    WAIT_ZOOTROPE();

    LONG_TEXT_LCD("Sofia decide chamar seus amigos para construirem um centro de reciclagem", true);
    LONG_TEXT_LCD("Eles decidem que irao reciclar os produtos e fazerem objetos dedecoracao e      utilidade", true);
    LONG_TEXT_LCD("Coloque a fita 6");
    currentStage = 1.7;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.7))
  {
    lcd.clear();
    WAIT_ZOOTROPE();

    LONG_TEXT_LCD("Sofia e seus amigos criam vassoura com  garrafas PET e esta tuas de plastico    reutilizado", true);
    LONG_TEXT_LCD("Coloque a fita 7");
    currentStage = 1.8;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.8))
  {
    lcd.clear();
    WAIT_ZOOTROPE();

    LONG_TEXT_LCD("Com isso, Sofia podeperceber o quao importante e cuidar do nosso planeta.       Seja como a Sofia", true);
    LONG_TEXT_LCD("Pratique a reciclagem e contribua com a sustentabilidade do planeta", true);
    LONG_TEXT_LCD("Coloque a fita 7");
    currentStage = 1.9;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 1.9))
  {
    lcd.clear();
    WAIT_ZOOTROPE();

    LONG_TEXT_LCD("Compartilhe o projeto nas redes sociais", true);
    LONG_TEXT_LCD("@colegiobrasilia.oficial", true);
    lcd.clear();
    LONG_TEXT_LCD("Projetistas:         Guilherme Galaverna Lucas Campos", true);
    delay(8000);
    currentStage = 2.0;
  }

  if (currentStage == 2.0)
  {
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("FIM");
    PLAY_END_SONG();
    currentStage = 2.1;
  }

  if (IS_BUTTON_PRESSED(BTN_START, 2.1)) {
    resetFunc();
  }
}
