cd "`dirname $0`"

artifacts="artifacts"

if ! [ -d ./$artifacts ]; then
    mkdir $artifacts
fi

cd $artifacts
rm -rf ./*

source_files=(
    '../src/main.c'
    '../src/file-handler.c'
)

files=("${source_files[@]}")

args=""

if [ -f ./JackAnalyzer ]; then
    rm JackAnalyzer
    rm -rf *.dSYM
fi

for file in "${files[@]}"; do
    args+=$file' '
done

clang -g -Wall -o JackAnalyzer $args
