# Bound Check Error

Tools that manage to find the bounds error:
- GCC / Clang __with__ fsanatizer=address or fsanitizer=undefined
- Coverity

Tools that don't find it:
- Cppcheck
- GCC / Clang
- Valgrind
- Clang-check
- Clang-tidy

> In certain cases the compiler generates faulty code

## Stack sanitizer
```
=================================================================
==5940==ERROR: AddressSanitizer: global-buffer-overflow on address 0x000000601080 at pc 0x00000040073d bp 0x7fff05a5f880 sp 0x7fff05a5f878
READ of size 4 at 0x000000601080 thread T0
    #0 0x40073c in get_domain (/home/bravl/Code/OPS/c_test/test+0x40073c)
    #1 0x400775 in main (/home/bravl/Code/OPS/c_test/test+0x400775)
    #2 0x7fe25ad53f89 in __libc_start_main (/lib64/libc.so.6+0x20f89)
    #3 0x400639 in _start (/home/bravl/Code/OPS/c_test/test+0x400639)

0x000000601080 is located 0 bytes to the right of global variable 'bla' defined in 'c_test.c:14:24' (0x601060) of size 32
SUMMARY: AddressSanitizer: global-buffer-overflow (/home/bravl/Code/OPS/c_test/test+0x40073c) in get_domain
Shadow bytes around the buggy address:
  0x0000800b81c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b81d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b81e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b81f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8200: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0000800b8210:[f9]f9 f9 f9 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8220: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8230: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8240: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8250: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0000800b8260: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==5940==ABORTING
```


## Wrongly generated ASM
```
[0x0040c318]>  # sym.ipc_get_domain (int32_t arg1);
 --------------------------------------.
|  0x40c318                            |
| (fcn) sym.ipc_get_domain 40          |
|   sym.ipc_get_domain (int32_t arg1); |
| ; arg int32_t arg1 @ x0              |
| adrp x1, 0x829000                    |
| add x1, x1, 0xbb8                    |
| add x1, x1, 0x40                     |
| ; arg1                               |
| mov w3, w0                           |
| movz w0, 0                           |
`--------------------------------------'
    v
    |
    '---------------.
                    |
                    |
                    |
              .-----'
       .--------.
       |      | |
       |.-----------------------.
       ||  0x40c32c             |
       || ldr w2, [x1], 0x28    |
       || add w0, w0, 1         |
       || cmp w2, w3            |
       || b.ne 0x40c32c         |
       |`-----------------------'
       |        f t
       |        | |
       `----------'
             .--'
             |
         .--------------------.
         |  0x40c33c          |
         | ret                |
         `--------------------'
```

## Correct generated code
```
[0x0040c418]>  # sym.ipc_get_domain (int32_t arg1);
  .--------------------------------------.
  |  0x40c418                            |
  | (fcn) sym.ipc_get_domain 60          |
  |   sym.ipc_get_domain (int32_t arg1); |
  | ; arg int32_t arg1 @ x0              |
  | adrp x1, 0x828000                    |
  | add x1, x1, 0xbb0                    |
  | add x1, x1, 0x40                     |
  | movz w2, 0x1                         |
  `--------------------------------------'
      v
      |
      '.
.--------.
|      | |
|.-----------------------------------------.
||  0x40c428                               |
|| ldr w3, [x1]; ../src/core-ipc.c:-196    |
|| add x1, x1, 0x28                        |
|| ; arg1                                  |
|| cmp w3, w0                              |
|| b.eq 0x40c450                           |
|`-----------------------------------------'
|        f t
|        | |
|        | '---------------------.
|      .-'                       |
|      |                         |
|  .--------------------.    .--------------------.
|  |  0x40c438          |    |  0x40c450          |
|  | add w2, w2, 1      |    | mov w0, w2         |
|  | cmp w2, 8          |    | ret                |
|  | b.ne 0x40c428      |    `--------------------'
|  `--------------------'
|        t f
|        | |
`--------' |
       .---'
   .--------------------.
   |  0x40c444          |
   | movn w0, 0         |
   | ret                |
   `--------------------'
```
