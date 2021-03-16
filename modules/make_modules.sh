#!/bin/sh
for dir in ./*; do (cd "$dir" && ./configure && make && make install); done
