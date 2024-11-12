
.PHONY: all 

all: clean compile

compile: writer.o reader.o
	gcc -o writer writer.o
	gcc -o reader reader.o

reader.o: reader.c circular_buffer.h
	gcc -std=c99 -g -DDEBUG -Wno-format -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -o reader.o -c reader.c

writer.o: writer.c circular_buffer.h 
	gcc -std=c99 -g -DDEBUG -Wno-format -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -o writer.o -c writer.c 


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
	clear

push: clean
	git add .
	git commit -m "make push commit"
	git push -u origin main
