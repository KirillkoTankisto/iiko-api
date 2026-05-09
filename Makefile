SRC ?= src
INC ?= $(shell pkg-config --cflags json-c libcurl gtk4 openssl)
BUILD ?= build

CC = clang
CFLAGS ?= -mtune=generic -O3 -pipe -Wall
CLIBS = $(shell pkg-config --libs json-c libcurl gtk4 openssl)

CCC = $(CC) $(CFLAGS) $(INC)

SRC_FILES = api/api.c gui/gui.c utils/utils.c main.c
BUILD_FILES = api.o gui.o utils.o main.o

all: $(BUILD) compile

compile: $(BUILD)/iiko-office

# Final executable
$(BUILD)/iiko-office: $(BUILD)/api.o $(BUILD)/gui.o $(BUILD)/utils.o $(BUILD)/main.o
	$(CCC) -o $@ $^ $(CLIBS)

### Components
$(BUILD)/api.o: $(SRC)/api/api.c
	$(CCC) -c -o $@ $^

$(BUILD)/gui.o: $(SRC)/gui/gui.c
	$(CCC) -c -o $@ $^

$(BUILD)/utils.o: $(SRC)/utils/utils.c
	$(CCC) -c -o $@ $^

$(BUILD)/main.o: $(SRC)/main.c
	$(CCC) -c -o $@ $^
###

# Build directory
$(BUILD):
	mkdir -p $(BUILD)

clean: $(BUILD)
	rm -rf $(BUILD)

.PHONY: all compile clean
