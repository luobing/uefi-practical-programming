.rdata
helloStr: .asciiz  "Hello, UEFI world!\n"
lengthStr: .word .-helloStr
  .text 
  .globl  main
  .ent  main
main:
  li $a0,1
  dla $a1,helloStr
  lw $a2,lengthStr
  li $v0,5001
  syscall
  li $a0,0
  li $v0, 5058
  syscall
  .end main