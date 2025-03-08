CXX := clang++
CXXFLAGS := -Wall -Wextra -O3 -march=znver2 -mtune=znver2 -nostdlib -ffreestanding -std=c++20 -fomit-frame-pointer -fpic -fno-stack-protector -mprefer-vector-width=512

all: memcpy.o memcpy.so

memcpy.o: memcpy.cc
	$(CXX) $< -o $@ -c $(CXXFLAGS)

memcpy.so: memcpy.cc
	$(CXX) $< -o $@ -shared $(CXXFLAGS)

objdump:
	objdump -dMintel memcpy.o
	objdump -dMintel memcpy.so

clean:
	rm -f memcpy.so memcpy.o

.PHONY: objdump all clean
