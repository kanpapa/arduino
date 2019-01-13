//
// 2364 Mask ROM Reader PROGRAM
//
// 2019.01.13 @kanpapa
//
//                     2364
unsigned D0=22;     // 9
unsigned D1=23;     // 10
unsigned D2=24;     // 11
unsigned D3=25;     // 13
unsigned D4=26;     // 14
unsigned D5=27;     // 15
unsigned D6=28;     // 16
unsigned D7=29;     // 17

unsigned AD0=30;    // 8
unsigned AD1=31;    // 7
unsigned AD2=32;    // 6
unsigned AD3=33;    // 5
unsigned AD4=34;    // 4
unsigned AD5=35;    // 3
unsigned AD6=36;    // 2
unsigned AD7=37;    // 1
unsigned AD8=38;    // 23
unsigned AD9=39;    // 22
unsigned AD10=40;   // 19
unsigned AD11=41;   // 18
unsigned AD12=42;   // 21

unsigned CE=47;     // 20 Chip Enable
                    // 24 Vcc
                    // 12 GND

unsigned DATA[]={D0,D1,D2,D3,D4,D5,D6,D7};
unsigned ADDR[]={AD0,AD1,AD2,AD3, AD4,AD5,AD6,AD7, AD8,AD9,AD10,AD11, AD12};

void setAddress(unsigned int addr) {
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

unsigned memRead(unsigned int addr) {
  digitalWrite(CE, HIGH);  // CE = High
  unsigned result = 0;
  setAddress(addr);
  digitalWrite(CE, LOW);  // CE = Low
  delayMicroseconds(1); // Wait 1micro sec
  for (int i = 7; i >= 0; i--) {
    result += result;
    if (digitalRead(DATA[i]) == HIGH) result |= 1;
  }
  digitalWrite(CE, HIGH);  // CE = High
  return result;
}

void hexprint2(unsigned char val){
  if (val < 16){
    Serial.print("0");
  }
  Serial.print(val, HEX);
}

void hexprint4(unsigned int val){
   unsigned char hi_val = val / 256;
   hexprint2(hi_val);
   unsigned char low_val = val % 256;
   hexprint2(low_val);
}

void setup() {
  // Initalize serial
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
    
  // set PINMMODE
  pinMode(CE, OUTPUT);
  digitalWrite(CE, HIGH);
  for (int i=0; i<13; i++) pinMode(ADDR[i], OUTPUT); // address
  for (int i=0; i<8; i++)  pinMode(DATA[i], INPUT);   // data

  // Output startup message
  Serial.println("2364 Mask ROM Reader PROGRAM");

  // Read memory 0x0000-0x1FFF (8KByte*8bit=64Kbit)
  unsigned int start_adrs = 0x0000;
  unsigned int end_adrs = 0x2000;
  unsigned int max_count = 16;
  unsigned int total_count = end_adrs - start_adrs;
  Serial.print(total_count);
  Serial.println("Bytes");

  delay(10000);

  // Output Intel HEX Format
  unsigned int read_adrs = start_adrs;

  while(total_count > 0){
    int chksum = 0;
    int count = 0;

    // RECORD MARK
    Serial.print(":");
    
    // LOAD REDLEN
    if (total_count >= 16){
      count = max_count;
    } else {
      count = total_count;
    }    
    hexprint2(count);
    chksum += count;

    // OFFSET
    hexprint4(read_adrs);
    chksum += (read_adrs / 256);
    chksum += (read_adrs % 256);

    // RECTYP
    Serial.print("00");

    // DATA
    for (int i = 0 ; i < count ; i++){
      // Read data
      unsigned char read_data = memRead(read_adrs + i);
      hexprint2(read_data);
      chksum += read_data;    
    }
   
    // CHKSUM
    chksum = (~chksum + 1 ) & 0xff;
    hexprint2(chksum);
    Serial.print("\n");

    //
    read_adrs += count;
    total_count = total_count - count;
  }

  // EOF
  Serial.println(":00000001FF");  
}

void loop() {
}
