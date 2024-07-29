# Definindo o compilador e as flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -pthread

# Arquivos fontes e objetos
SRCS = SleepServer.cpp Discovery.cpp Management.cpp Monitoring.cpp Interface.cpp Server.cpp Client.cpp global.cpp
OBJS = $(SRCS:.cpp=.o)

# Nome do executável
TARGET = SleepServer

# Regra principal para criar o executável
all: $(TARGET)

# Regra para criar o executável a partir dos objetos
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regra para compilar os arquivos fonte em objetos
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar arquivos objetos e o executável
clean:
	rm -f $(OBJS) $(TARGET)
