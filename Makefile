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

build/antlr-4.13.1-complete.jar:
	wget https://www.antlr.org/download/antlr-4.13.1-complete.jar -O build/antlr-4.13.1-complete.jar

antlr-generate: build/antlr-4.13.1-complete.jar
	cd src/sql/antlr && java \
		-jar ../../../build/antlr-4.13.1-complete.jar \
		-listener -visitor -Dlanguage=Cpp \
		SQLiteLexer.g4 \
		SQLiteParser.g4
