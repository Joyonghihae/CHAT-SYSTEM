# FILE		: makefile
# PROJECT	: CanWeTalk
# PROGRAMMER	: Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
# FIRST VERSION	: 2023-03-18
# DESCRIPTION	: This makefile is to compile and link chat-client application

# FINAL BINARY TARGET
./bin/chat-client.o: ./obj/chat-client.o ./obj/clientOperation.o
	cc ./obj/chat-client.o ./obj/clientOperation.o ../Common/obj/common.o -o ../Common/bin/chat-client -lncurses -lpthread

# DEPENDENCIES
./obj/chat-client.o: ./src/chat-client.c ./inc/chat-client.h
	cc -c ./src/chat-client.c -o ./obj/chat-client.o

./obj/clientOperation.o: ./src/clientOperation.c ./inc/chat-client.h
	cc -c ./src/clientOperation.c -o ./obj/clientOperation.o

# OTHER TARGETS
all: ./bin/chat-client

clean:
	rm ./obj/*
