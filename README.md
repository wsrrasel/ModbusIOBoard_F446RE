# Modbus IO Board Firmware

This repository contains the firmware for the **Modbus IO Board**, designed for seamless integration with Modbus RTU protocol and hardware IO control. This firmware supports digital inputs (DI), digital outputs (DO), and debug functionality.

## Features

- **Modbus RTU Slave**: Configurable slave address, baud rate, data bits, parity, and stop bits.
- **Digital Inputs (DI)**: 8 inputs with configurable debounce delays.
- **Digital Outputs (DO)**: 8 outputs with simple ON/OFF control.
- **RS485 Support**: Efficient UART-based communication with TX/RX control.
- **Debug Interface**: UART-based debugging with adjustable parameters.
- **Factory Reset**: Simple mechanism to reset to default settings.

---

## Getting Started

### Hardware Setup

#### Pin Configurations
| **Function**       | **MCU Pin** | **Description**                |
|---------------------|-------------|--------------------------------|
| **Slave Address**   | PB9, PB3, PA15, PB2 | DIP switch for address setting. |
| **RS485 Interface** | PC6, PC7, PD2, PB15, PB14 | UART TX/RX with LED indicators. |
| **Digital Inputs**  | PA0-PA4, PB8-PB4 | 8 inputs with debounce functionality. |
| **Digital Outputs** | PA8-PA12, PC9, PC8, PB12 | 8 outputs for control. |
| **Debug Port**      | PA2, PA3 | UART TX/RX for debugging. |
| **Factory Reset**   | PC10 | Press and hold for reset. |

---

## Software Configuration

### Modbus Configuration
- **Registers for Modbus RTU Slave**:
  | **Register**          | **Address** | **Range**       | **Default** |
  |-----------------------|-------------|-----------------|-------------|
  | Slave Address         | 30001       | 1 - 247         | 1           |
  | Baud Rate             | 40021       | 0 - 65535       | 19200       |
  | Data Bit              | 40022       | 7, 8            | 8           |
  | Parity                | 40023       | 0 = None, 1 = Odd, 2 = Even | 2 |
  | Stop Bit              | 40024       | 1, 2            | 1           |
  | Keep Alive Timeout    | 40025       | 1000 - 65535 ms | 1000 ms     |

- **Debug Configuration**:
  | **Parameter**         | **Value**   |
  |-----------------------|-------------|
  | Baud Rate             | 576000      |
  | Data Bits             | 8           |
  | Parity                | None        |
  | Stop Bits             | 1           |

---

## Usage

1. Configure the DIP switch for the desired slave address.
2. Connect the board via RS485 and power it up.
3. Use Modbus master software or tools to communicate with the board.
4. Debug via UART if needed.

---

## Factory Reset

- Hold the button connected to PC10 to reset all configurations to factory default.

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Contributions

Contributions and feedback are welcome. Please follow the [contribution guidelines](CONTRIBUTING.md).

---

## Contact

For queries, please reach out to wsrrasel@gmail.com.

---

### Acknowledgments

- Modbus protocol resources.
- Open-source tools for Modbus communication and debugging.
