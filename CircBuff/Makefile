
.PHONY: all 

all: clean compile push

compile: writer.o reader.o
	gcc -o writer writer.o
	gcc -o reader reader.o

reader.o: reader.c circular_buffer.h
	gcc -std=c99 -g -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -c reader.c -o reader.o

writer.o: writer.c circular_buffer.h 
	gcc -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -c writer.c -o writer.o


docs:
	# Check if doxygen is available
	@if command -v doxygen >/dev/null 2>&1; then \
		echo "Doxygen found, generating documentation..."; \
		doxygen Doxyfile && make -C ./docs/latex; \
	else \
		echo "Doxygen not found. Skipping documentation generation."; \
	fi

clean:
	rm -rf *.o writer reader

push:
	git add .
	git commit -m "make push commit"
	git push -u origin main
