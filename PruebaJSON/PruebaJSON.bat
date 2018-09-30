gcc -Wall -o "./run/PruebaJSON" ^
    "PruebaJSON.c" ^
    -I "/usr/include/json-c" ^
    -L /usr/lib ^
    -l json-c

set /p in=">>> "
