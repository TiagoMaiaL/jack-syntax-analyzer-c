cd "`dirname $0`"

artifacts="artifacts"

if ! [ -d ./$artifacts ]; then
    mkdir $artifacts
fi

cd $artifacts
rm -rf ./*

# TODO: Add source files to be compiled
source_files=()
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
