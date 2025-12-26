
heptc -target c scheduler_data.ept externc.epi scheduler.ept

gcc -I. \
    -I$(opam var lib)/heptagon/c \
    -Ischeduler_c \
    -Ischeduler_data_c \
    scheduler_c/*.c \
    scheduler_data_c/*.c \
    externc.c main.c \
    -o main.out

./main.out
