heptc -target c first.ept
gcc -Wall  main.c first_c/first.c -o main.out -I ./first_c -I "$(opam var lib)/heptagon/c"
./main.out
