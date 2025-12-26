heptc -target c square.ept

gcc -I. \
    -I"$(opam var lib)/heptagon/c" \
    -Isquare_c \
    square_c/*.c \
    main.c \
    -o main.out

./main.out