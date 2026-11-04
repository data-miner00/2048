# basically a command runner
.PHONY: build run format format-check clean

build: build/Makefile
	cmake --build build

build/Makefile:
	cmake -S . -B build

run: build
	./build/2048

clean:
	@rm -rf build

format:
	find . -path ./build -prune -o \( -name '*.c' -o -name '*.h' \) -print | xargs clang-format -i

format-check:
	find . -path ./build -prune -o \( -name '*.c' -o -name '*.h' \) -print | xargs clang-format --dry-run --Werror
