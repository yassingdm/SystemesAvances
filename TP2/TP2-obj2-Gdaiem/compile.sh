heptc -c extern.epi
heptc -target c hs_handler.ept
gcc -Wall -c main.c -I . -I ./hs_handler_c -I "$(opam var lib)/heptagon/c"
gcc -Wall -c extern.c -I . -I ./hs_handler_c -I "$(opam var lib)/heptagon/c"
gcc -Wall -c hs_handler_c/hs_handler.c -I . -I ./hs_handler_c -I "$(opam var lib)/heptagon/c"
gcc -Wall main.o extern.o hs_handler.o -o main.out -I . -I ./hs_handler_c -I "$(opam var lib)/heptagon/c" -lm
./main.out
