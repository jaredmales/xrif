# Lookup table generators

These standalone programs generate the lookup tables used in xrif.c by include.  They are provided here for completeness and documentation, but do not normally need to be run as part of building xrif.

## bitshift_and_nibbles_lupgen.c

This program creates the lookup table `bitshift_and_nibbles`, used in the `bytepack renibble` reordering method.  To create the lookup table do
```
$ gcc -o bitshift_and_nibbles_lupgen bitshift_and_nibbles_lupgen.c
$ ./bitshift_and_nibbles_lupgen > bitshift_and_nibbles.inc
```

## bit_to_position_lupgen.c

This program creates the lookup table `bit_to_position`, used in the `bitpack` reordering method.  To create the lookup table do
```
$ gcc -o bit_to_position_lupgen bit_to_position_lupgen.c
$ ./bit_to_position_lupgen > bit_to_position.inc
```

## set_bits_lupgen.c

This program creates the lookup table `set_bits`, used in the `bitpack` reordering method.  To create the lookup table do
```
$ gcc -o set_bits_lupgen set_bits_lupgen.c
$ ./set_bits_lupgen > set_bits.inc
```

