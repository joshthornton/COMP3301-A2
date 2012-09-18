CC=gcc
CFLAGS=-g
LDFLAGS=-g
LIBS=-pthread
SRC =./
BIN =./

all: echat test test2 test3 test4 test5 test6 test7 test8 test9 test10

echat: $(BIN)echat.o
	$(CC) $(LDFLAGS) -o $(BIN)echat $(BIN)echat.o $(LIBS)

$(BIN)echat.o: $(SRC)echat.c
	$(CC) $(CFLAGS) -c $(SRC)echat.c -o $(BIN)echat.o

test: $(BIN)test.o
	$(CC) $(LDFLAGS) -o $(BIN)test $(BIN)test.o $(LIBS)

$(BIN)test.o: $(SRC)test.c
	$(CC) $(CFLAGS) -c $(SRC)test.c -o $(BIN)test.o

test2: $(BIN)test2.o
	$(CC) $(LDFLAGS) -o $(BIN)test2 $(BIN)test2.o $(LIBS)

$(BIN)test2.o: $(SRC)test2.c
	$(CC) $(CFLAGS) -c $(SRC)test2.c -o $(BIN)test2.o

test3: $(BIN)test3.o
	$(CC) $(LDFLAGS) -o $(BIN)test3 $(BIN)test3.o $(LIBS)

$(BIN)test3.o: $(SRC)test3.c
	$(CC) $(CFLAGS) -c $(SRC)test3.c -o $(BIN)test3.o

test4: $(BIN)test4.o
	$(CC) $(LDFLAGS) -o $(BIN)test4 $(BIN)test4.o $(LIBS)

$(BIN)test4.o: $(SRC)test4.c
	$(CC) $(CFLAGS) -c $(SRC)test4.c -o $(BIN)test4.o

test5: $(BIN)test5.o
	$(CC) $(LDFLAGS) -o $(BIN)test5 $(BIN)test5.o $(LIBS)

$(BIN)test5.o: $(SRC)test5.c
	$(CC) $(CFLAGS) -c $(SRC)test5.c -o $(BIN)test5.o

test6: $(BIN)test6.o
	$(CC) $(LDFLAGS) -o $(BIN)test6 $(BIN)test6.o $(LIBS)

$(BIN)test6.o: $(SRC)test6.c
	$(CC) $(CFLAGS) -c $(SRC)test6.c -o $(BIN)test6.o

test7: $(BIN)test7.o
	$(CC) $(LDFLAGS) -o $(BIN)test7 $(BIN)test7.o $(LIBS)

$(BIN)test7.o: $(SRC)test7.c
	$(CC) $(CFLAGS) -c $(SRC)test7.c -o $(BIN)test7.o

test8: $(BIN)test8.o
	$(CC) $(LDFLAGS) -o $(BIN)test8 $(BIN)test8.o $(LIBS)

$(BIN)test8.o: $(SRC)test8.c
	$(CC) $(CFLAGS) -c $(SRC)test8.c -o $(BIN)test8.o

test9: $(BIN)test9.o
	$(CC) $(LDFLAGS) -o $(BIN)test9 $(BIN)test9.o $(LIBS)

$(BIN)test9.o: $(SRC)test9.c
	$(CC) $(CFLAGS) -c $(SRC)test9.c -o $(BIN)test9.o

$(BIN)test10.o: $(SRC)test10.c
	$(CC) $(CFLAGS) -c $(SRC)test10.c -o $(BIN)test10.o

clean:
	rm -rf $(BIN)*.o
	rm -f $(BIN)test $(BIN)test2 $(BIN)test3 $(BIN)echat $(BIN)test4 $(BIN)test5 $(BIN)test6 $(BIN)test7 $(BIN)test8 $(BIN)test9 $(BIN)test10


