# Arduino_UNO_Experimental_Shield
Experimental shield for Arduino UNO form factor boards.



Welcome to the official repository of the **Experimental SHIELD**, an expansion board designed to provide a complete laboratory environment for experimenting, testing, and learning with the Arduino UNO Q.
This shield integrates digital and analog I/O, SPI, I2C, sensors, memory, and diagnostics to create a versatile platform ideal for rapid prototyping, debugging, and education.

---

## 🚀 Key Features

### 🔹 Digital I/O
- SMD LEDs on pins **D2–D9**
- Push buttons on pins **D2–D9**
- Individual jumpers to enable/disable LED and button per pin

### 🔹 Analog Inputs
- **Two 10 kΩ trimmers** → A0, A1  
- **LM35 temperature sensor** → A2  
  - RC filter + supply bypass for stable readings  
- **Light sensor (LDR in voltage divider)** → A3

### 🔹 Serial Communication
- **USB-to-Serial bridge** (CH340/CP2102 or equivalent)  
- Jumpers to route TX/RX to UNO hardware serial pins

### 🔹 CAN Bus
- **5V-compatible CAN transceiver** (e.g., MCP2562)  
- **120 Ω termination resistor** enabled by jumper  
- Screw terminal output: CANH / CANL / GND

### 🔹 I2C Display
- Integrated **SSD1306 OLED display**  
- Additional I2C expansion header

### 🔹 PWM Audio
- **Passive buzzer** driven via PWM  
- NPN transistor (BC547 / 2N2222) driver  
- Jumper to disconnect the buzzer when not needed

### 🔹 SPI Memory
- **25LCxxx EEPROM** connected to hardware SPI  
- Dedicated Chip Select line

---

## 📐 Pinout Summary

### Digital I/O
| UNO Pin | Shield Function |
|------------|-----------------|
| D2–D9 | LED + Button (isolatable via jumpers) |

### Analog
| Pin | Function |
|-----|----------|
| A0 | Trimmer 1 |
| A1 | Trimmer 2 |
| A2 | LM35 |
| A3 | LDR |

### Peripherals
| Function | Pin |
|----------|-----|
| PWM Buzzer | D10 (configurable) |
| SPI EEPROM | MOSI / MISO / SCK + CS |
| I2C OLED | SDA / SCL |
| CAN | CAN_TX / CAN_RX |

---

## 🛠️ Project Goals

This shield is intended for:
- Users who want a **complete test environment** for Arduino UNO Boards  
- Makers performing **rapid firmware debugging**  
- Schools, workshops, and educational labs  
- Testing all core firmware interfaces (PWM, ADC, SPI, I2C, CAN, UART)

---

## 📷 Logo

The official project logo is available in the `/assets/logo/` directory.

---

## 📦 Repository Structure

- `/hardware/` – KiCad files, schematics, PCB layout  
- `/firmware/` – Example sketches for testing each subsystem  
- `/docs/` – Technical docs, notes, and datasheets  
- `/assets/` – Logos, media, and images

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

