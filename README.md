### nonBlockingIO

Use by 

`$ mkfifo /tmp/pipe1` 

`$ </tmp/pipe1 app1 | app2 >/tmp/pipe1`


Whatever gets sent view cout the other will receive via cin
