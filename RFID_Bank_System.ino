// Include required libraries
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>

// Create instances
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(10, 9);  // MFRC522 mfrc522(SS_PIN, RST_PIN)

char initial_password[4] = { '0', '0', '0', '0' };  // Variable to store initial password
String tagUID = "5B E6 6E 12";                      // String to store UID of tag. Change it with your tag's UID
char password[4];                                   // Variable to store user's password
boolean RFIDMode = false;                           // boolean to change modes, initially false to enter amount first
boolean confirmMode = false;                        // To handle confirmation before scanning card
boolean pinEntryMode = false;                       // To enter PIN after card scan
char key_pressed = 0;                               // Variable to store incoming keys
uint8_t i = 0;                                      // Variable used for counter
const int buzzer = 5;
String amount = "";  // Store entered amount

// Defining how many rows and columns our keypad has
const byte rows = 4;
const byte columns = 4;

// Keypad pin map
char hexaKeys[rows][columns] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '*', '0', '#', 'D' },
  { '7', '8', '9', 'C' }
};

// Initializing pins for keypad
byte row_pins[rows] = { A0, A1, A2, A3 };
byte column_pins[columns] = { 4, 3, 2 };  // Ensure the array size matches the number of columns (3 in this case)

// Create instance for keypad
Keypad keypad_key = Keypad(makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

void setup() {
  lcd.init();  // LCD screen
  lcd.backlight();
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);  // Initialize serial communication

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-----SRMIST-----");
  lcd.setCursor(0, 1);
  lcd.print("Payment System");

  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Amount:");
}

void loop() {
  // Step 1: Enter the amount
  if (RFIDMode == false && confirmMode == false && pinEntryMode == false) {  // Entering the amount
    key_pressed = keypad_key.getKey();                                       // Storing keys
    if (key_pressed) {
      if (key_pressed == '#') {  // User confirms the amount with '#'

        // Check if the amount is empty or all zeros
        bool isAllZeros = true;
        for (int j = 0; j < amount.length(); j++) {
          if (amount[j] != '0') {
            isAllZeros = false;  // If any character is not '0', it's valid
            break;
          }
        }

        if (amount == "" || isAllZeros) {  // Check if the amount is empty or all zeros
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter valid");
          lcd.setCursor(0, 1);
          lcd.print("Amount");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter Amount:");
          amount = "";
        } else {
          confirmMode = true;  // Move to confirmation mode
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Amount: ");
          lcd.print(amount);
          lcd.setCursor(0, 1);
          lcd.print("1:Yes  0:No");
        }
      } else if (key_pressed == '*') {  // Clear amount with '*'
        amount = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Amount:");
      } else {  // Keep entering the amount
        amount += key_pressed;
        lcd.setCursor(0, 1);
        lcd.print(amount);
      }
    }
  }

  // Step 2: Confirm the amount
  if (confirmMode == true && pinEntryMode == false) {
    key_pressed = keypad_key.getKey();
    if (key_pressed == '1') {  // Confirm the amount with '1'
      RFIDMode = true;
      confirmMode = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tap Your Card!");
    } else if (key_pressed == '0') {  // Reject and re-enter the amount with '0'
      confirmMode = false;
      amount = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Amount:");
    }
  }

  // Step 3: Scan RFID card
  if (RFIDMode == true && pinEntryMode == false) {
    if (!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++) {
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();

    if (tag.substring(1) == tagUID) {
      lcd.clear();
      tone(buzzer, 2000);
      delay(200);
      noTone(buzzer);
      lcd.print("Card Detected :)");
      delay(1000);
      lcd.clear();
      lcd.print("Enter Pin:");
      lcd.setCursor(0, 1);
      pinEntryMode = true;
      RFIDMode = false;  // Disable RFID mode
    } else {
      tone(buzzer, 2000);
      delay(200);
      noTone(buzzer);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card Invalid!");
      delay(2000);

      // Resetting the system after wrong card is detected
      lcd.clear();
      lcd.print("Please Scan a");
      lcd.setCursor(0, 1);
      lcd.print("Valid Card.");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Amount:");
      amount = "";           // Reset the amount for the next transaction
      RFIDMode = false;      // Reset RFID mode
      confirmMode = false;   // Ensure confirm mode is false
      pinEntryMode = false;  // Ensure pin entry mode is false
      i = 0;                 // Reset the PIN index
    }
  }

  // Step 4: Enter the PIN
  if (pinEntryMode == true) {
    key_pressed = keypad_key.getKey();  // Storing keys
    if (key_pressed) {
      if (key_pressed == '*') {  // Cancel the entire process with '*'
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Processing");
        delay(2500);
        lcd.setCursor(10, 0);
        lcd.print(".");
        delay(2500);
        lcd.setCursor(11, 0);
        lcd.print(".");
        delay(2500);
        lcd.setCursor(12, 0);
        lcd.print(".");
        delay(2500);
        delay(2000);  // Simulate processing delay

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Transaction");
        lcd.setCursor(0, 1);
        lcd.print("Cancelled.");
        tone(buzzer, 5000);
        delay(500);
        noTone(buzzer);
        delay(2000);

        lcd.clear();
        pinEntryMode = false;  // Reset for next transaction
        RFIDMode = false;
        i = 0;        // Reset PIN index
        amount = "";  // Reset amount for next transaction
        lcd.setCursor(0, 0);
        lcd.print("Enter Amount:");
      } else {
        password[i++] = key_pressed;  // Storing in password variable
        lcd.print("*");

        Serial.print("Key pressed: ");
        Serial.println(key_pressed);

        if (i == 4) {  // If 4 keys are entered
          delay(200);
          Serial.print("Entered PIN: ");
          for (int j = 0; j < 4; j++) {
            Serial.print(password[j]);
          }
          Serial.println();

          if (!(strncmp(password, initial_password, 4))) {  // If password matches
            lcd.clear();
            lcd.print("Pin Accepted.");
            tone(buzzer, 2000);
            delay(200);
            noTone(buzzer);
            delay(500);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Processing");
            delay(2500);
            lcd.setCursor(10, 0);
            lcd.print(".");
            delay(2500);
            lcd.setCursor(11, 0);
            lcd.print(".");
            delay(2500);
            lcd.setCursor(12, 0);
            lcd.print(".");
            delay(2500);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Transaction");
            lcd.setCursor(0, 1);
            lcd.print("Successful!");
            tone(buzzer, 5000);
            delay(500);
            noTone(buzzer);
            delay(2000);

            lcd.clear();
            pinEntryMode = false;  // Reset for next transaction
            RFIDMode = false;
            i = 0;        // Reset PIN index
            amount = "";  // Reset amount for next transaction
            lcd.setCursor(0, 0);
            lcd.print("Enter Amount:");
          } else {
            lcd.clear();
            lcd.print("Wrong Pin!");
            tone(buzzer, 5000);
            delay(500);
            noTone(buzzer);
            delay(2000);
            i = 0;  // Reset PIN index
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Enter Pin:");
          }
        }
      }
    }
  }
}