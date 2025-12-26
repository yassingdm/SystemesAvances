heptc -c print.epi
heptc -target c gnc.ept
gcc -Wall -c main.c -I . -I ./gnc_c -I "$(opam var lib)/heptagon/c"
gcc -Wall -c print.c -I . -I ./gnc_c -I "$(opam var lib)/heptagon/c"
gcc -Wall -c gnc_c/gnc.c -I . -I ./gnc_c -I "$(opam var lib)/heptagon/c"
gcc -Wall main.o print.o gnc.o -o main.out -I . -I ./gnc_c -I "$(opam var lib)/heptagon/c" -lm
./main.out
