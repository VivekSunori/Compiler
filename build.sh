#!/bin/bash

echo "Compiling CompilerX..."

# Run flex to generate lex.yy.c
cd utils
flex lexer.l
cd ..

# Create directories if they don't exist
mkdir -p obj
mkdir -p obj/components
mkdir -p obj/components/parsers
mkdir -p obj/utils

# Compile individual source files
gcc -c components/tokens.c -o obj/components/tokens.o
gcc -c utils/lex.yy.c -o obj/utils/lex.yy.o
gcc -c compiler.c -o obj/compiler.o
gcc -c components/memory.c -o obj/components/memory.o
gcc -c components/symbol_table.c -o obj/components/symbol_table.o
gcc -c components/ast_visualizer.c -o obj/components/ast_visualizer.o
gcc -c components/ast_json_exporter.c -o obj/components/ast_json_exporter.o
gcc -c components/parsers/parser.c -o obj/components/parsers/parser.o
gcc -c components/parsers/expressions.c -o obj/components/parsers/expressions.o
gcc -c components/parsers/statements.c -o obj/components/parsers/statements.o
gcc -c components/parsers/conditionals.c -o obj/components/parsers/conditionals.o
gcc -c components/parsers/functions.c -o obj/components/parsers/functions.o
gcc -c components/parsers/loops.c -o obj/components/parsers/loops.o
gcc -c semantic.c -o obj/semantic.o

# Link all object files
gcc obj/compiler.o obj/components/memory.o obj/components/symbol_table.o obj/components/tokens.o obj/components/ast_visualizer.o obj/components/ast_json_exporter.o obj/components/parsers/parser.o obj/components/parsers/expressions.o obj/components/parsers/statements.o obj/components/parsers/conditionals.o obj/components/parsers/functions.o obj/components/parsers/loops.o obj/semantic.o obj/utils/lex.yy.o -o cmpx

echo "Build completed!"

# Optional: Create bin directory and copy executable
mkdir -p bin
cp cmpx bin/
echo "Executable copied to bin/cmpx"

# Make the executable executable
chmod +x cmpx
chmod +x bin/cmpx