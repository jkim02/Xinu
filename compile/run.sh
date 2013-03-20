echo "]0;Running XINU...
[1;32mRunning[31m XINU[0m...
" &&\
qemu-system-arm -M versatilepb -cpu arm1176 -m 128M -serial stdio -kernel xinu.bin
echo "\
"
