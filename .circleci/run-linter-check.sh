#!/bin/bash
set -o nounset
set -o pipefail

# Run Clangformat
cd /root/project/build
make clangformat || { echo "Make Clangformat failed!"; exit 1; }

# Check git for changes
cd /root/project
linterchecklist=`git status | grep "modified"`
if [[ -z $linterchecklist ]]; then
    echo "ClangFormat test passed!"
    exit 0;
else
    echo "You have clang-format problems :";
    for f in $linterchecklist; do
        filename=`echo $f | awk '{print $2}'`
        echo $filename
        git diff $filename
    done
    exit 1;
fi
