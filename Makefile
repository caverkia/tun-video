all:
	gcc -o tapudp tapudp.c
	gcc -o tunudp tunudp.c


clean:
	rm tunudp
	rm tapudp
