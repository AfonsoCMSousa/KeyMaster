if [ -z "$1" ]; then
    echo "Error: Invalid Argument"
    echo "Usage: $0 <executable_name>"
    exit 1
fi

cd ./build
cmake -DCMAKE_C_FLAGS="-fsanitize=address -g -Wall -Wextra -Wunused-result" ..
make
cp "$1" ../bin

# DEBUGGING
mv "$1" ../