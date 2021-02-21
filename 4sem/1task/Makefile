CC = gcc
CFLAGS = -Wall -g -MD
MC = valgrind
MCFLAGS = -s

all: exec

# Target for using tree in main.c
exec: main.o avltree.o
	$(CC) -o $@ $^

# Check the code for memory leaks
memcheck: unit_tests_mc
	$(MC) $(MCFLAGS) ./$<
	@ rm -rf $^

unit_tests_mc: unit_tests.o avltree.o
	$(CC) -o $@ $^

# Check the code coverage of this program
code_coverage: unit_tests.c avltree.c
	@ $(CC) -o $@ $^ --coverage
	@ echo "LAUNCHING UNIT TESTS..."
	@ ./code_coverage
	@ lcov -c -d . -o ccoverage.info
	@ genhtml -o html/ ccoverage.info  
	@ firefox html/index.html
	@ rm -rf *.o

# Clean all executable and temporary files
.PHONY: clean
clean:
	@ rm -rf exec code_coverage *.d *.o *.gcda *.gcno *.info
	@ rm -rf html/*

-include *.d