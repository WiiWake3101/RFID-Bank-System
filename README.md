# RFID Bank System

The RFID Bank System is a project designed to use RFID technology for payment transactions. Users can enter an amount, scan their RFID card, and confirm the transaction by entering a PIN.

## Components Used
- MFRC522 RFID Reader
- LiquidCrystal_I2C LCD
- Keypad
- Buzzer
- Arduino Board

## Features
- Enter amount for transaction
- Confirm the entered amount
- Scan RFID card to proceed
- Enter PIN to complete the transaction
- Buzzer feedback for user actions

## Future Enhancements
- Integration with a database to store user data and transaction history
- Implement the system in a college environment using student ID cards
- Allow students to store and use amounts for various services like DTP printing or canteen purchases

## Getting Started

### Prerequisites
- Arduino IDE installed on your computer
- Required libraries installed in Arduino IDE:
  - MFRC522
  - LiquidCrystal_I2C
  - Keypad
  - SPI

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/WiiWake3101/RFID-Bank-System.git
   ```
2. Open the `RFID-Bank-System` folder in Arduino IDE.
3. Upload the code to your Arduino board.

### Usage
1. Connect the hardware components as specified in the code.
2. Power on the Arduino board.
3. The LCD will prompt you to enter the amount.
4. Confirm the amount by pressing `#` or clear by pressing `*`.
5. Scan your RFID card.
6. Enter the PIN to complete the transaction.

## Contributing
Contributions are welcome! Feel free to open issues or submit pull requests for new features, bug fixes, or improvements.

## Contact
For any questions or suggestions, please contact [WiiWake3101](https://github.com/WiiWake3101).

---
This project can be expanded to include a database for storing user information and transaction history. This would enable its implementation in a college environment, where students could use their ID cards for various services like DTP printing or canteen purchases.
```
