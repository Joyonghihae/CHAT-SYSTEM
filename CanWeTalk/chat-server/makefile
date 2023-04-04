# FILE		: makefile
# PROJECT	: CanWeTalk
# PROGRAMMER	: Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
# FIRST VERSION	: 2023-03-18
# DESCRIPTION	: This makefile is to compile and link chat-server application

# FINAL BINARY TARGET
./bin/chat-server.o: ./obj/chat-server.o ./obj/serverOperation.o
	cc ./obj/chat-server.o ./obj/serverOperation.o ../Common/obj/common.o -o ../Common/bin/chat-server -lpthread

# DEPENDENCIES
./obj/chat-server.o: ./src/chat-server.c ./inc/chat-server.h
	cc -c ./src/chat-server.c -o ./obj/chat-server.o

./obj/serverOperation.o: ./src/serverOperation.c ./inc/chat-server.h
	cc -c ./src/serverOperation.c -o ./obj/serverOperation.o

# OTHER TARGETS
all: ../Common/bin/chat-server

clean:
	rm ./obj/*
