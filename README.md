# laminator

Microcontroller firmware for controlling universal hot laminator like TRACER TRL-A4

It has motor for turning rubber drums with heating element.

Temperature is controlled by 2 NC thermostats effectively maintaining around 150 deg Celsius.

4 buttons and a 2x8 LCD display is connected to board, allowing setting and reading operation parameters.

Available settings are drum move time (0.5s - 1.5s) and drum stop time (0s - 10s).

In particular, if stop time is set to 0, motor is running indefinitely.
