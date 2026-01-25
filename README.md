# Arduino_UNO_Experimental_Shield
Experimental shield for Arduino UNO form factor boards.

Welcome to the official repository of the **Experimental SHIELD**, an expansion board designed to provide a complete laboratory environment for experimenting, testing, and learning with the Arduino UNO Q.
This shield integrates digital and analog I/O, SPI, I2C, sensors, memory, and diagnostics to create a versatile platform ideal for rapid prototyping, debugging, and education.

<img width="801" height="641" alt="image" src="https://github.com/user-attachments/assets/cd532058-da11-46a7-9276-57e8f626410f" />

---

## 🚀 Key Features

### 🔹 Digital I/O
- SMD LEDs on pins **D0–D7**
- Push buttons on pins **D2–D5**

### 🔹 Analog Inputs
- **Two 100 kΩ trimmers** → A0, A1  

### 🔹 Serial Communication
- **USB-to-Serial bridge** (CH340)  
- Jumpers to route TX/RX to UNO hardware serial pins

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

### 🔹 PWM Audio
- **Passive buzzer** driven via PWM  

### 🔹 SPI Memory
- **25LC256 EEPROM** connected to hardware SPI  
- Dedicated Chip Select line

---

## 📐 Pinout Summary

### Digital I/O
| UNO Pin | Shield Function |
|------------|-----------------|
| D0–D7 | LED  |
| D2–D5 | Buttons  |

### Analog
| Pin | Function |
|-----|----------|
| A0 | Trimmer 1 |
| A1 | Trimmer 2 |

### Peripherals
| Function | Pin |
|----------|-----|
| PWM Buzzer | D3 |
| SPI EEPROM | MOSI / MISO / SCK + CS |
| I2C OLED | SDA / SCL |
| I2C Thermo | SDA / SCL |
| CAN | CAN_TX / CAN_RX |
| RS485 | TX / RX |
| USB-Serial | TX / RX |

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

