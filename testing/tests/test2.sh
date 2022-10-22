printf "Test 2 [Cyclic dependencies]... "

{
  Test() {
    $FAKE "$1" 2>out.txt
    ret=$?
    grep "Self-dependence" <out.txt
    self_dep_ret=$?
    grep "decoy" <out.txt
    decoy_ret=$?
    [ $ret -eq "$DEPENDENCY_ERROR" ] && [ $self_dep_ret -eq 0 ] && [ $decoy_ret -eq 1 ] && return 0 || return 1
  }

  cd test2 || return 1

  Test exec &&
  Test build &&
  Test compile &&
  Test phony2
} 1>>log2.txt 2>>log2.txt

tests=$?
grep "decoy" < ../log2.txt >> grep_log.txt # should not contain

[ $? -eq 1 ] && [ $tests -eq 0 ] && printf "passed\n" || printf "failed\n"
