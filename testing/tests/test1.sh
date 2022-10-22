printf "Test 1 [Basic]... "

{
  Test() {
    $FAKE exec
    [ $? -eq $OK ] && return 0 || return 1
  }

  $FAKE
  [ $? -eq $INCORRECT_CALL ] || exit 1

  cd test1

  $FAKE
  [ $? -eq $INCORRECT_CALL ] || exit 1

  Test &&
    Test &&
    rm ./main && Test &&
    rm ./main && Test &&
    rm ./main.o && Test
} 1>>log1.txt 2>>log1.txt

[ $? -eq 0 ] && printf "passed\n" || printf "failed\n"
