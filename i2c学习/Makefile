i2c_demo:main.o i2c.o
	$(CC)	-Wall	main.o i2c.o -o	elf1_cmd_i2c
main.o:main.c i2c.h
	$(CC)	-c	-Wall	main.c	-o	main.o
i2c.o:i2c.c
	$(CC)	-c	-Wall	i2c.c	-o	i2c.o
clean:
	$(RM) *.o	elf1_cmd_i2c

