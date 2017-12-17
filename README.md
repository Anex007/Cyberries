# Cyberries
A lightweight Botnet made in C. Safe to use on Client side, because of the lack of Remote Access to the machine.
Only works in Linux.

## NOTE: Will not be looking into fixing or even changing parts of the code until February

### Compilation
#### Bots
	cd bots/src
	make

#### Bot-Master
	cd bot-master/src
	make


### How To Use
#### Bots
	Type in the IP of the bot-master to connect to inside the bots/src/main.c on page 11 from 127.0.0.1 to that IP. 

#### Bot-Master
	Make sure to have a WAN ip for the bot's to connect to, or must be in the same network.

### Uses
	-> Limited access to Clients machine
	-> Now only supporting
		-> Syn-Flood
		-> SlowLoris
	-> Can only DDOS prebuild types. Not able to use a different DDOS type other than the one defined previously.

#### Command line controlling coming soon...

##### We need your help to improve and fix the bugs. Thank You. Help us add more types of DDOS scripts.

