# H-Bridge Switching System

This project implements an Arduino-based control system for three H-Bridge driver stages (HBA, HBB, and HBC).
Each H-Bridge is controlled through four digital output pins (A1, A2, B1, B2), while three external interrupt inputs trigger state transitions.
The system is suitable for driving multiphase loads, commutated motors, or any application that requires structured phase control.

## Features

Control of three independent H-Bridges

Phase switching through external interrupt inputs

Six-step switching pattern (three main phases, each with an inverted state)

Clean separation of hardware abstraction via arrays

Efficient interrupt-driven state management

## Hardware Overview
H-Bridge Output Pins

Each H-Bridge is defined by four pins:

H-Bridge	Pins
HBA	2, 3, 4, 5
HBB	6, 7, 8, 9
HBC	10, 11, 12, 13

These pins are configured as outputs during setup.

Interrupt Pins

Three interrupt inputs control the switching state:

Interrupt	Pin
A	18
B	19
C	20

Each interrupt maps to one of the system states (0, 1, or 2).

## System Behavior

The program maintains two global switching parameters:

HBX_cycle_state: selects which H-Bridge sequence is active (0, 1, or 2)

HBX_second_half: toggles the polarity for a given phase

When an interrupt is triggered, the corresponding ISR sets the new HBX_cycle_state and toggles HBX_second_half.
This results in a deterministic six-step switching routine, cycling through:

Phase 0 (normal / inverted)

Phase 1 (normal / inverted)

Phase 2 (normal / inverted)

This structure is appropriate for commutation routines commonly used in multi-phase motor control.

Core Function: switchPair()

The switchPair() function configures the logic levels for the four pins of a given H-Bridge.
It accepts the following parameters:

HBX[4]: pin array for the selected H-Bridge

direction: sets the logical polarity

off: forces all outputs LOW if true

Pins A1/A2 and B1/B2 are assigned complementary levels depending on the direction and state.

## Usage

Upload the code to your Arduino board.

Connect three H-Bridges to the defined output pins.

Provide stable power to the driver stages.

Connect external trigger signals to pins 18, 19, and 20.

Trigger any interrupt input to cycle through the switching pattern.

## Safety Notes

Ensure that H-Bridges are not enabled without proper load conditions.

Verify that your driver hardware supports the switching pattern used here.

Mechanical switches on interrupt lines may require debouncing to prevent unintended state changes.

Incorrect HIGH/LOW configurations can cause shoot-through in certain H-Bridge designs; verify compatibility before use.

## License

This project is licensed under the GNU Afferro General Public License (AGPL) and copyrighted to Carl Öttinger

Copyright © 2025 Carl Öttinger
