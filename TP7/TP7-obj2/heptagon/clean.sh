
rm -rf scheduler_c
rm -rf scheduler_data_c
rm -rf externc_c

rm -f *.o
rm -f *.out
rm -f main.out
rm -f a.out

rm -f *.mls
rm -f *.log
rm -f *.epci

find . -maxdepth 1 -type f -name "*.c" ! -name "main.c" ! -name "externc.c" -delete
find . -maxdepth 1 -type f -name "*.h" ! -name "externc.h" ! -name "externc_types.h" -delete

