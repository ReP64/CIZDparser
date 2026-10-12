/*  

    author      : R. Paats
    last edit   : 27-07-2026   11:40
    filename    : CIZDparser.ino
    processor   : Attiny1614 20MHz
    description : send data from a CI200Sc to a zebra printer ZD420
                  the data from the CI200SC format is BP-DT4 Form0
                  the programming for de ZD420 is ZPL format
    Sketch uses : 3687 bytes
    Global var  : 443 bytes
    
    CI200SC settings
    F27 0           8N1
    F28 4           9600
    F29 1   F33 0   0=prt 1=pc
    F30 0   F34 0   22byte
    F31 1   F35 1   1 stream
    F40 3           BP-DT4 format
    F41 0         ` FORM0
    F42 0           manual print
    F43 1           line feed

    BP-DT4 format
    V00   NETWEIGHT   7
    V01   UNIT        2
    V02   GROSS       7
    V03   TARE        7
    V04   DATE        10
    V05   TIME        8
    V06   ID          2
    V07   COUNT       3
    V08   NET:BARCODE 6
    V09   TOTAL NET   9
*/

#include <SoftwareSerial.h>
SoftwareSerial RS232(PIN_PA2, PIN_PA1);  // RX, TX

const char a[] PROGMEM = "^XA^PW580^LL400^PQ1^FO180,80^A0N,60,60^FD";
const char b[] PROGMEM = "^FS^FO100,250^A0N,30,30^FD";
const char c[] PROGMEM = "^FS^FO320,250^A0N,30,30^FD";
const char d[] PROGMEM = "^FS^XZ";
const char e[] PROGMEM = "^XZ^XA^XZ";

const char startMarker = 0x3F;
const char endMarker = 0x0D;
const byte numChars = 100;

char receivedChars[numChars];
char zd420[160];
char dat[11];
char tyd[9];

char* V00;
char* V01;
char* V02;
char* V03;
char* V04;
char* V05;
char* V06;
char* V07;
char* V08;
char* V09;

bool newData = false;
bool kg = false;

void setup() {
  Serial.begin(9600);
  RS232.begin(9600);
  delay(1000);                            // wait for printer boot
  //RS232.print("CIZDparser V1.0");
  RS232.print("^XZ");
  RS232.flush(); 
}

void loop() {
  receiveWithStartAndEndMarkers();
  if (newData) {
    parsedData();
    showParsedData();
    newData = false;
  }
}

void receiveWithStartAndEndMarkers() {
  static bool receiving = false;
  static byte idx = 0;

  while (Serial.available() && !newData) {  // is there data and newdata = false
    char c = Serial.read();                 // c is incomming data
    if (receiving) {                        // start mark detected
      if (c != endMarker) {                 // no end mark
        if (idx < numChars - 1) {           // index < 99
          receivedChars[idx++] = c;         // chars[index] = incoming data
        }
      } else {                              // receiving false
        receivedChars[idx] = '\0';          // after end mark + 0
        receiving = false;                  // receiving = false
        idx = 0;                            // index = 0
        newData = true;                     // ready to receive newdata
      }
    } else if (c == startMarker) {          // c=startmark ?
      receiving = true;                     // start receiving
    }
  }
}                        

void parsedData() {
  V00 = strtok(receivedChars, "\r\n");
  V01 = strtok(NULL, "\r\n");
  V02 = strtok(NULL, "\r\n");
  V03 = strtok(NULL, "\r\n");
  V04 = strtok(NULL, "\r\n");
  V05 = strtok(NULL, "\r\n");
  V06 = strtok(NULL, "\r\n");
  V07 = strtok(NULL, "\r\n");
  V08 = strtok(NULL, "\r\n");
  V09 = strtok(NULL, "\r\n");
  if (kg==false) {V01[0] = 'g';         // only when the unit is grams
                  V01[1] = ' ';}                  
}

void fillSpaces() {
// reverse date 
  if (V05[0] == 0x20) { V05[0] = '0';}
  if (V05[4] == 0X3A) { V05[8] = V05[7];  //if : then shift right
                        V05[7] = V05[6];
                        V05[6] = V05[5];
                        V05[5] = V05[4];
                        V05[4] = V05[3];
                        V05[3] ='0';
                        if (V05[6] == 0x20) { V05[6] = '0';}
                      }
  if (V05[6] == 0x20) { V05[6] = '0';}                   
  if (V04[5] == 0x20) { V04[5] = '0';}
  if (V04[8] == 0x20) { V04[8] = '0';}
  dat[0] = V04[8];        // d
  dat[1] = V04[9];        // d
  dat[2] = '.';
  dat[3] = V04[5];        // m
  dat[4] = V04[6];        // m
  dat[5] = '.';
  dat[6] = V04[0];        // j
  dat[7] = V04[1];
  dat[8] = V04[2];
  dat[9] = V04[3];
  dat[10] = '\0';
}

void showParsedData() {
  fillSpaces();

  char* p = zd420;
  
  strcpy_P(p, a);       // ^XA^PW580^LL400^FO180,80^A0N,60,60^FD
  p += strlen_P(a);
  strcpy(p, V00);       // weight
  p += strlen(V00);
  *p = ' ';             // space
  p++;
  strcpy(p, V01);       // unit
  p += strlen(V01);
  strcpy_P(p, b);       // ^FS^FO180,170^A0N,30,30^FD
  p += strlen_P(b);
  strcpy(p, dat);       // date
  p += strlen(dat);
  strcpy_P(p, c);       // ^FS^FO180,210^A0N,30,30^FD
  p += strlen_P(c);
  
  strcpy(p, V05);       // time
  p += strlen(V05);
  strcpy_P(p, d);       // ^FS^XZ
  p += strlen_P(d);


RS232.print (e);
delay(5);
RS232.flush();
delay(30);
RS232.print(zd420);
delay(500);
RS232.print(zd420);
RS232.flush();
delay(150);

}