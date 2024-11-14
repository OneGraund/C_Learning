CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE \
			-D_POSIX_C_SOURCE=200809L

CDFLAGS = -DDEBUG -g -Wall -fsanitize=address

.PHONY: all compile docs clean cleeean

all: compile docs

compile: compile_supervisor compile_generator

debug: debug_supervisor debug_generator

# SUPERVISOR part-------------------
compile_supervisor: supervisor_comp.o
	gcc -o supervisor supervisor_comp.o

supervisor_comp.o: supervisor.c supervisor.h
	gcc $(CFLAGS) -c supervisor.c -o supervisor_comp.o

debug_supervisor: supervisor_debug.o
	gcc -g -fsanitize=address -o supervisor supervisor_debug.o

supervisor_debug.o: supervisor.c supervisor.h
	gcc $(CDFLAGS) -c supervisor.c -o supervisor_debug.o
#-----------------------------------

# GENERATOR part-------------------
compile_generator: generator_comp.o
	gcc -o generator generator_comp.o

generator_comp.o: generator.c generator.h
	gcc $(CFLAGS) -c generator.c -o generator_comp.o

debug_generator: generator_debug.o
	gcc $(CDFLAGS) -o generator generator_debug.o

generator_debug.o: generator.c generator.h
	gcc $(CFLAGS) -c generator.c -o generator_debug.o
#-----------------------------------

#GRAPH part------------------------
debug_graph: graph_debug.o
	gcc -g -fsanitize=address -o graph graph_debug.o

graph_comp.o: graph.c graph.h header.c header.h
	gcc $(CFLAGS) -c graph.c -o graph_comp.o

graph_debug.o: graph.c graph.h header.c header.h
	gcc -DTEST $(CDFLAGS) -c graph.c -o graph_debug.o
#----------------------------------

#HEADER part-----------------------

#----------------------------------


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
