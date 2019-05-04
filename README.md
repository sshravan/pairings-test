```
git clone --recursive git@github.com:sshravan/name.git
cd name
git submodule update --init --recursive
rm -rf buid
mkdir build && cd build && cmake ..
make
./src/main

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
