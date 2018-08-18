//
// HM6264 TEST PROGRAM
//
// 2018.08.18 @kanpapa
//
//                     HM6264
unsigned D0=22;     // 11
unsigned D1=23;     // 12
unsigned D2=24;     // 13
unsigned D3=25;     // 15
unsigned D4=26;     // 16
unsigned D5=27;     // 17
unsigned D6=28;     // 18
unsigned D7=29;     // 19

unsigned AD0=30;    // 10
unsigned AD1=31;    // 9
unsigned AD2=32;    // 8
unsigned AD3=33;    // 7
unsigned AD4=34;    // 6
unsigned AD5=35;    // 5
unsigned AD6=36;    // 4
unsigned AD7=37;    // 3
unsigned AD8=38;    // 25
unsigned AD9=39;    // 24
unsigned AD10=40;   // 21
unsigned AD11=41;   // 23
unsigned AD12=42;   // 2

unsigned WE=46;     // 27 Write Enable
unsigned OE=47;     // 22 Output Enale
                    // 20 CS1 = LOW
                    // 26 CS2 = HIGH
                    // 28 Vcc
                    // 14 Vss
                    // 1  NC

unsigned DATA[]={D0,D1,D2,D3,D4,D5,D6,D7};
unsigned ADDR[]={AD0,AD1,AD2,AD3, AD4,AD5,AD6,AD7, AD8,AD9,AD10,AD11, AD12};

void setAddress(unsigned addr) {
  if (addr & 0x1000) digitalWrite(AD12, HIGH); else digitalWrite(AD12, LOW);
  if (addr & 0x800) digitalWrite(AD11, HIGH); else digitalWrite(AD11, LOW);
  if (addr & 0x400) digitalWrite(AD10, HIGH); else digitalWrite(AD10, LOW);
  if (addr & 0x200) digitalWrite(AD9, HIGH); else digitalWrite(AD9, LOW);
  if (addr & 0x100) digitalWrite(AD8, HIGH); else digitalWrite(AD8, LOW);
  if (addr & 0x80) digitalWrite(AD7, HIGH); else digitalWrite(AD7, LOW);
  if (addr & 0x40) digitalWrite(AD6, HIGH); else digitalWrite(AD6, LOW);
  if (addr & 0x20) digitalWrite(AD5, HIGH); else digitalWrite(AD5, LOW);
  if (addr & 0x10) digitalWrite(AD4, HIGH); else digitalWrite(AD4, LOW);
  if (addr & 0x08) digitalWrite(AD3, HIGH); else digitalWrite(AD3, LOW);
  if (addr & 0x04) digitalWrite(AD2, HIGH); else digitalWrite(AD2, LOW);
  if (addr & 0x02) digitalWrite(AD1, HIGH); else digitalWrite(AD1, LOW);
  if (addr & 0x01) digitalWrite(AD0, HIGH); else digitalWrite(AD0, LOW);
  return;
}

void memWrite(unsigned addr, unsigned data) {
  digitalWrite(OE, HIGH);  // OE = High
  setAddress(addr);
  for (int i=0; i<8; i++) pinMode(DATA[i], OUTPUT);
  if (data & 0x80) digitalWrite(D7, HIGH); else digitalWrite(D7, LOW);
  if (data & 0x40) digitalWrite(D6, HIGH); else digitalWrite(D6, LOW);
  if (data & 0x20) digitalWrite(D5, HIGH); else digitalWrite(D5, LOW);
  if (data & 0x10) digitalWrite(D4, HIGH); else digitalWrite(D4, LOW);
  if (data & 0x08) digitalWrite(D3, HIGH); else digitalWrite(D3, LOW);
  if (data & 0x04) digitalWrite(D2, HIGH); else digitalWrite(D2, LOW);
  if (data & 0x02) digitalWrite(D1, HIGH); else digitalWrite(D1, LOW);
  if (data & 0x01) digitalWrite(D0, HIGH); else digitalWrite(D0, LOW);
  delayMicroseconds(1); // Wait 1micro sec
  digitalWrite(WE, LOW);  // WE = Low
  delayMicroseconds(1); // Wait 1micro sec
  digitalWrite(WE, HIGH);  // WE = High
  return;
}

unsigned memRead(unsigned addr) {
  digitalWrite(OE, HIGH);  // OE = High
  unsigned result = 0;
  setAddress(addr);
  for (int i=0; i<8; i++) pinMode(DATA[i], INPUT);
  digitalWrite(OE, LOW);  // OE = Low
  delayMicroseconds(1); // Wait 1micro sec
  for (int i = 7; i >= 0; i--) {
    result += result;
    if (digitalRead(DATA[i]) == HIGH) result |= 1;
  }
  digitalWrite(OE, HIGH);  // OE = High
  return result;
}

void setup() {
  // Initalize serial --------------------------------------------------------------------
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  randomSeed(analogRead(0));
    
  // set PINMMODE
  pinMode(OE, OUTPUT);
  digitalWrite(OE, HIGH);
  pinMode(WE, OUTPUT);
  digitalWrite(WE, HIGH);
  for (int i=0; i<13; i++) pinMode(ADDR[i], OUTPUT); // address
  for (int i=0; i<8; i++) pinMode(ADDR[i], INPUT);   // data

  Serial.println("HM6264 TEST PROGRAM");
  delay(10000);
  Serial.println("START!!");
  
  // memory check
  for (unsigned testadrs = 0x1F00; testadrs < 0x2000 ; testadrs++){
    // display address
    Serial.print(testadrs, HEX);
    Serial.print(":");

    // display memory
    Serial.print(memRead(testadrs), HEX);
    
    // random write memory
    Serial.print("  Write data: ");
    unsigned rnddata = random(256);
    Serial.print(rnddata, HEX);
    memWrite(testadrs, rnddata);

    // display memory
    Serial.print("  Read data: ");
    unsigned readdata = memRead(testadrs);
    Serial.print(readdata, HEX);

    // match?
    if (rnddata == readdata) {
      Serial.println("  OK");
    } else {
      Serial.println("  ERROR!!");
    }
  }
}

void loop() {
}
