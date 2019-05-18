for f in *.m19; 
do 
	FILE="${FILE%%.*}"
	../m19 --target asm $f;
	yasm -felf32 "$FILE.asm"
	echo "Creating executable of $FILE"
	ld -m elf_i386 -o "$FILE" "$FILE.o" -lrts
	echo "Executing $FILE"
	./$FILE > "$FILE.out"
	diff "$FILE.out" excepted/"$FILE.out"
done

