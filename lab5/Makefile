CC = g++ -std=c++0x -g 
DEPS = eventhandler.hh process.hh scheduler.hh clock.hh
OBJ = eventhandler.o process.o scheduler.o clock.o


# $@ : lhs
# $< : first item in rhs
# $< : complete rhs

%.o : %.cpp $(DEPS)
	$(CC) -c -o $@ $< 

main.out: $(OBJ)
	$(CC) -o $@ $(OBJ)


.PHONY: clean
clean:
	rm -rf a.out
	rm -rf *.o

