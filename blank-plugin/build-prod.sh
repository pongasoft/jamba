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
${BASEDIR}/configure.sh

./build/jamba.sh prod
