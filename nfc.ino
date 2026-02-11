#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
const int IO = 4;    // DAT
const int SCLK = 5;  // CLK
const int CE = 2;    // RST

ThreeWire myWire(4, 5, 2);  // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime compiled;

int oke = 0;
int jam,menit,detik;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd (0x27,16,2);
String authorizedString = "13 34 97 1A"; // Variable to hold the authorized string
String kartu ="13 34 97 1A";
String tag ="13 34 97 1A";


void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
  int jamStartkerja = 8;
  int jamSelesaikerja = 17;

  jam = dt.Hour();
  menit = dt.Minute();
  detik = dt.Second();
}

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();        
  lcd.backlight();  // Initiate SPI bus
  lcd.init();
  mfrc522.PCD_Init();   // Initiate MFRC522
  Rtc.Begin();

  compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();
  if (!Rtc.IsDateTimeValid()) {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }
}
  
int kumpulan_id[3];
String kumpulan_jenis_absen[3];
int kumpulan_status_nilai[3];
int no = 0;

void store_data(int id,String jenisAbsen,int nilai){

  kumpulan_id[no] = id;
  kumpulan_jenis_absen[no] = jenisAbsen;
  kumpulan_status_nilai[3] = nilai;
}
void tulis(String kata1,String kata2){
  lcd.setCursor(0, 0);
  lcd.print(kata1);
  lcd.setCursor(0, 1);
  lcd.print(kata2);
  delay(3000);
  lcd.clear();
}

void loop() 
{
  // Check if there's any input from the Serial Monitor
   RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now);

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  Serial.println("Approximate your card to the reader...");
  Serial.println("Enter the authorized string in the Serial Monitor and press Enter:");
}
  if (Serial.available() > 0) {
    authorizedString = Serial.readStringUntil('\n'); // Read the input string
    authorizedString.toUpperCase(); // Convert to uppercase for comparison
    Serial.print("Authorized string set to: ");
    Serial.println(authorizedString);
  }

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  // Show UID on serial monitor
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase(); // Convert UID to uppercase for comparison

  Serial.print("UID tag : ");
  Serial.println(content);

  // Compare the UID with the authorized string
  if(waktu == kerja){
    store_data(kartu,"masuk",1);
  }else {
    store_data(kartu,"masuk",1);
  }
  if (content.substring(1) == authorizedString) 
  {
    if(oke < 1){
    Serial.println("Authorized access");
    tulis("KARYAWAN"," MASUK!");
    oke += 1;
    }else{
    tulis("GAGAL KARYAWAN"," SUDAH MASUK!");
    }
  } 
  else 
  {
    tulis("BUKAN KARYAWAN"," GAGAL MASUK!");
  }

  
  Serial.println();
}
