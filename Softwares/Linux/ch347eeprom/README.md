ch347eeprom
===

A libusb based programming tool for 24Cxx serial EEPROMs using the WinChipHead CH347 IC

![WinChipHead ch347A based programmer](https://raw.github.com/commandtab/ch347eeprom/master/pics/programmer.jpg)


**Usage**

```
ch347eeprom - an i2c EEPROM programming tool for the WCH ch347a IC
Version 0.6 copyright (c) 2011 asbokid <ballymunboy@gmail.com>

This program comes with absolutely no warranty; This is free software,
and you are welcome to redistribute it under certain conditions:
GNU GPL v3 License: http://www.gnu.org/licenses/gpl.html

Usage:
 -h, --help              display this text
 -v, --verbose           verbose output
 -d, --debug             debug output
 -s, --size              size of EEPROM {24c01|24c02|24c04|24c08|24c16|24c32|24c64|24c128|24c256|24c512|24c1024}
 -e, --erase             erase EEPROM (fill with 0xff)
 -p, --speed             i2c speed (low|fast|high) if different than standard which is default
 -w, --write  <filename> write EEPROM with image from filename
 -r, --read   <filename> read EEPROM and save image to filename
 -V, --verify <filename> verify EEPROM contents against image in filename

Example: ch347eeprom -v -s 24c64 -w bootrom.bin
```

**Author**

Originally written by [asbokid](http://sourceforge.net/projects/ch341eepromtool/) and released under the terms of the GNU GPL, version 3, or later. Modifications by [command-tab](https://github.com/command-tab) to make it work under OS X.

**License**

This is free software: you can redistribute it and/or modify it under the terms of
the latest GNU General Public License as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.


**Tested**

| Name              | Write | Read |
|-------------------|-------|------|
| CAT24M01WI        |  OK   |  OK  |
| AT24C256          |  OK   |  OK  |
| Microchip 24LC256 |  OK   |  OK  |
| AT24C32N          |  OK   |  OK  |
| AT24C08           |  OK   |  OK  |