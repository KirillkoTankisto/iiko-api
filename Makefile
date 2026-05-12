SRC ?= src
INC ?= $(shell pkg-config --cflags json-c libcurl gtk4 openssl)
BUILD ?= build

CC = clang
CFLAGS ?= -mtune=generic -O3 -pipe -Wall
CLIBS = $(shell pkg-config --libs json-c libcurl gtk4 openssl)

CCC = $(CC) $(CFLAGS) $(INC)

SRC_FILES = $(SRC)/api/api.c $(SRC)/gui/cashshifts.c $(SRC)/gui/common.c $(SRC)/gui/gui.c $(SRC)/gui/login.c $(SRC)/gui/main_view.c $(SRC)/utils/utils.c $(SRC)/main.c
OBJ_FILES = $(BUILD)/api/api.o $(BUILD)/gui/cashshifts.o $(BUILD)/gui/common.o $(BUILD)/gui/gui.o $(BUILD)/gui/login.o $(BUILD)/gui/main_view.o $(BUILD)/utils/utils.o $(BUILD)/main.o

all: $(BUILD) compile

compile: $(BUILD)/iiko-office

# Final executable
$(BUILD)/iiko-office: $(OBJ_FILES)
	$(CCC) -o $@ $^ $(CLIBS)

### Components

$(BUILD)/%.o: $(SRC)/%.c
	dir=`dirname "$@"` && mkdir -p "$$dir"
	$(CCC) -c -o $@ $<

###

# Build directory
$(BUILD):
	mkdir -p $(BUILD)

clean: $(BUILD)
	rm -rf $(BUILD)

.PHONY: all compile clean
