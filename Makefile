
TARGET_DIR = build
TEST_DIR = test
APP_NAME = smelter

CC=gcc
SRC= src/main.c

all:
	mkdir -p $(TARGET_DIR)
	
	$(CC) `pkg-config --cflags gtk+-3.0` \
	-o $(TARGET_DIR)/$(APP_NAME) \
	$(SRC) \
	`pkg-config --libs gtk+-3.0` \
	-export-dynamic
	
test:
	mkdir -p $(TEST_DIR)
	
	$(CC) `pkg-config --cflags gtk+-3.0` \
	-g -o $(TEST_DIR)/$(APP_NAME) \
	$(SRC) \
	`pkg-config --libs gtk+-3.0` \
	-export-dynamic
		
clean:
	rm -fr $(TARGET_DIR)
	rm -fr $(TEST_DIR)
	
