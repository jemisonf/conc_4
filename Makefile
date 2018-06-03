defaut: smokers barbershop
smokers:
	gcc -Wall -std=c99 -pthread smokers.c -o smokers
barbershop:
	gcc -Wall -std=c99 -pthread barbershop.c -o barbershop
custom_barbershop:
	gcc -Wall -std=c99 -pthread -D NUM_CHAIRS=$(NUM_CHAIRS) barbershop.c -o barbershop
clean:
	rm -f barbershop smokers
