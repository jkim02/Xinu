echo "]0;Cleaning XINU...
[1;32mCleaning[31m XINU[0m...
"
make clean &&\
echo "]0;Building XINU...
[1;32mBuilding[31m XINU[0m...
" &&\
make &&\
echo "]0;Running XINU...
[1;32mRunning[31m XINU[0m...
" &&\
qemu-system-arm -M versatilepb -cpu arm1176 -m 128M -serial stdio -kernel xinu.bin
echo "\
"
