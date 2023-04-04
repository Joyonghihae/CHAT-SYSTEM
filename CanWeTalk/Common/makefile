# FILE		: makefile
# PROJECT	: CanWeTalk
# PROGRAMMER	: Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
# FIRST VERSION	: 2023-03-18
# DESCRIPTION	: This makefile is to compile and link the common source modules

# FINAL BINARY TARGET
./bin/libcommon.so: ./obj/common.o
	cc -shared ./obj/common.o -o ./bin/libcommon.o

# DEPENDENCIES
./obj/common.o: ./src/common.c ./inc/common.h
	cc -c ./src/common.c -o ./obj/common.o

# OTHER TARGETS

clean:
	rm ./bin/*
	rm ./obj/*
