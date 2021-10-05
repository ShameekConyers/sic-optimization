#!/bin/bash

cd build &&
cmake build .. &&
cmake --build . &&
./sic_optim &&
cd ..
