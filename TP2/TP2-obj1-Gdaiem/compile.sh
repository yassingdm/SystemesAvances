heptc -target c rcounter.ept
gcc -Wall  main.c rcounter_c/rcounter.c -o main.out -I ./rcounter_c -I "$(opam var lib)/heptagon/c"
./main.out