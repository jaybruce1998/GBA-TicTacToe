CC = arm-none-eabi-gcc
CFLAGS = -mthumb-interwork -mthumb -O2 -Wall

LIBGBA = /opt/devkitpro/libgba/lib
INCLUDES = /opt/devkitpro/libgba/include

# Object files
OBJS = main.o

# Target executable
TARGET = tictactoe.gba

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CRT0) $(OBJS) -L$(LIBGBA) -lgba -specs=gba.specs -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

