# Accumulator as a Linux kernel module

This is a toy linux kernel module that implements a character device
(123, 0) that keeps an integer initialized to 0 at module load. Writing an integer (as a string) to
the device will add the written integer to the kept integer. Reading the device will
give the current value of the kept integer as a string.

## Setup

    $ make
    $ sudo insmod accumulator.ko
    $ sudo mknod /dev/accumulator c 123 0
    $ sudo chmod ga+w /dev/accumulator
    
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
