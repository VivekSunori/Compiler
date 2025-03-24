# CompilerX

CompilerX is a compiler that compiles cx code into Assembly code using flex and bison in C. The purpose of this compiler is to learn how to use flex and bison in C and how to write a compiler for your own language in C. It is also a good example of how to use flex and bison in C to write a compiler.

## Installation

To install CompilerX, you need to have flex and bison installed on your computer. You can download flex and bison from [here](https://www.gnu.org/software/flex/bison.html).

## Usage

To Edit & Recompile CompilerX, you need to run the following command:

```
$ flex <lexerFiles>
$ gcc compiler.c -o cmpx
```
- `lexerFiles` is the lexer files which are in the `COMPILERX/utils` directory
- `cmpx` is the executable file, put it in `bin` directory in the `COMPILERX` directory to use it in the command line using $PATH variable

This will compile the compiler and the lexer and the parser. You can then run the compiler with the following command:

```
$ cmpx <file.cx>
```

This will compile the file.cx file into LLVM bytecode and print the LLVM bytecode to the screen.
- Note that this is a work in progress and it is under development.

## Examples

You can find some examples of cx code below.
```
# A simple program that prints the sum of two numbers in cx
int x = 10;
int y = 5;

print x;
print y;

x = x + y;

print x;

int z = x * 2;

print z;
```

## How Our Compiler Works


```
.
└── CompilerX/
    ├── bin/
    │   └── cmpx.exe # Executable compiler file for $PATH variable
    ├── components/
    │   ├── ast.h
    │   ├── tokens.c
    │   ├── tokens.h
    │   └── ...Other header files for declarations
    ├── logs/ # To handle error logs
    ├── scripts/ # Various auto scripts
    ├── utils/
    │   ├── codePreprocessor.l # Lexical file for comment and spaces remove.
    │   ├── lex.yy.c # Lexical c file to perform lexical analysis.
    │   └── lexer.l # Lexical file for lexical analysis
    ├── .gitignore
    ├── changes.md # Maintain at every PR
    ├── cmpx.exe # Only for testing duration
    ├── compiler.c # Main file to compile compiler
    ├── parser.c # For analysis will be splitted into modules soon.
    └── semantic.c # For analysis adn AST to Assembly code generation.

```
### **Roles of Each Component:**  

#### 1️⃣ **Lexer (Lexical Analyzer)**  
- **Role:** Scans the source code and breaks it into tokens.  
- **Tokens:** `Identifiers`, `Keywords`, `Operators`, `Numbers`.  
- **Example:**  
  ```plaintext
  int x = 5;
  ```
  becomes:  
  ```
  <KEYWORD: int> <ID: x> <ASSIGN: => <NUMBER: 5>
  ```
- **Lex Usage:**  
   - Define patterns (e.g., `int`, `float`, `if`, `while`).  
   - Handle whitespace and comments.  
   - Output tokens for the parser.

---

#### 2️⃣ **Parser (Syntax Analyzer)**  
- **Role:** Analyzes token sequences based on grammar rules.  
- **Uses:** Recursive Descent Parsing or `yacc/bison` with Lex.  
- **Checks:** Correct structure like `if-else` blocks or expressions.  
- **Example:**  
   - Grammar for assignment:  
     ```
     statement -> ID ASSIGN expression
     expression -> term (OPERATOR term)*
     term -> ID | NUMBER
     ```
- **Error Handling:** Detects `Syntax Error` if grammar is incorrect.

---

#### 3️⃣ **Semantic Analyzer**  
- **Role:** Ensures meaningful code.  
- **Tasks:**  
  - Type Checking.  
  - Symbol Table management.  
  - Scope validation.  
- **Example:**  
   - Invalid code:  
     ```c
     int x = "hello"; // Type mismatch
     ```

---

#### 4️⃣ **Compiler (`compiler.c`)**  
- **Role:** Coordinates everything.  
- **Responsibilities:**  
  - Calls `lexer.c` to tokenize.  
  - Passes tokens to `parser.c` to validate structure.  
  - Performs semantic checks.  
  - Generates intermediate/assembly code.  
- **Output:** Machine-executable code or instructions.

---

### Reference
- [Parsing expressions by precedence climbing](https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing)
- [CS 340: Lecture 6: Precedence Climbing, Abstract Syntax Trees](https://ycpcs.github.io/cs340-fall2018/lectures/lecture06.html)
- Recursive Descent Parser in C
- [Untangling Lifetimes: The Arena Allocator](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator)
--- 

### **Precedence and Arena Usage in Memory Management**  

In our project, **precedence and arena-based memory management** are used to optimize performance, reduce fragmentation, and efficiently allocate and deallocate memory.  

#### **Why Use Precedence?**  
Precedence is used to determine the priority of memory allocation strategies based on factors such as access speed, alignment, and resource constraints. It ensures that:  
- **Frequently used memory blocks** are allocated efficiently.  
- **Memory deallocation follows an ordered approach** to prevent leaks.  
- **Cache-friendly memory allocation** enhances execution speed.  

#### **Why Use Arena-Based Memory Management?**  
Arena-based memory management helps in:  
- **Fast allocation and deallocation** by managing memory in pre-allocated chunks.  
- **Minimizing fragmentation** since memory is reused efficiently.  
- **Simplified memory handling**, as all memory is freed at once when the arena is destroyed, reducing overhead.  

This approach is particularly beneficial in scenarios where frequent memory allocation and deallocation occur, such as **real-time applications, compilers, and high-performance computing tasks**.  

---

## Author
[Jeevan Joshi](https://github.com/jeevanjoshi4434)

## License

CompilerX is released under the [MIT License](https://opensource.org/licenses/MIT).

## Future work
- Convert large If-Else blocks to Switch-Case blocks for better performance.
- Bytecode Generation (Platform Independency).
- faster Execution.
