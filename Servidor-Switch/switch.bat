gcc -Wall -o "./run/Servidor-Switch" ^
    "Servidor-Switch.c" ^
    ./object/*.o ^
    -I "/usr/include/json-c" ^
    -L /usr/lib ^
    -l json-c

set /p in=">>> "
