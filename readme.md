This is an implementation of the cigarette smokers and barbershop problems from the Little Book of semaphores.

To build all files, run `$ make`. By default, the barbershop problem uses 4 chairs; however, you may customize it to use a custom number of chairs by first setting the `$NUM_CHAIRS` variable, then running `make custom_barbershop`. For example, to use 12 chairs instead of 4, you would run `$ export NUM_CHAIRS=12` then `make custom_barbershop`.
