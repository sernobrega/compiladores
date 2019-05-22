passed=0
total=0

for f in *.m19; 
do 
	FILE="${f%%.*}"
	../m19 --target asm $f;
	yasm -felf32 "$FILE.asm"
	if [[ "$?" -eq "0" ]]; then
		printf "YASM: OK, " 
	else 
		printf "YASM: Failed, ";

	ld -m elf_i386 -o "$FILE" "$FILE.o" -lrts
	if [[ "$?" -eq "0" ]]; then
		echo "LD: OK." 
	else 
		echo "LD: Failed.";

	./$FILE > "$FILE.out"

	DIFF=$(diff -w -E -B  "$FILE.out" expected/"$FILE.out")

	if [ "$DIFF" != "" ]
	then
		echo "!!!!! Test $FILE didn't pass."
		echo "$DIFF"
	else
		passed=$((passed+1))
	fi

	total=$((total+1))

	rm "$FILE.asm" "$FILE.o" "$FILE.out" "$FILE"
done

echo "Passed: $passed/$total"
