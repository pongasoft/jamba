#!/usr/bin/env bash

# from http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
BASEDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

echo "============================================================="
echo "==                                                         =="
echo "==              Generating Makefiles...                    =="
echo "==              -----------------------                    =="
echo "============================================================="
${BASEDIR}/configure.sh Release

cd build/Release

echo "============================================================="
echo "==                                                         =="
echo "==              Running tests..........                    =="
echo "==              -----------------------                    =="
echo "============================================================="
./test.sh
if [ $? -eq 0 ]
then
  echo "[Tests ran successfully]"
else
  echo "Failure while running tests... aborting..."
  exit 1
fi

echo "============================================================="
echo "==                                                         =="
echo "==              Validating plugin......                    =="
echo "==              -----------------------                    =="
echo "============================================================="
./validate.sh
if [ $? -eq 0 ]
then
  echo "[Validation ran successfully]"
else
  echo "Failure while validating plugin... aborting..."
  exit 1
fi

echo "============================================================="
echo "==                                                         =="
echo "==              Building archive.......                    =="
echo "==              -----------------------                    =="
echo "============================================================="
cmake --build . --target archive
