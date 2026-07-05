# c-gradebook-analyzer
A small command-line gradebook analyzer written in C. This project reads student grades from a CSV file, calculates weighted final grades, assigns letter grades, displays class statistics, searches students by ID, sorts records by total grade, and saves a report to a text file.

How to Compile
Using GCC:

gcc -std=c11 -Wall -Wextra -pedantic -g main.c -o gradebook
Or using the provided Makefile:

make
How to Run
./gradebook sample_students.csv
Or:

make run
Debugging with GDB
Compile with debug symbols using:

make
Then run:

gdb ./gradebook
Example GDB commands:

break main
run sample_students.csv
next
print students.size
continue
