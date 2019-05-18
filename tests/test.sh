for f in *.m19; do ../m19 --target asm $f; done
for c in *.asm; do yasm -felf32 $c ; done
for b in *.o; do ld -m elf_i386 -o "$bu" $b ; done
for e in *.ou; do ./$e > "$et" ; done
for o in *.out; do diff $o expected/$o; done;	
for r in *.asm; do rm $c ; done
for r in *.o; do rm $c ; done
for r in *.executable; do rm $c ; done