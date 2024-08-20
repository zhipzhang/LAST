#!/bin/bash

mkdir -p build && cmake -B build -S .

make -C build -j$(nproc)

make -C build install

