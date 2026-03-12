CC= gcc
CFLAGS= -Wall -Wextra
TARGET= riscv

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c  -o $(TARGET) 

clean:
	rm -f $(TARGET) 

# target: dependencies
# 	command

# - target: what you want to build (e.g., riscv executable)
# - dependencies: what files are needed (e.g., main.c)
# - command: how to build it (e.g., gcc ...)
