#!/bin/bash

SRC=tema
TESTS=tests
OUT=out
PYTHON_CMD=python3

# Cleanup the previous run's temporary files
rm -f ${TESTS}/*.out.sorted

# Run tests
for i in {1..9}
do
   rm -f "${TESTS}/0$i".out
   echo "Starting test $i"
   ${PYTHON_CMD} test.py "${TESTS}/0$i.in" > "${TESTS}/0$i.out"
   echo "Finished test $i"
   ${PYTHON_CMD} check_test.py $i "${TESTS}/0$i.out" "${TESTS}/0$i.ref.out"
done

rm -f "${TESTS}/10".out
echo "Starting test 10"

${PYTHON_CMD} test.py "${TESTS}/10.in" > "${TESTS}/10.out"
echo "Finished test 10"
${PYTHON_CMD} check_test.py 10 "${TESTS}/10.out" "${TESTS}/10.ref.out"

# Pylint checks - the pylintrc file being in the same directory
# Uncoment the following line to check your implementation's code style :)
${PYTHON_CMD} -m pylint ${SRC}/*.py