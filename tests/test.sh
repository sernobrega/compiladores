for f in *.m19; do ../m19 --target asm $f; done
for c in *.asm; do yasm -felf32 $c ; done
for b in *.o;
do 
	echo "Processing $b..."
	ld -m elf_i386 -o "$bu" $b -lrts
done
for e in *.ou
do 
	echo "Executing $e..."
	./$e > "$et" 
	diff "$et" expected/"$et"
done
