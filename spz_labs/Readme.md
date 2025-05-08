# This is `pro` labs

### To build run 
```bash
$ mkdir build
$ cd build
$ cmake -G "Visual Studio 17 2022" .. -DCMAKE_BUILD_TYPE=Release
```
### Then build specific lab
``` bash
$ cmake --build . --target lab_one --parallel --config Release 
$ cmake --build . --target lab_two --parallel --config Release 
$ cmake --build . --target lab_two_single_thd --parallel --config Release 
$ cmake --build . --target lab_three --parallel --config Release 
$ cmake --build . --target lab_four --parallel --config Release 
```
### or simply build them all
``` bash
$ cmake --build . --parallel --config Release 
```