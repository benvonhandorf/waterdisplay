# Overview

Water Display is intended to be an open source project combining electronics, embedded software and water to create a small water display for our small pond.

# Project goals

As with any project, there are multiple goals here, both primary and secondary

## Primary
* Functional, programmable water display.
* Further my knowledge of electronics.
* Gain some experience circuit board design, using KiCad.

## Secondary
* Minimize cost.  In particular, cost per node should be kept as low as possible.
* Maximize flexibility.  Creating an extensible design capable of doing other tasks in the future would be optimal.
* Learn a new microcontroller, if the opportunity presents itself.

# Design Philosophy

The goal is to have multiple small independent water squirters (a _node_) located around the pond.  Each of these will contain:
* (A small 12V solenoid)[https://www.sciplus.com/p/12-VDC-SOLENOID-VALVE-10PACK-SAVINGS_54414]
* RGB LED
  * WS2812 seems likely
  * APA102 would also work and we have no shortage of GPIO in the node (see below)

These will be run from a central controller running various patterns which could be based on sensors or some other option.

# Component selection

## General

As part of the goal to do circuit board design, most components will be sourced as SMD, where possible.  A few power components will be through hole (e.g. MOSFET, flyback diode) just to give a ridiculous amount of rating overhead.

## Node design

After consideration, I've decided to pursue making each node smart.  This minimizes the required bus lines, maximizes the expandability of the installation and hopefully allows for different use cases for the electronic work.

Intended communication is over a shared serial UART from the controller to all nodes.  Each node will have an address assigned to it (compiled in to the flash) and the protocol will allow it to only process messages intended for it.

To further reduce cabling requirements, each node will have a local 5V regulator for Vdd supply for the microcontroller and RGB LED.

### Microcontroller

General Requirements:
* UART
* 5V part (to eliminate the need for further regulation)
* At least 3 GPIO pins
  * MOSFET
  * LED Data
  * LED Clock, if an APA102 style RGB LED is used

In order to keep cost per part down and given the very modest requirements above, an STM8 type part is recommended.  Research indicates they can be sourced at less than $0.50/pc in small quantities.  Given the small amount of board space available (with an optimally sized circuit board) and the need to hand-solder the parts, a TSSOP20 part is suggested.  RAM and Flash requirements should be very modest, to a STM8S003F or similar part should be sufficient in all respects.  Even if I decided to make a 4-way node, there should still be sufficient GPIO available.

Given the lack of additional requirements, where possible GPIO pins should be selected to maximize future expandability.  SPI and I2C should be left available, if possible, along with ADC pins.  

#### Microcontroller supporting parts

* 100 uF ceramic capacitor - Power smoothing Capacitor 
* Cext - 470-3300 nF 
  * 0.3 Ohm Max ESR at 1MHz 
  * 15 nH Max ESL at 1MHz
* If an external oscillator is necessary:
  * 16 MHz crystal
  * 2 x 20pF Load capacitors

### Connectivity

Connections from central node are:
* +12V DC
* 0V DC
* Serial Data (TX from controller)
* Pressurized water (obviously.  Solenoid ratings call for < 4psi)

Additional local connections for programming and debugging (not exposed through weatherproof casing)
* Serial Data (TX from node)
* Reset
* SWIM
* +5VDC
* Ground

### Additional

In order to make the design as maintainable as possible, the following indicator LEDs will be placed on the board as well:
* Green power indicator LED, connected to 12V line
* Red "active" LED, connected to MOSFET control pin

# BOM
## Per Node BOM
* Microcontroller
  * STM8S003F ~ $0.50/pc
  * STM8S103F ~ $0.50/pc
  * MSP430G2001 ~ $0.
* AMS1117 ~ $0.10/pc
* 2x female spade connectors ~ $0.28/pc (e.g. 1217080-1)[http://www.newark.com/webapp/wcs/stores/servlet/ProductDisplay?catalogId=15003&langId=-1&urlRequestType=Base&partNumber=30H5445&storeId=10194]
* 