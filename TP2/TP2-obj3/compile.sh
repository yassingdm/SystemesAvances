heptc -c extern.epi
heptc -target c hs_handler_merge.ept
gcc -Wall -c main.c -I . -I ./hs_handler_merge_c -I "$(opam var lib)/heptagon/c"
gcc -Wall -c extern.c -I . -I ./hs_handler_merge_c -I "$(opam var lib)/heptagon/c"
gcc -Wall -c hs_handler_merge_c/hs_handler_merge.c -I . -I ./hs_handler_merge_c -I "$(opam var lib)/heptagon/c"
gcc -Wall main.o extern.o hs_handler_merge.o -o main.out -I . -I ./hs_handler_merge_c -I "$(opam var lib)/heptagon/c" -lm
./main.out
