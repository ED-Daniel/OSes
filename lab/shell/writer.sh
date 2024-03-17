if [ $# -eq 0 ]
then
  echo "Writing argument was none"
  exit 1
fi

echo "$1" >> output.txt

echo "Data has been written to output.txt"
