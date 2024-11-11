
.PHONY: all compile docs clean cleeean

all: compile docs

#compile: mygrep_comp.o
#	gcc -o mygrep mygrep_comp.o

#debug: mygrep_debug.o
#	gcc -g -fsanitize=address -o mygrep mygrep_debug.o

#mygrep_comp.o: mygrep.c mygrep.h
#	gcc -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE \
#			-D_POSIX_C_SOURCE=200809L -c mygrep.c -o mygrep_comp.o

#mygrep_debug.o: mygrep.c mygrep.h
#	gcc -DDEBUG -g -Wall -fsanitize=address -c mygrep.c -o mygrep_debug.o


compile: compile_supervisor compile_generator


compile_supervisor: supervisor_comp.o
	gcc -o supervisor supervisor_comp.o

supervisor_comp.o: supervisor.c supervisor.h
	gcc -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE \
			-D_POSIX_C_SOURCE=200809L -c supervisor.c -o supervisor_comp.o

debug: debug_supervisor debug_generator

debug_supervisor: supervisor_debug.o
	gcc -g -fsanitize=address -o supervisor supervisor_debug.o

supervisor_debug.o: supervisor.c supervisor.h
	gcc -DDEBUG -g -Wall -fsanitize=address -c supervisor.c -o supervisor_debug.o



compile_generator: generator_comp.o
	gcc -o generator generator_comp.o

generator_comp.o: generator.c generator.h
	gcc -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE \
			-D_POSIX_C_SOURCE=200809L -c generator.c -o generator_comp.o

debug_generator: generator_debug.o
	gcc -g -fsanitize=address -o generator generator_debug.o

generator_debug.o: generator.c generator.h
	gcc -DDEBUG -g -Wall -fsanitize=address -c generator.c -o generator_debug.o

docs:
	# Check if doxygen is available
	@if command -v doxygen >/dev/null 2>&1; then \
		echo "Doxygen found, generating documentation..."; \
		doxygen Doxyfile && make -C ./docs/latex; \
	else \
		echo "Doxygen not found. Skipping documentation generation."; \
	fi

#assignment:
#	tar -cvzf exercise_1a.tar.gz Makefile Doxyfile mygrep.c mygrep.h

clean:
	rm -rf *.o generator supervisor
	clear

#cleeean:
#	rm -rf *.o *.txt *.md mygrep exercise_1a.tar.gz ./docs
