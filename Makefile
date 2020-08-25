exec = holyc.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -g -Wall -lm -ldl -fPIC -rdynamic -std=c99


$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

libholyc.a: $(objects)
	ar rcs $@ $^

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

install:
	make
	make libholyc.a
	mkdir -p /usr/local/include/holyc
	cp -r ./src/include/* /usr/local/include/holyc/.
	cp ./libholyc.a /usr/local/lib/.
	cp ./holyc.out /usr/local/bin/holyc

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o

lint:
	clang-tidy src/*.c src/include/*.h
