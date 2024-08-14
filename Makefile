CC = g++-11
CFLAGS = -Wall -Wextra -std=c++11 -pthread

mainApp: mainApp.cpp
	$(CC) $(CFLAGS) -o mainApp mainApp.cpp

clean:
	rm -f mainApp
	
