#!/bin/sh

if [ ! -d bin/module ]
then
    mkdir bin/module
else
    unlink bin/chat
    unlink bin/module/libchat.so
fi

cp Helens/bin/helens bin/chat
cp lib/libchat.so bin/module/
