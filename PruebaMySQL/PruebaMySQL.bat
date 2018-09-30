gcc -Wall -o "./run/PruebaMySQL" ^
    "PruebaMySQL.c" ^
    -I "/usr/include/mysql" ^
    -I "/usr/include/json-c" ^
    -L /usr/lib ^
    -l mysqlclient ^
    -l json-c

set /p in=">>> "
