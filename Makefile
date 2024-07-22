configure:
	CXX=/usr/bin/clang++ cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build

compile:
	cmake --build build
	cp build/compile_commands.json compile_commands.json

test:
	(cd build; ctest)

run:
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
		YQL.g4 \
		YQL.g4
