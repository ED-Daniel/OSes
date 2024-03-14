counter=0

if [ -s "output.txt" ]
then
    echo 'Deleting existing output.txt file'
    rm -f "output.txt"
fi

if [[ ! -s 'writer.sh' || ! -x 'writer.sh' ]]
then
    echo 'File writer.sh does not exist or not executable - exitting...'
    exit 1
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

    if [[ -z "$filename" ]]
    then
        echo '\nEnter file name: '
    fi

    if [[ -z "$pattern" && ! -z "$filename" ]]
    then
        echo '\nEnter searching pattern (q - quit): '
    fi    

    # echo '\nReceived CTRL+C: Re-enter your input!'
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
