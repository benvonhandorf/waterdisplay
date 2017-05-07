# Purpose

This protocol is designed to be used for the controller to node communication in the water display.  Limitations will primarily be driven by the nodes, since they are the more processor limited side of the communication.

Note that this is not intended to limit the TX side from the individual nodes, which will be primarily used for debugging and should be free to speak whatever is most useful.

# Serial Communication Settings

For the development state at least, communication will be at 9600 N-8-1 to allow for easiest debugging.  If this results in too much lag in reality higher baud rates will be used.

# Node Addressing

The STM8 supports inherently dealing with node addressing by using Address Mark Detection (See reference manual, page 334).  The MSB of the first byte in the Address byte must be 1 and the 4 LSBs will be compared with the ADD bits in UART_CR4.  This un-mutes that receiver until a different address byte is received.  This allows the possibility of transmitting multiple commands without re-addressing the node.

A limitation of this approach is that there is no support for broadcast packets.  This may wind up being an issue for large coordinated changes of many nodes at once.  If so, this can either be addressed by increased transmission speeds or by revisiting the mute mode used (or not using any mute mode at all).

# General Protocol Design

## Limitations

Due to the addressing mode choices above, non-address bytes may not have their MSB set.  This limits us to bytes between 0x00 and 0x7F for commands and data.  Addresses bytes will be between 0x80 and 0x8F, limiting the network to 16 nodes with this version of the protocol.  Future versions not using the address muting of the STM8 may be able to use up to 127 nodes without violating the MSB

To allow expandability of the network, some consideration must be given to message compactness while still hopefully allowing for node growth and enhanced capabilities (e.g. multiple sprayers and LEDs).

## Node Addressing

A node will be muted, listening only until its address byte is seen.  After a node is addressed it will continue to listen to commands until another node is addressed, at which point it will enter mute mode again.

## Command Structure

Generally a command will be followed by 0-n data bytes defining some aspect of the behavior.

### General Commands

#### Command: `D`
Data Bytes: 0

##### Meaning
Dump current state to TX pin.  The node should output its current internal state and any useful debugging information to the serial interface.  The format of this data should be as human readable as possible but does not have any defined requirements.

#### Command: `F`
Data Bytes: 0

##### Meaning
Off.  Shut down all peripherals on the node.  Solenoids should be de-energized, LEDs should be turned off.

### Solenoid Commands

#### Command: `S`
Data Bytes: 1

##### Meaning
Set solenoids to the specified state, with 0 representing de-energized and 1 representing energized.  Each solenoid on a sprayer will be represented by a single bit, so up to 8 solenoids are supported by a single node using this command.

### LED Commands

#### Command: `L`
Data Bytes: 1 Count Byte followed by (3 * Count) data bytes.
Each set of 3 data bytes shall be in R G B order and represent a *Color Packet*.

##### Meaning
Set LEDs to a specific RGB code, instantaneously.  For nodes with multiple LEDs the first *Color Packet* should correspond with the LSB solenoid, to the extend possible.  Subsequent color packets should correspond to more significant solenoids, as makes sense.

Since each color byte must NOT have the MSB set, each byte should be right shifted by 1 by the transmitter and left shifted by 1 by the receiver.  This results in a loss of 1 bit of accuracy for each channel, but given the general limitations of RGB LEDs this should be fairly acceptable.

There is no assumption that the number of LEDs is equal to the number of solenoids, just that their physical direction corresponds roughly when appropriate.

For nodes speaking to hardware with non R G B color order of bytes, it is reliant upon the node to handle the appropriate re-ordering of bits.

#### Command: `l`
Data Bytes: 1 duration byte (max 127), 1 Count Byte (max 127) followed by (3 * Count) data bytes.
Each set of 3 data bytes shall be in R G B order and represent a *Color Packet*.

##### Meaning
Fade LEDs to a specific RGB code.  First byte represents 50ms increments to use for fade duration, all subsequent bytes are as `L`

