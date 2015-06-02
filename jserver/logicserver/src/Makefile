CC=g++ -Wall -g

OBJECTS=jsdb.o buffer.o epoll.o jsserver.o jslogdb.o net.o prot.o main.o handler.o  jsutil.o \
		./model/char/jschardb.o ./model/char/jschar.o \
		./model/chat/jschat.o \
		./model/login/jslogin.o \
		./model/friend/jsfrienddb.o ./model/friend/jsfriend.o \
		init.o

TARGET=JSServer

$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) -L /usr/local/mysql/lib -lmysqlclient

jsdb.o:jsdb.h
buffer.o:buffer.h
epoll.o:epoll.h
jsserver.o:jsserver.h
jslogdb.o:jslogdb.h
net.o:net.h
prot.o:prot.h
handler.o:handler.h
jsutil.o:jsutil.h
main.o:jsstd.h
./model/char/jschardb.o:./model/char/jschardb.h
./model/char/jschar.o:./model/char/jschar.h
./model/chat/jschat.o:./model/chat/jschat.h
./model/login/jslogin.o:./model/login/jslogin.h
./model/friend/jsfrienddb.o:./model/friend/jsfrienddb.h
./model/friend/jsfriend.o:./model/friend/jsfriend.h
init.o:init.h

.PHONY:clean
clean:
	rm -f *.o
	rm -f ./model/chat/*.o
	rm -f ./model/login/*.o
	rm -f ./model/char/*.o
	rm -f ./model/friend/*.o
	rm -f $(TARGET)
	rm -f tmp*
	rm -f nohup.out
