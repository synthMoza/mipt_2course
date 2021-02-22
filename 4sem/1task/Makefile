CC = gcc
CFLAGS = -Wall -g -MD
MC = valgrind
MCFLAGS = -s
RMEXEC = exec code_coverage unit_tests_mc
RMFILES = *.d *.o *.gcda *.gcno *.info
RMDIR = html/*

all: exec

# Target for using tree in main.c
exec: main.o avltree.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Check unit tests for memory leaks
memcheck: unit_tests_mc
	$(MC) $(MCFLAGS) ./$<

# Compile unit tests
unit_tests_mc: unit_tests.o avltree.o
	$(CC) -o $@ $^

# Check the code coverage of this program
code_coverage: unit_tests.c avltree.c
	@ $(CC) -o $@ $^ --coverage
	@ echo "LAUNCHING UNIT TESTS..."
	@ ./$@
	@ lcov -c -d . -o ccoverage.info
	@ genhtml -o html/ ccoverage.info  
	@ firefox html/index.html

# Clean all executable and temporary files, clean directories
clean:
	@ rm -rf $(RMEXEC) $(RMFILES)
	@ rm -rf $(RMDIR)

-include *.d