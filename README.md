## Navigation

* [DE Deutsch German](https://github.com/FaolanBig/motor-control?tab=readme-ov-file#h-br%C3%BCcken-schaltsystem-deutsch-german-de)
* [EN English](https://github.com/FaolanBig/motor-control?tab=readme-ov-file#h-bridge-switching-system-english-en)

---

# H-Brücken-Schaltsystem (Deutsch German DE)

Dieses Projekt bietet einen Arduino-basierten Controller für drei H-Brücken-Stufen (HBA, HBB, HBC).  
Die Schaltlogik ist interruptgesteuert und implementiert ein sechsstufiges Kommutierungsmuster, das für mehrphasige Lasten wie BLDC-Motoren oder ähnliche phasengesteuerte Systeme geeignet ist.

Die folgende Dokumentation beschreibt die Systemarchitektur, die Pinbelegung, das Kommutierungsverhalten, das Timing und erweiterte Nutzungsoptionen.

---

## 1. Systemarchitektur

```
                  +---------------------- -+
                  |       Arduino          |
                  |   (z. B. Mega 2560)    |
                  +----------+-------------+
                             |
                             |
            +----------------+----------------+
            |                |                |
       Interrupt A      Interrupt B      Interrupt C
         Pin 18           Pin 19           Pin 20
            |                |                |
    +-------+-------+ +------+-------+ +------+-------+
    |  ISR switch_A | | ISR switch_B | | ISR switch_C |
    +-------+-------+ +------+-------+ +------+-------+
            |                |                |
            +---------+------+---------+------+
                      |                |
        HBX_cycle_state (0–2)     HBX_second_half (Polarität)
                      |                |
                +-----+----------------+------+
                |  Schaltlogik (Schleife)     |
                +----------------+------------+
                                 |
                                 |
             +-------------------+-------------------+
             |                   |                   |
       +-----+-------+     +-----+-------+     +-----+-------+
       |    HBA      |     |   HBB       |     |   HBC       |
       +-------------+     +-------------+     +-------------+
       | A1 A2 B1 B2 |     | A1 A2 B1 B2 |     | A1 A2 B1 B2 |
       | 2  3  4  5  |     | 6  7  8  9  |     | 10 11 12 13 |
       +-------------+-----+-------------+-----+-------------+
```

## 2. Schaltzustandsdiagramm (6-Stufen-Sequenz)

```
       ┌────────────────────────────────────────┐
       │               ZUSTANDSFLUSS            │
       └────────────────────────────────────────┘

            +-----------+        +-----------+        +-----------+
            | Zustand 0 | -----> | Zustand 1 | -----> | Zustand 2 |
            +-----+-----+        +----+------+        +----+------+
                  ^                   ^                    ^
                  |                   |                    |
                  |                   |                    |
            +-----+-------------------+--------------------+-----+
            |               Polaritätsumschaltung                |
            |              (HBX_second_half ^= 1)                |
            +---------- -----------------------------------------+
```
Jeder Zustand hat:

- Normale Version  
- Invertierte Version (`second_half = 1`)

Dies ergibt die folgende 6-stufige Kommutierung:

```

0 → 0' → 1 → 1' → 2 → 2' → Wiederholung
(0' = invertierte Polarität von 0)

```

---

## 3. Pinbelegungsreferenz

### H-Brücken-Pinbelegungstabelle

| H-Brücke | A1 | A2 | B1 | B2 |
|----------|----|----|----|----|
| HBA      | 2  | 3  | 4  | 5  |
| HBB      | 6  | 7  | 8  | 9  |
| HBC      | 10 | 11 | 12 | 13 |

### Tabelle zur Zuordnung der Interrupt-Pins

| Interrupt | Pin |
|-----------|-----|
| A         | 18  |
| B         | 19  |
| C         | 20  |

---

## 4. Kommutierungsdiagramm (Übersicht über die Wellenform)

Nachfolgend finden Sie eine konzeptionelle Darstellung eines Phasenschaltmusters für eine 6-stufige Kommutierung:

```

Zeit --->

Zustand: 0    1     2    1'    2'   3'

HBA:    ++++ ....  ---- ----  .... ++++
HBB:    ---- ++++  .... ....  ++++ ----
HBC:    .... ----  ++++ ++++  ---- ....

Legende:
++++ = Vorwärtspolarität
---- = Rückwärtspolarität....
.... = Aus

```

> *(Die tatsächlichen Pegel hängen von der Richtung und der Pin-Verdrahtung ab.)*

---

## 5. Zeitablaufdiagramm

```

Zeit ---> |---- ISR A ----|---- ISR A ----|---- ISR B ----|---- ISR B ----| ...

Interrupt:        A               A                B               B
Zustand:          0               0'               1               1'
HBX_second:     0 -> 1          1 -> 0           0 -> 1          1 -> 0

Effektiver Schritt: 0             0'               1               1'

```

Dies setzt sich für Interrupt C fort:

```

ISR C → Status 2 / 2'

````

---

## 6. Hinweise zur fortgeschrittenen Verwendung

### Verwendung des Systems für BLDC-Motoren

Die sechsstufige Umschaltung entspricht der trapezförmigen BLDC-Kommutierung. Mögliche Erweiterungen:

- Back-EMF-Erkennung  
- Hall-Sensor-Eingänge  
- Drehzahlregelung (PID)  
- PWM-Modulation anstelle von statischem HIGH/LOW  

### Verwendung von PWM

So integrieren Sie PWM:

- Ersetzen Sie `digitalWrite()`-Aufrufe durch `analogWrite()` auf kompatiblen Pins  
- Stellen Sie sicher, dass Ihr Motortreiber PWM sowohl auf der High- als auch auf der Low-Seite unterstützt  

### Erweiterung auf weitere Phasen

Fügen Sie zusätzliche Arrays hinzu:

```cpp
int HBD[] = {14, 15, 16, 17};
````

Erhöhen Sie den Statuszyklus entsprechend.

---

## 7. Technische Dokumentation

### Interrupt-Behandlung

Die Firmware verwendet flankengesteuerte Interrupts (RISING). Jeder ISR:

1. Setzt `HBX_cycle_state`
2. Schaltet `HBX_second_half` um
3. Kehrt sofort zurück (keine Verzögerungen)

Dadurch wird eine minimale Interrupt-Latenz gewährleistet.

### Schaltlogik

Die Hauptschleife wendet kontinuierlich Ausgangspegel basierend auf globalen Statuswerten an. Durch dieses Design wird vermieden, dass I/O-Schreibvorgänge innerhalb von ISRs durchgeführt werden.

### Sicherheitshinweise

* Stellen Sie sicher, dass Ihr H-Brücken-Design ein Durchschlagen verhindert.
* Aktivieren Sie niemals beide High-Side-Schalter gleichzeitig (polarität prüfen).
* Verwenden Sie nur Treiber mit integrierter Totzeit oder fügen Sie firmwarebasierte Zeitsteuerung hinzu.
* Überprüfen Sie die Strombelastbarkeit und die thermischen Grenzen der Treiberhardware.

## 8. Lizenz- und Copyright-Hinweis

Dieses Projekt unterliegt der GNU Afferro General Public License

Copyright © 2025 by Carl Öttinger

# H-Bridge Switching System (English EN)

This project provides an Arduino-based controller for three H-Bridge stages (HBA, HBB, HBC).  
The switching logic is interrupt-driven and implements a six-step commutation pattern suitable for multi-phase loads such as BLDC motors or similar phase-driven systems.

The following documentation describes system architecture, pin assignments, commutation behavior, timing, and advanced usage options.

---

## 1. System Architecture

```
                  +-----------------------+
                  |       Arduino         |
                  |   (e.g. Mega 2560)    |
                  +----------+------------+
                             |
                             |
            +----------------+----------------+
            |                |                |
       Interrupt A      Interrupt B      Interrupt C
         Pin 18           Pin 19           Pin 20
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
                +----------------+------------+
                                 |
             +-------------------+-------------------+
             |                   |                   |
       +-----+-------+     +-----+-------+     +-----+-------+
       |    HBA      |     |   HBB       |     |   HBC       |
       +-------------+     +-------------+     +-------------+
       | A1 A2 B1 B2 |     | A1 A2 B1 B2 |     | A1 A2 B1 B2 |
       | 2  3  4  5  |     | 6  7  8  9  |     | 10 11 12 13 |
       +-------------+-----+-------------+-----+-------------+
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
            |               Polarity Toggle                 |
            |         (HBX_second_half ^= 1)                |
            +-----------------------------------------------+
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

State:    0    1    2    1'    2'   3'

HBA:    ++++ ....  ---- ----  .... ++++
HBB:    ---- ++++  .... ....  ++++ ----
HBC:    .... ----  ++++ ++++  ---- ....

Legend:
++++ = forward polarity
---- = reverse polarity
.... = off

```

> *(Actual levels depend on direction and pin wiring.)*

---

## 5. Timing Sequence Chart

```

Time ---> |---- ISR A ----|---- ISR A ----|---- ISR B ----|---- ISR B ----| ...

Interrupt:      A                A               B                B
State:          0                0'              1                1'
HBX_second:   0 -> 1           1 -> 0          0 -> 1           1 -> 0

Effective Step: 0                0'              1                1'

```

This continues for interrupt C:

```

ISR C → State 2 / 2'

````

---

## 6. Advanced Usage Notes

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
* Never activate both high-side switches simultaneously (check polarity)
* Only use drivers with built-in dead-time or add firmware-based timing
* Confirm current capability and thermal limits of the driver hardware

## 8. License and Copyright Notice

This project is licensed under the GNU Afferro General Public License

Copyright © 2025 by Carl Öttinger
