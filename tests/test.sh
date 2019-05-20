passed=0
total=0

for f in *.m19; 
do 
	FILE="${f%%.*}"
	../m19 --target asm $f;
	yasm -felf32 "$FILE.asm"
	ld -m elf_i386 -o "$FILE" "$FILE.o" -lrts
	./$FILE > "$FILE.out"

	DIFF=$(diff -Z  "$FILE.out" expected/"$FILE.out") 
	if [ "$DIFF" != "" ] 
	then
		echo "Test $FILE didn't pass."
	fi

	if [ "$seconds" -eq 0 ]; then
		echo "Test $FILE didn't pass."
	else
		passed=$((passed+1))
	fi
	
	total=$((total+1))
done

echo "Passed: $passed/$topass"
