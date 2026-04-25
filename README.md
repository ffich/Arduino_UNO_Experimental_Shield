# Arduino_UNO_Experimental_Shield
Experimental shield for Arduino UNO form factor boards.

Welcome to the official repository of the **UNO Experimental SHIELD**, an expansion board designed to provide a complete laboratory environment for experimenting, testing, and learning with the Arduino UNO Boards (e.g **UNO Q**).
This shield integrates digital and analog I/O, SPI, I2C, sensors, memory, and diagnostics to create a versatile platform ideal for rapid prototyping, debugging, and education.

<img width="869" height="688" alt="image" src="https://github.com/user-attachments/assets/804fbab6-fae5-49fe-ab55-bd0280554af7" />

---

## 🚀 Key Features

### 🔹 Digital I/O
- SMD LEDs on pins **D2, D3, D6, D7, D8, D9**
- Push buttons on pins **A2, A3**

### 🔹 Analog Inputs
- **Two 100 kΩ trimmers** on analog inputs **A0, A1**  

### 🔹 Serial Communication
- **USB-to-Serial bridge** (CH340)  

### 🔹 CAN Bus
- **CAN transceiver** (TJA1051T3)  
- **120 Ω termination resistor** enabled by jumper  
- Screw terminal output: CANH / CANL

### 🔹 RS-485
- **RS485 transceiver**
- **120 Ω termination resistor** enabled by jumper  
- Screw terminal output: A / B 

### 🔹 I2C Display
- Integrated **SSD1306 OLED display**

### 🔹 I2C Thermometer
- Integrated **TMP102 thermo**  

### 🔹 SPI Memory
- **25LC256 EEPROM** connected to hardware SPI  
- Dedicated Chip Select line

### 🔹 Crypto Chip
- Integrated **ATECC608C**  - 

---

## 📐 Pinout Summary

### Digital I/O
| UNO Pin | Shield Function |
|------------|-----------------|
| D2, D3, D6, D7, D8, D9 | LED  |
| A2, A3 | Buttons  |

### Analog
| Pin | Function |
|-----|----------|
| A0 | Trimmer 1 |
| A1 | Trimmer 2 |

### Peripherals
| Function | Pin |
|----------|-----|
| SPI EEPROM | MOSI / MISO / SCK + CS |
| I2C OLED | SDA / SCL |
| I2C Thermo | SDA / SCL |
| I2C Crypto Chip | SDA / SCL |
| CAN | CAN_TX / CAN_RX |
| RS485 | TX / RX |
| USB-Serial | TX / RX |

The Shield is compatible with both Arduino UNOs with 3V3 and 5V logic (some jumpers allow to adapt the voltage levels).

---

## 🛠️ Project Goals

This shield is intended for:
- Users who want a **complete test environment** for Arduino UNO Boards (e.g. **UNO Q**, **UNO R4**) 
- Makers performing **rapid firmware debugging**  
- Schools, workshops, and educational labs  
- Testing all core firmware interfaces (PWM, ADC, SPI, I2C, CAN, UART)

---

## 📷 Logo

The official project logo is available in the `/assets/logo/` directory.

---

## 📝 License

This project is released under the **MIT License**, unless otherwise specified for hardware files.

---

## 🤝 Contributing

Pull requests and issues are welcome!  
Contributions improving documentation, firmware examples, or hardware design are highly appreciated.

---

## 📫 Contact

For questions or suggestions, open an Issue in the repository or contact the maintainer.

---

Happy experimenting! 🔧✨

