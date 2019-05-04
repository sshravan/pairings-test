```
git clone --recursive git@github.com:sshravan/pairings-test.git
cd pairings-test
git submodule update --init --recursive
rm -rf build && mkdir build && cd build && cmake ..
make 
./src/main 2
#rm -rf build && mkdir build && cd build && cmake .. && make
```
```
# in the root dir
cmake -H. -Bdebug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
ln -s debug/compile_commands.json
```

valgrind and GDB

```bash

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./src/main


gdb ./src/main

```


```
# Root directory
cmake -H. -Bdebug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
% ln -s debug/compile_commands.json
```
