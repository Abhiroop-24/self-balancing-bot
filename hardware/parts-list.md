# Parts List

Here is the list of hardware components used in the ESP32 Self-Balancing Robot (V1).

---

## 🔌 Main Electronics

| Component | Qty | Link Type | Notes |
|---|---|---|---|
| ESP32 Dev Board | 1 | Microcontroller | Wi-Fi, BLE onboard |
| TB6612FNG Motor Driver | 1 | Motor Driver | Dual H-Bridge |
| MPU6050 | 1 | IMU Sensor | Gyro + Accelerometer |

---

## ⚙ Power Section

| Component | Qty | Notes |
|---|---|---|
| 18650 Li-ion battery | 3 | Connected as 3S pack |
| 3S BMS | 1 | Battery protection |
| Buck Converter | 1 | 12V→5V conversion |
| Fan (5V) | 1 | Optional, for cooling |
| Battery Level Indicator | 1 | Display voltage |

---

## 🌀 Motors & Locomotion

| Component | Qty | Notes |
|---|---|---|
| 300 RPM Metal Gear Motor | 2 | Torque ~2 kg-cm |
| Wheels (10 cm diameter) | 2 | Rubber |
| PVC Pipe Body Tube | 1 | Used as chassis |

---

## 💡 Display & Indicators

| Component | Qty | Usage |
|---|---|---|
| 8×8 LED Matrix (MAX7219) | 1 | Animation/Status |
| On-board LED (ESP32 pin 2) | 1 | Status blinking |

---

## 🔄 Switching and Connectivity

| Component | Qty | Notes |
|---|---|---|
| DPDT or 2-way Switch | 1 | For ON/OFF + charging |
| JST Connectors | Few | For battery/driver |

---

## 🧷 Hardware & Mechanical Items

| Component | Notes |
|---|---|
| Zip ties | For mounting battery and wires |
| Screw set | Motor mounting |
| Foam pads | For vibration reduction |
| Heat Shrink Sleevings | Insulation |
| Wires (24/22 AWG) | Power + signal wiring |

---

## Cost Estimate (Approx)

| Item Category | Cost Range |
|---|---|
| Electronics | ₹800–₹1000 |
| Motors & Wheels | ₹600–₹900 |
| Battery section | ₹450–₹700 |
| Structure, switch, wiring | ₹200–₹300 |

---

## 💰 Total Estimated Cost  
**₹2,000 to ₹3,000 INR** depending on parts

---

## Notes
- You can upgrade to **high torque motors** for better balancing.
- BMS must support at least **10A peak**.
- Fan is optional but recommended if motors heat up.

---

Ready to upload on GitHub 👍🔥
