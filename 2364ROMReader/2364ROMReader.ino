//
// 2364 Mask ROM Reader PROGRAM
//
// 2019.01.13 @kanpapa
//
//                2364
#define D0 22     // 9
#define D1 23     // 10
#define D2 24     // 11
#define D3 25     // 13
#define D4 26     // 14
#define D5 27     // 15
#define D6 28     // 16
#define D7 29     // 17

#define AD0 30    // 8
#define AD1 31    // 7
#define AD2 32    // 6
#define AD3 33    // 5
#define AD4 34    // 4
#define AD5 35    // 3
#define AD6 36    // 2
#define AD7 37    // 1
#define AD8 38    // 23
#define AD9 39    // 22
#define AD10 40   // 19
#define AD11 41   // 18
#define AD12 42   // 21

#define CE 47  // 20 Chip Enable
               // 24 Vcc
               // 12 GND

int DATA[]={D0,D1,D2,D3,D4,D5,D6,D7};
int ADDR[]={AD0,AD1,AD2,AD3, AD4,AD5,AD6,AD7, AD8,AD9,AD10,AD11, AD12};

void setAddress(int addr) {
  for (int bitno = 0 ; bitno < 16 ; bitno++){
    if (bitRead(addr, bitno) == 1) {
      digitalWrite(ADDR[bitno], HIGH);
    } else {
      digitalWrite(ADDR[bitno], LOW);
    }
  }
  return;
}

byte memRead(int addr) {
  digitalWrite(CE, HIGH);  // CE = High
  byte result = 0;
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

void hexprint2(byte val){
  if (val < 16){
    Serial.print("0");
  }
  Serial.print(val, HEX);
}

void hexprint4(int val){
   hexprint2(highByte(val));
   hexprint2(lowByte(val));
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
  for (int i=0; i<8; i++)  pinMode(DATA[i], INPUT);  // data

  // Output startup message
  Serial.println("2364 Mask ROM Reader PROGRAM");

  // Read memory 0x0000-0x1FFF (8KByte*8bit=64Kbit)
  int start_adrs = 0x0000;
  int end_adrs = 0x2000;
  int max_count = 16;
  int total_count = end_adrs - start_adrs;
  Serial.print(total_count);
  Serial.println("Bytes");

  delay(10000);

  // Output Intel HEX Format
  int read_adrs = start_adrs;

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
    chksum += highByte(read_adrs);
    chksum += lowByte(read_adrs);

    // RECTYP
    Serial.print("00");

    // DATA
    for (int i = 0 ; i < count ; i++){
      // Read data
      byte read_data = memRead(read_adrs + i);
      hexprint2(read_data);
      chksum += read_data;    
    }
   
    // CHKSUM
    chksum = lowByte(~chksum + 1 );
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
