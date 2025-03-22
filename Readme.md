# minimal-watch
This repo contains hardware and software for a minimalistic smartwatch. 

## Design goals

* step counter
* battery life of >= one year
* NFC data exchange
* powered by a coin cell
* fits a Casio F-91 W housing
* binary or hex time display using 12 bicolor LEDs
* brightness sensor to dim LEDs

![minimal-watch](https://raw.githubusercontent.com/bkarl/minimal-watch/main/hardware/img/render.jpg)

## Issues in hw v0.0.1
* swapped SCL/SDA lines
* missing pull up on NFC interrupt line (BOOT0 does not have an internal pull up)
* wakeup sources (BMA400 or NFC) are not connected to WKUP pins so standby mode is not possible
* resistor for green LEDs is too large
* NFC antenna circuit is not working (coil had to be crafted by hand and glued to housing)
