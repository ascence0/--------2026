demo name : spi_demo
demo ver : sv01.001
first edition compilation time :2022/01/21

note:

1)different platforms have to choose different cross-compilation .you need to modify makefile redefine "CC".
2)function describe : read regs ,write regs .
3)if you want to save the data received from the serial port in file,you must set"-f path".
  for example:"-f /home/root/",then the receive data save in /home/root/receive_data.txt.
4)about mode sampling data location
  cpol = 0 cpha = 0         cpol = 0 chap = 1       cpol = 1 cpha = 0         cpol = 1 chap = 1    

     |____                       ____|              ____|    _____             ____     |____
     |    |                     |    |                  |    |                     |    |
  ___|    |___               ___|    |___               |____|                     |____|
     |                               |                  |                               |
5)about dual spi :command standard spi , addr standard spi ,data dual spi.
6)about quad spi :command standard spi , addr standard spi ,data quad spi.

some more examples: