
cleanpath: cleanpath.o list.o

.PHONY: clean
clean:
	-rm cleanpath *.o

.PHONY: format
format:
	astyle --options=.astylerc cleanpath.c list.c list.h

.PHONY: test
test: cleanpath
	@./test.sh
