COUNTER=1
FAILED=0
COMPILERFAIL=()
YASMFAIL=()
LDFAIL=()
DIFFFAIL=()

passed=0
total=0

for f in *.m19; 
do 
	# detecta numero do teste
	FILENAME=$(basename $file)
	NUM=`echo "$FILENAME" | cut -d'-' -f3`
	
	# Se foi fornecido um intervalo
	if [[ -n "$1" && -n "$2" ]]; then
		# limite inferior
		if [ "$NUM" -lt "$1" ]; then
		continue
		fi
		
		#limite superior
		if [ "$NUM" -gt "$2" ]; then
		break
		fi
	fi
	
	# apenas 1 argumento, numero de testes a correr
	if [[ -n "$1" && -z "$2" ]]; then
		if [ "$COUNTER" -gt "$1" ]; then
			break
		fi
	fi
	

	# comando a ser executado
	NAME=`echo "$file" | cut -d'.' -f1`
	N=`echo "$FILENAME" | cut -d'.' -f1`
	
	if [[ "$COUNTER" -eq "1" ]]; then
		echo "-----------------------------------------------------"
	fi
	
	# executar o compilador
	printf "%s : %s " "$COUNTER" "$N"
	{ ../m19 --target asm $file; } >& "$NAME.output";
	if [[ "$?" -eq "0" ]]; then
		printf "..... Compiler: OK, " 
	else 
		printf "..... Compiler: Failed, ";
		COMPILERFAIL+=("$N")
		let FAILED=FAILED+1
	fi
	
	yasm -felf32 "$FILE.asm"
	if [[ "$?" -eq "0" ]]
	then
		printf "YASM: OK, " 
	else 
		printf "YASM: Failed, "
	fi

	ld -m elf_i386 -o "$FILE" "$FILE.o" -lrts
	if [[ "$?" -eq "0" ]]
	then
		echo "LD: OK." 
	else 
		echo "LD: Failed."
	fi

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
