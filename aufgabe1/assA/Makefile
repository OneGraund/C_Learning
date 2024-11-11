
.PHONY: all compile docs clean cleeean

all: compile docs

compile: mygrep_comp.o
	gcc -o mygrep mygrep_comp.o

debug: mygrep_debug.o
	gcc -g -fsanitize=address -o mygrep mygrep_debug.o

mygrep_comp.o: mygrep.c mygrep.h
	gcc -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L \
		 -c mygrep.c -o mygrep_comp.o

mygrep_debug.o: mygrep.c mygrep.h
	gcc -DDEBUG -g -Wall -fsanitize=address -c mygrep.c -o mygrep_debug.o

docs:
	# Check if doxygen is available
	@if command -v doxygen >/dev/null 2>&1; then \
		echo "Doxygen found, generating documentation..."; \
		doxygen Doxyfile && make -C ./docs/latex; \
	else \
		echo "Doxygen not found. Skipping documentation generation."; \
	fi

assignment:
	tar -cvzf exercise_1a.tar.gz Makefile Doxyfile mygrep.c mygrep.h

clean:
	rm -rf *.o mygrep

cleeean:
	rm -rf *.o *.txt *.md mygrep exercise_1a.tar.gz ./docs
