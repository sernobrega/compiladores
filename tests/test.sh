for f in *.m19; 
do 
	FILE="${f%%.*}"
	echo "==== Creating ASM $FILE ====="
	../m19 --target asm $f;
	yasm -felf32 "$FILE.asm"
	ld -m elf_i386 -o "$FILE" "$FILE.o" -lrts
	echo "==== Executing $FILE ====="
	./$FILE > "$FILE.out"
	diff -Z  "$FILE.out" expected/"$FILE.out"
done

