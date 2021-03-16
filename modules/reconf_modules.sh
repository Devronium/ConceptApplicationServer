#!/bin/sh
for dir in ./*; do (cd "$dir" && autoreconf --install); done
