#wdawhdbawkjd jwlknd lawndlakwn dinclude <EtherCard.h>

//определяем пины к которым мы присоединим нашу MAX7219
#define DIN 8
#define CLK 6
#define CS  7

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
byte Ethernet::buffer[700];
static uint32_t timer;
const char website[] PROGMEM = "ultrasonic.hol.es";
//const char website[] PROGMEM = "93.188.160.127";

static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}


//определяем пины для HC-SR04(датчик)
int trigPin = 5, echoPin = 4;

char dec[] = {0x01, 0x02, 0x03, 0x04};
char number[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
int rezult[] = {1, 1, 1};

void Transfer7219(byte adr, byte data)//Функция вывода числа на индикатор
{
  byte i = 0; //просто переменная-счетчик
  digitalWrite (CS, LOW); //выводим «0» на лапку CS, чтобы начать передачу команды
  delay(10);//коротенькая пауза длиной в 1 такт
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
    // delay(10); //создаем тактовый импульс, посылая сначала 1, а затем спустя короткое время 0 на лапку CLK
    digitalWrite (CLK, HIGH);
    //delay(10);
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

    //  delay(10); //создаем тактовый импульс, посылая сначала 1, а затем спустя короткое время 0 на лапку CLK
    digitalWrite (CLK, HIGH);
    delay(10);
    digitalWrite (CLK, LOW);
    data <<= 1;
  }
  digitalWrite (CS, HIGH); //выводим 1 на CS, завершая таким образом процесс передачи команды
}
void setup() {
  // put your setup code here, to run once:
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(2, INPUT);//тумблер авто/руч
  pinMode(3, INPUT);//тактовая кнопка
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  Serial.begin(9600);
  Transfer7219(0x0F, 0); //очищаем регистр DisplayTest, для перехода в нормальный режим работы
  Transfer7219(0x0C, 1); //записываем «1» в регистр MAX7219, тем самым выводя микросхему из режима Shutdown, обычно ей требуется 250 мкс на включение, но это не мешает принимать команды
  Transfer7219(0x0A, 1); //записываем значение яркости дисплея, в данном случае — «1», почти минимальное значение
  Transfer7219(0x0B, 3); //записываем в ScanLimit максимальное значение «7», что позволяет отображать содержимое всех регистров дисплея от Digit 0 до Digit 7
  Transfer7219(0x09, 0); //ну и наконец отключаем дешифратор, путем записи 0 в регистр Decode Mode
Serial.println("\n[webClient]");
  if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
 
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
 
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
}
void chikl() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int mm = duration / 5.8;
  Serial.println( mm);
  //Serial.println(":" );
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

    Transfer7219(dec[0], number[rezult[0]]); delay(100);
    Transfer7219(dec[1], number[rezult[1]]); delay(100);
    Transfer7219(dec[2], number[rezult[2]]); delay(100);
    Transfer7219(dec[3], number[rezult[3]]);  delay(100);

    Serial.println((String)rezult[3] + ":" + (String)rezult[2] + ":" + (String)rezult[1] + ":" + (String) rezult[0]);
  }
  else {
    Serial.println("otr///");
  }
}
void loop() {

  if (digitalRead(2) == HIGH) {
    chikl();
    delay(100);
  }
  if (digitalRead(2) == LOW) {
    ether.packetLoop(ether.packetReceive());
  if (millis() > timer) {
    timer = millis() + 5000;
    Serial.println();
    Serial.print("<<< REQ ");
    ether.browseUrl(PSTR("/index.php?name=MAMA&data=SAMAYA_LUCHSHAYA&time=1975-05-01"), "", website, my_callback);
  }

  }
}
