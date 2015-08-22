How to build :

> CentOS Linux :

  * install libevent2 (http://libevent.org/)
  * yum install expat-devel gcc-c++ cmake
  * cmake ./;make or make -f Makefile.old

> OSX 10.9 :

  * port install libevent cmake
  * cmake ./;make or make -f Makefile.old

> SunOS :

  * make -f Makefile.old

How to Run :

> Server :

  * goto server/
  * modify server.ini file
    * SERVER\_PORT : listen port
    * USE\_SWAP : allow page in/out
    * INIT\_COUNT : when table is created, reserved memory row count
    * INCLEMENT\_COUNT : when reserved memory shotage, next inclement count
    * INCLEMENT\_PERCENT : maximum reserved memory usage.
> > > when approached to that percent, reserved memory incleased.
  * run ./bmdb


> Client :

  * goto client/
  * subcless Servermanager, ConnectServer to your program.
  * example is MyClient.cpp
  * embedding to your program.