g++ -O1 -g -Wall -Werror -lm test.cpp -lcrypto
valgrind --log-file="res.txt" --tool=cachegrind --I1=32768,8,64 --D1=32768,8,64 --LL=1048576,16,64 ./a.out 