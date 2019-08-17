#!/usr/bin/env bash

BUILD_CONFIG="Debug"
DRY_RUN=

###############################################################################
# Function Name : usage()
# Arguments     : N/A
# Return Result : N/A, exit 0
###############################################################################
usage()
{
  echo "   Usage:  test.sh [-hdrn] [-- <ctest options>]"
  echo ""
  echo "     -h : usage help"
  echo "     -d : use Debug build config (default)"
  echo "     -r : use Release build config (Debug if not defined)"
  echo "     -n : dry run (prints what it is going to do)"
  echo "     -- : anything after -- will be passed to ctest"
  exit 0
}

# get script options
while getopts "hdrn" opt ; do
  case $opt in
    h  ) usage
         exit 0
         ;;
    d  ) BUILD_CONFIG="Debug"
         ;;
    r  ) BUILD_CONFIG="Release"
         ;;
    n  ) DRY_RUN=echo
         ;;
    \? ) usage
         exit 1
         ;;
  esac
done

# correct the index so that we are now at $1
shift $(($OPTIND - 1))

# extract ctest options
CTEST_OPTS=()

while [[ $# -gt 0 ]]; do
  CTEST_OPTS+=("$1")
  shift
done

${DRY_RUN} cmake --build . --config $BUILD_CONFIG --target test_vst3
${DRY_RUN} cd jamba-test-plugin
${DRY_RUN} ctest -C $BUILD_CONFIG "${CTEST_OPTS[@]}"