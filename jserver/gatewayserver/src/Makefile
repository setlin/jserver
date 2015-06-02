CC=g++ -Wall
OBJECTS=buffer.o epoll.o jsgateway.o net.o prot.o main.o handler.o init.o
TARGET=JSGateway

$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

buffer.o:buffer.h
epoll.o:epoll.h
jsgateway.o:jsgateway.h
net.o:net.h
prot.o:prot.h
handler.o:handler.h
main.o:jsstd.h
init.o:init.h

.PHONY:clean
clean:
	rm -f *.o
	rm -f $(TARGET)
	rm -f nohup.out
