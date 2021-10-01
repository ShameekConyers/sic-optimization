#!/bin/bash

cd build &&
cmake --build . &&
./sic_optim &&
cd ..
