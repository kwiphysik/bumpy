ECHO bumpy firmware burning script
avrdude.exe -c stk500v2 -P COM4 -p m8 -U lfuse:w:0xdf:m -U hfuse:w:0xc9:m  -C ../etc/avrdude.conf
