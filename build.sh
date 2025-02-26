if [ -z "$1" ]; then
    echo "Error: Invalid Argument"
    echo "Usage: $0 <executable_name>"
    exit 1
fi

cd ./build
cmake ..
make
cp "$1" ../bin

# DEBUGGING
mv "$1" ../