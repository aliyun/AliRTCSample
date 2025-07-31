CXX      := g++
TARGET   := main
SRC      := main.cpp

# OPENSSL_CFLAGS := $(shell pkg-config --cflags openssl)
# OPENSSL_LIBS   := $(shell pkg-config --libs openssl)

PREFIX   := /opt/homebrew/opt/openssl@3
INCLUDE  := -I$(PREFIX)/include
LIBDIR   := -L$(PREFIX)/lib
LIBS     := -lcrypto -lz

CXXFLAGS := -std=c++11 $(INCLUDE)
LDFLAGS  := $(LIBDIR) $(LIBS)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(TARGET)