
//определяем пины к которым мы присоединим нашу MAX7219
#define DIN 8
#define CLK 6
#define CS  7
#define AUTO 5
#define AUTO_LED A3
#define TAKT 4
#define TAKT_LED_ON A2
#define TAKT_LED A1

//определяем пины для HC-SR04(датчик)
int trigPin = 3, echoPin = 2;

char dec[] = {0x01, 0x02, 0x03, 0x04};
char number[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
char number_dp[] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF};
int rezult[] = {1, 1, 1, 1};

void Transfer7219(byte adr, byte data)//Функция вывода числа на индикатор
{
  byte i = 0; //просто переменная-счетчик
  digitalWrite (CS, LOW); //выводим «0» на лапку CS, чтобы начать передачу команды
  //delay(10);//коротенькая пауза длиной в 1 такт
  for (i = 0; i < 8; i++) //выгружаем байт адреса
  {
    if ((adr & 0x80) == 0x80) // определяем, что выводить
    {
      digitalWrite (DIN, HIGH); // единицу
    }
    else
    {
      digitalWrite (DIN, LOW); // или ноль
    }
    //delay(10); //создаем тактовый импульс, посылая сначала 1, а затем спустя короткое время 0 на лапку CLK
    digitalWrite (CLK, HIGH);
    delay(10);
    digitalWrite (CLK, LOW);
    adr <<= 1;
  }

  for (i = 0; i < 8; i++) //выводим байт данных
  { //тут всё то же самое, что и при выводе адреса. Если хотите, то можно вынести этот кусок кода в отдельную функцию
    if ((data & 0x80) == 0x80)
    {
      digitalWrite (DIN, HIGH); // единицу
    }
    else
    {
      digitalWrite (DIN, LOW); // или ноль
    }

    // delay(10); //создаем тактовый импульс, посылая сначала 1, а затем спустя короткое время 0 на лапку CLK
    digitalWrite (CLK, HIGH);
    delay(10);
    digitalWrite (CLK, LOW);
    data <<= 1;
  }
  digitalWrite (CS, HIGH); //выводим 1 на CS, завершая таким образом процесс передачи команды
}
void setup() {

  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(CS,  OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(AUTO, INPUT);//тумблер авто/руч
  pinMode(TAKT, INPUT);//тактовая кнопка

  digitalWrite(AUTO, LOW);
  digitalWrite(TAKT, LOW);

  Serial.begin(9600);
  Transfer7219(0x0F, 0); //очищаем регистр DisplayTest, для перехода в нормальный режим работы
  Transfer7219(0x0C, 1); //записываем «1» в регистр MAX7219, тем самым выводя микросхему из режима Shutdown, обычно ей требуется 250 мкс на включение, но это не мешает принимать команды
  Transfer7219(0x0A, 1); //записываем значение яркости дисплея, в данном случае — «1», почти минимальное значение
  Transfer7219(0x0B, 3); //записываем в ScanLimit максимальное значение «7», что позволяет отображать содержимое всех регистров дисплея от Digit 0 до Digit 7
  Transfer7219(0x09, 0); //ну и наконец отключаем дешифратор, путем записи 0 в регистр Decode Mode
}
void chikl() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int mm = duration / 5.8;

  if (mm > 0) {
    if (mm > 999) {
      rezult[3] = mm / 1000;
    } else {
      rezult[3] = 0;
    }
    if (mm > 99) {
      rezult[2] = mm / 100 % 10;
    } else {
      rezult[2] = 0;
    }
    if (mm > 9) {
      rezult[1] = mm % 100 / 10;
    } else {
      rezult[1] = 0;
    }
    rezult[0] = mm % 10;

    Transfer7219(dec[0], number[rezult[0]]);
    Transfer7219(dec[1], number_dp[rezult[1]]);
    Transfer7219(dec[2], number[rezult[2]]);
    Transfer7219(dec[3], number[rezult[3]]);
    Serial.println((String)rezult[3] + ":" + (String)rezult[2] + ":" + (String)rezult[1] + ":" + (String) rezult[0]);
  }
  else {
    Serial.println("Ошибка измерения");
  }
}
void loop() {
  if (digitalRead(AUTO) == HIGH) {
    //digitalWrite(AUTO_LED,HIGH);
    analogWrite(AUTO_LED, 1024);
    analogWrite(TAKT_LED_ON, 0);
    analogWrite(TAKT_LED, 0);
    chikl();
  }
  else if (digitalRead(AUTO) == LOW) {
    analogWrite(AUTO_LED, 0);
    analogWrite(TAKT_LED_ON, 1024);
    if (digitalRead(TAKT) == HIGH) {
      analogWrite(TAKT_LED, 1024);
      chikl();
    }
    else {
      analogWrite(TAKT_LED, 0);
    }
  }
}
