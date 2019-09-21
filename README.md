# Accumulator as a Linux kernel module

This is a toy linux kernel module that implements a character device
(123, 0) that keeps an integer. Writing an integer (as a string) to
the device will add the integer to the kept integer. Reading it will
give the current value of the kept integer in as a string.

## Setup

    $ make
    $ sudo insmod accumulator.ko
    $ sudo mknod /dev/accumulator c 123 0
    $ sudo chmod ag+w /dev/accumulator
    
## Run

    $ cat /dev/accumulator
    0
    $ echo 2 > /dev/accumulator
    $ cat /dev/accumulator
    2
    $ echo 2 > /dev/accumulator
    $ cat /dev/accumulator
    4
    $ echo -5 > /dev/accumulator
    $ cat /dev/accumulator
    -1
