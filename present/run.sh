g++ -O1 -g -Wall -Werror -lm main.cpp -lcrypto
valgrind --log-file="res.txt" --tool=cachegrind --I1=32768,8,64 --D1=32768,8,64 --LL=1048576,16,64 ./a.out ../vit_normal.ppm
# g++ compute.cpp -o compute
cg_annotate cachegrind.out.* main.cpp > cg.txt
rm cachegrind.out.*
./compute