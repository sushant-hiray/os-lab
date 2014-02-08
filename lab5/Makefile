CC = g++ -std=c++0x -g 
DEPS = eventhandler.hh process.hh scheduler.hh clock.hh Event.h
OBJ = eventhandler.o process.o scheduler.o clock.o Event.o asgn5.cpp


# $@ : lhs
# $< : first item in rhs
# $< : complete rhs

%.o : %.cpp $(DEPS)
	$(CC) -c -o $@ $< 

main.out: $(OBJ)
	$(CC) -o $@ $(OBJ)


.PHONY: clean
clean:
	rm -rf *.out
	rm -rf *.o

