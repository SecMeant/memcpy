MARCH := znver2
CXX := clang++
CXXFLAGS := -Wall -Wextra -O3 -march=$(MARCH) -mtune=$(MARCH) -std=c++20
CXXFLAGS += -fno-stack-protector -fomit-frame-pointer -fPIC
CXXFLAGS += -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-exceptions
NOSTDLIB_FLAGS := -ffreestanding -nostdlib

all: memcpy.o memcpy.so

memcpy.o: memcpy.cc
	$(CXX) $< -o $@ -c $(CXXFLAGS) $(NOSTDLIB_FLAGS)

memcpy.so: memcpy.cc
	$(CXX) $< -o $@ -shared $(CXXFLAGS) $(NOSTDLIB_FLAGS)

test: test.cc memcpy.o
	$(CXX) $< -o $@ memcpy.o $(CXXFLAGS) -static

objdump:
	objdump -dMintel memcpy.o
	objdump -dMintel memcpy.so

clean:
	rm -f memcpy.so memcpy.o test

.PHONY: objdump all clean
