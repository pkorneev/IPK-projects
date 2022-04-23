#IPK: projekt č. 1
Lightweight server v jazyce C.

#Knihovny potřebné k překladu
-#include<stdio.h>
-#include<string.h>	 
-#include<stdlib.h>
-#include<sys/socket.h>
-#include<arpa/inet.h>	
-#include<unistd.h>
 
#Popis 
Server je přeložitelný pomocí Makefile, který vytvoří spustitelný soubor hinfosvc.
Tento server je spustitelný s argumentem označující lokální port na kterém bude naslouchat požadavkům:

./hinfosvc 12345
Server je možné ukončit pomocí CTRL+C.

#Použití
./hinfosvc 12345 &
curl http://localhost:12345/hostname  : Vrací síťové jméno počítače včetně domény.
curl http://localhost:12345/cpu-name  : Vrací informaci o procesoru.
curl http://localhost:12345/load      : Vrací aktuální informace o zátěži v %.

#Autor
Pavlo Kornieiev - autor projektu

