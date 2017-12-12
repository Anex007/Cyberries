slow-loris = attacks/slow-loris.c
syn-flood  = attacks/syn-flood.c 
packet = packet.c

cyberry-bot: main.c $(packet) packet.h  $(syn-flood) attacks/syn-flood.h $(slow-loris) attacks/slow-loris.h
	gcc main.c $(packet) $(syn-flood) $(slow-loris) -o cyberry-bot -lpthread
