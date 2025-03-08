CXX := clang++
CXXFLAGS := -Wall -Wextra -O3 -march=znver2 -mtune=znver2 -std=c++20 -fomit-frame-pointer -fpic -fno-stack-protector -mprefer-vector-width=512
NOSTDLIB_FLAGS := -ffreestanding -nostdlib

all: memcpy.o memcpy.so test

memcpy.o: memcpy.cc
	$(CXX) $< -o $@ -c $(CXXFLAGS) $(NOSTDLIB_FLAGS)

memcpy.so: memcpy.cc
	$(CXX) $< -o $@ -shared $(CXXFLAGS) $(NOSTDLIB_FLAGS)

test: test.cc memcpy.so
	$(CXX) $< -o $@ memcpy.so $(CXXFLAGS)

objdump:
	objdump -dMintel memcpy.o
	objdump -dMintel memcpy.so

clean:
	rm -f memcpy.so memcpy.o test

.PHONY: objdump all clean
