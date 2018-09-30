gcc -Wall -o "./run/PruebaPostgres" ^
    "PruebaPostgres.c" ^
    -I "/usr/include/postgresql" ^
    -I "/usr/include/json-c" ^
    -L /usr/lib ^
    -l pq ^
    -l json-c

set /p in=">>> "
