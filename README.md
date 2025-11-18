# H-Bridge Switching System

This project provides an Arduino-based controller for three H-Bridge stages (HBA, HBB, HBC).  
The switching logic is interrupt-driven and implements a six-step commutation pattern suitable for multi-phase loads such as BLDC motors or similar phase-driven systems.

The following documentation describes system architecture, pin assignments, commutation behavior, timing, and advanced usage options.

---

## 1. System Architecture (ASCII Block Diagram)

```
                  +-----------------------+
                  |       Arduino         |
                  |   (e.g. Mega 2560)    |
                  +----------+------------+
                             |
                             |
            +----------------+----------------+
            |                |                |
    Interrupt A        Interrupt B      Interrupt C
        Pin 18            Pin 19            Pin 20
            |                |                |
    +-------+-------+ +------+-------+ +------+-------+
    |  ISR switch_A | | ISR switch_B | | ISR switch_C |
    +-------+-------+ +------+-------+ +------+-------+
            |                |                |
            +---------+------+---------+------+
                      |                |
        HBX_cycle_state (0–2)     HBX_second_half (polarity)
                      |                |
                +-----+----------------+------+
                |  Switching Logic (loop)     |
                +---------------+-------------+
                                |
             +------------------+------------------+
             |                  |                  |
       +-----+-----+      +-----+-----+      +-----+-----+
       |   HBA     |      |   HBB     |      |   HBC     |
       +-----------+      +-----------+      +-----------+
       | A1 A2 B1 B2      | A1 A2 B1 B2      | A1 A2 B1 B2
       | 2  3  4  5       | 6  7  8  9       | 10 11 12 13
       +------------------+------------------+-------------+
```

## 2. Switching-State Diagram (6-Step Sequence)

```
       ┌────────────────────────────────────────┐
       │               STATE FLOW               │
       └────────────────────────────────────────┘

            +---------+        +---------+        +---------+
            | State 0 | -----> | State 1 | -----> | State 2 |
            +----+----+        +----+----+        +----+----+
                 ^                  ^                  ^
                 |                  |                  |
                 |                  |                  |
            +----+------------------+------------------+----+
            |               Polarity Toggle                |
            |         (HBX_second_half ^= 1)               |
            +----------------------------------------------+
```
Each state has:

- Normal version  
- Inverted version (`second_half = 1`)

This yields the following 6-step commutation:

```

0 → 0' → 1 → 1' → 2 → 2' → repeat
(0' = inverted polarity of 0)

```

---

## 3. Pinout Reference

### H-Bridge Pin Assignment Table

| H-Bridge | A1 | A2 | B1 | B2 |
|----------|----|----|----|----|
| HBA      | 2  | 3  | 4  | 5  |
| HBB      | 6  | 7  | 8  | 9  |
| HBC      | 10 | 11 | 12 | 13 |

### Interrupt Pin Assignment Table

| Interrupt | Pin |
|-----------|-----|
| A         | 18  |
| B         | 19  |
| C         | 20  |

---

## 4. Commutation Diagram (Waveform Overview)

Below is a conceptual representation of a phase switching pattern for 6-step commutation:

```

Time --->

State:   0    0'    1    1'    2    2'

HBA:    ++++ ----  .... ....  **** ****
HBB:    .... ....  ++++ ----  .... ....
HBC:    .... ....  .... ....  ++++ ----

Legend:
++++ = forward polarity
---- = reverse polarity
.... = off
**** = polarity depends on half-cycle

```

> *(Actual levels depend on direction and pin wiring.)*

---

## 5. Timing Sequence Chart

```

Time ---> |---- ISR A ----|---- ISR A ----|---- ISR B ----|---- ISR B ----| ...

Interrupt:      A                A               B                B
State:          0               0'               1               1'
HBX_second:     0 -> 1          1 -> 0          0 -> 1          1 -> 0

Effective Step: 0               0'               1               1'

```

This continues for interrupt C:

```

ISR C → State 2 / 2'

````

---

## 6. Advanced Usage Notes

### Debouncing Interrupts

Mechanical switches can emit multiple edges. Consider:

- Hardware RC filter  
- Software debounce inside ISR (not recommended for long waits)  
- Lockout flag in main loop  

### Using the System for BLDC Motors

The six-step switching aligns with trapezoidal BLDC commutation. Possible extensions:

- Back-EMF sensing  
- Hall sensor inputs  
- Speed control (PID)  
- PWM modulation instead of static HIGH/LOW  

### Using PWM

To integrate PWM:

- Replace `digitalWrite()` calls with `analogWrite()` on compatible pins  
- Ensure your motor driver supports PWM on both high and low sides  

### Expanding to More Phases

Add additional arrays:

```cpp
int HBD[] = {14, 15, 16, 17};
````

Increase state cycle accordingly.

---

## 7. Technical Documentation

### Interrupt Handling

The firmware uses edge-triggered interrupts (RISING). Each ISR:

1. Sets `HBX_cycle_state`
2. Toggles `HBX_second_half`
3. Returns immediately (no delays)

This ensures minimal interrupt latency.

### Switching Logic

The main loop continuously applies output levels based on global state values. This design avoids performing I/O writes inside ISRs.

### Safety Considerations

* Ensure your H-Bridge design prevents shoot-through
* Never activate both high-side switches simultaneously
* Only use drivers with built-in dead-time or add firmware-based timing
* Confirm current capability and thermal limits of the driver hardware

## 8. License and Copyright Notice

This project is licensed under the GNU Afferro General Public License to Carl Öttinger

Copyright © 2025 by Carl Öttinger