#!/bin/bash
COUNTER=1
FAILED=0
COMPILERFAIL=()
YASMFAIL=()
LDFAIL=()
DIFFFAIL=()

passed=0
total=0

for file in *.m19; 
do 
	# detecta numero do teste
	FILENAME=$($file)
	NUM=`echo "$file" | cut -d'-' -f3`
	
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
	{ ../m19 --target asm "$file"; } >& "$NAME.output";
	if [[ "$?" -eq "0" ]]; then
		printf "..... Compiler: OK, " 
	else 
		printf "..... Compiler: Failed, ";
		COMPILERFAIL+=("$N")
		let FAILED=FAILED+1
	fi
	
	 # produzir o ficheiro binario
	{ yasm -felf32 "$N.asm"; } >& /dev/null
	if [[ "$?" -eq "0" ]]; then
		printf "YASM: OK, " 
	else 
		printf "YASM: Failed, ";
		YASMFAIL+=("$N")
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

echo
echo
echo $(($COUNTER - 1)) " testes efectuados, falhas abaixo:"
echo
echo "COMPILADOR XPL:"
for i in "${COMPILERFAIL[@]}"
do
   echo "    !falha : " $i
done

echo "YASM:"
for i in "${YASMFAIL[@]}"
do
   echo "    !falha : " $i
done

echo "LD:"
for i in "${LDFAIL[@]}"
do
   echo "    !falha : " $i
done

echo "DIFF:"
for i in "${DIFFFAIL[@]}"
do
   echo "    !falha : " $i
done

echo
echo "Passam " $(($(($COUNTER - 1)) - $FAILED)) "/" $(($COUNTER - 1))
echo
echo "Nota: Se precisares podes ver os .output gerados para ver o que aconteceu durante o ../xpl/xpl file.xpl"
echo "Está tudo despachado, até à próxima!"
echo