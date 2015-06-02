CC=g++ -Wall
OBJECT=jsdb.o jsaccountinfo.o jsgatewayinfo.o login.o buffer.o epoll.o net.o prot.o main.o handler.o init.o jstest.o 
TARGET=JSLogin

$(TARGET):$(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) -L /usr/local/mysql/lib -lmysqlclient

jsdb.o:jsdb.h
jsaccountinfo.o:jsaccountinfo.h
jsgatewayinfo.o:jsgatewayinfo.h
login.o:login.h
buffer.o:buffer.h
epoll.o:epoll.h
net.o:net.h
prot.o:prot.h
handler.o:handler.h
main.o:jsstd.h
init.o:init.h
jstest.o:jstest.h

.PHONY:clean
clean:
	rm -f *.o
	rm -f $(TARGET)
	rm -f nohup.out
