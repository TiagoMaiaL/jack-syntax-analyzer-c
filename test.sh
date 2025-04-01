cd "`dirname $0`"

artifacts="artifacts"

if ! [ -d ./$artifacts ]; then
    mkdir $artifacts
fi

cd $artifacts

source_files=(
    '../src/parser.c'
    '../src/tokenizer.c'
    '../src/file-handler.c'
)
test_files=(
    '../tests/main.c'
    '../tests/test.c'
    '../tests/utils.c'
    '../tests/test-tokenizer.c'
    '../tests/test-parser.c'
)
files=("${source_files[@]}" "${test_files[@]}")

args=""

if [ -f ./_test ]; then
    rm _test
fi

for file in "${files[@]}"; do
    args+=$file' '
done

if clang -g -Wall -o _test $args; then
    ./_test
fi

