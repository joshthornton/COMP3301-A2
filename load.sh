set e

# init
make all > /dev/null
sudo insmod cryptodev-1.0/cryptodev.ko 
sudo insmod crypto.ko
sudo mknod /dev/crypto c 250 0
sudo chmod a+rw /dev/crypto

# build tests
make -C ./test > /dev/null

# new line
echo ""

# Run tests
./test/test "TEST ONE:	PASSED" 
./test/test2 "TEST TWO:	PASSED" 
./test/test3 "TEST THREE:" "	PASSED" 
./test/test4 `cat "test/test4-cipher.txt"` 
./test/test5 `cat "test/test5-cipher.txt"`
./test/test6 "TEST SIX:	PASSED"
./test/test7 "TEST SEVEN:" "	PASSED" 
#./test/test8 "TEST" " EIGHT:" "	PASSED" "" 
./test/test9 "TEST NINE:	PASSED" 
./test/test10 "TEST TEN:	PASSED" 

# new line
echo ""

# destroy
sudo rm -f /dev/crypto
sudo rmmod crypto
sudo rmmod cryptodev
make clean > /dev/null
make -C ./test clean > /dev/null

# print log
sudo dmesg -c
