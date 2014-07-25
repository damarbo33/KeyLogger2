# A simple makefile generator by KiSoft, 2010. mailto: kisoft@rambler.ru
# version: 0.3.12.5

# Project Variables start
CPP=g++
CC=gcc
LD=g++
LIB=ar
WINDRES=
# Project Variables end

# Target: Debug

OBJS_Debug=obj/Debug/hotkey.o obj/Debug/main.o obj/Debug/printscreen.o
FLAGS= -DUNIX -fexpensive-optimizations -O3 -s 

Debug: bin/Debug/KeyLogger2

bin/Debug/KeyLogger2: $(OBJS_Debug)
	@echo Building console executable bin/Debug/KeyLogger2
	@g++ -o bin/Debug/KeyLogger2 $(OBJS_Debug) -lpthread

obj/Debug/hotkey.o: hotkey.cpp
	@echo Compiling: hotkey.cpp
	@g++ -Wall   $(FLAGS) -c hotkey.cpp -o obj/Debug/hotkey.o

obj/Debug/main.o: main.cpp
	@echo Compiling: main.cpp
	@g++ -Wall   $(FLAGS) -c main.cpp -o obj/Debug/main.o

obj/Debug/printscreen.o: printscreen.cpp
	@echo Compiling: printscreen.cpp
	@g++ -Wall   $(FLAGS) -c printscreen.cpp -o obj/Debug/printscreen.o

main.cpp: hotkey.h thread.h

hotkey.cpp: hotkey.h

hotkey.h: constant.h

printscreen.cpp: printscreen.h

.PHONY: clean

clean: 
	@echo Delete $(OBJS_Debug) bin/Debug/KeyLogger2
	-@rm -f $(OBJS_Debug) bin/Debug/KeyLogger2



