configure:
	CXX=/usr/bin/clang++ cmake -S . -B build

compile: configure
	cmake --build build
	cp build/compile_commands.json compile_commands.json

test: compile
	(cd build; ctest)

run: compile
	(./build/repl-sql-autocomplete)

clean:
	rm -rf build
	rm -rf .cache
	rm -rf compile_commands.json

download-antlr:
	wget https://www.antlr.org/download/antlr-4.13.1-complete.jar -O build/antlr-4.13.1-complete.jar
