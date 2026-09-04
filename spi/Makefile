spi_demo:main.o spi.o file.o
	$(CC)	-Wall	main.o spi.o file.o	-o	elf1_cmd_spi
main.o:main.c spi.h file.h
	$(CC)	-c	-Wall	main.c	-o	main.o
spi.o:spi.c
	$(CC)	-c	-Wall	spi.c	-o	spi.o
file.o:file.c file.h
	$(CC)	-c	-Wall	file.c	-o	file.o
clean:
	$(RM) *.o	elf1_cmd_spi

