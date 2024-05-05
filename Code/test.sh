#!/bin/bash
./parser ../Test/test1.cmm output1.ir
echo "===============================" >> output1.ir
echo $(date) >> output1.ir