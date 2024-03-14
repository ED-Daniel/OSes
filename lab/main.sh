counter=0

if [ -s "output.txt" ]
then
    echo "Deleting existing file"
    rm -f "output.txt"
fi

readfilename() {
    echo 'Enter file name: '
    read filename

    while [[ -z "$filename" || ! -r "$filename" || ! -s "$filename" ]]
    do
        echo 'File does not exist or permission was denied!'
        echo 'Enter file name: '
        read filename
    done
}

readpattern() {
    echo 'Enter searching pattern (q - quit): '
    read -r pattern

    while [[ -z "$pattern" ]]
    do
        echo 'Enter non-empty string!'
        echo 'Enter searching pattern (q - quit): '
        read -r pattern
    done
}

sigtrap() {
    counter=`expr $counter + 1`

    if [ $counter -gt 10 ]
    then
        echo 'Execution terminated due to more than 10 interrupt signals received'
        exit 1
    fi

    echo '\nReceived CTRL+C: Re-enter your input!'
}

trap sigtrap SIGINT

while :
do
    readfilename
    readpattern
    
    if [[ $pattern == "q" ]]
    then
        break
    fi

    result=`grep "$pattern" "$filename"`
    (sh ./writer.sh "$result")

    filename=""
    pattern=""
done

wait
echo "Program terminated"
