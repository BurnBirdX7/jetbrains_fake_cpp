export FAKE
FAKE="$(pwd)/build/fake"

[ ! -x "$FAKE" ] && echo "You have to build it with ./build.sh first..." && exit 1

export OK=0
export INCORRECT_CALL=1
export DEPENDENCY_ERROR=2
export EXECUTION_ERROR=3

cd testing || exit 2
[ -d "stage" ] && rm -rf "stage"
mkdir stage

cp -r tests stage && cd stage/tests || exit 2

./test1.sh && ./test2.sh
