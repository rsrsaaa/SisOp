CC = g++
CFLAGS = -Wall -Wextra -std=c++11

mainApp: mainApp.cpp
	$(CC) $(CFLAGS) -o mainApp mainApp.cpp

clean:
	rm -f mainApp
	