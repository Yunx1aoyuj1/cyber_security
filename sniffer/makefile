TARGET = sniffer
$(TARGET):sniffer.o
	gcc -o sniffer sniffer.o
sniffer.o:sniffer.c
	gcc -c sniffer.c
clean:
	rm $(TARGET) *.o
