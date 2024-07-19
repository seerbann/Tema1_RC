> WeatherMonitor (C)
>
> Chiriac Serban-Mihail 2B4

**Viziune generala si obiectivele proiectului**

WheaterMonitor este o aplicatie client-server ce ofera utilizatorilor
informatii despre vremea din Romania (temperatura, precipitatii, viteza
vantului, ceata). Aceasta permite <u>clientilor fara privilegii</u>:
listarea tuturor temperaturilor, creearea unei liste de orase specifice
impreuna cu datele despre acestea iar <u>clientilor de tip
administrator</u> ce se vor conecta la un server diferit vor putea
updata si sterge informatii despre orasele dorite.

**Tehnologii utilizate**

Aplicatia foloseste o arhitectura client-server , folosind protocolul
TCP deoarece este nevoie de o conexiune intre clienti si servere iar
totodata serverele (unul folosit doar pentru vizualizarea datelor iar
altul folosit pentru modificarea lor) sunt concurente (se creaza un
proces copil prin fork() pentru fiecare client ce acceseaza unul dintre
ele) , acestea putand fi accesate cu porturi diferite de catre mai multi
clienti fie ei obisnuiti sau administratori.

**Diagrama flow-ului**

<img src="images/CAPTURE.PNG" style="width:5in;height:3.03125in" />

Clientul 3 este un client administrator , pe cand 1 si 2 sunt clienti
obisnuiti.

**Detalii de implementare**

**Protocolul TCP**

Protocolul TCP implementat este concurent, serverul creând cate un
proces copil pentru fiecare client.

<img src="/media/image2.png" style="width:3.55208in;height:5in" />

-\>**SERVER SIDE**

Serverul TCP deschide socketul, atașează un port pentru furnizarea
serviciului și așteaptă conectarea clienților. Bucla de tratare a
clienților începe cu acceptarea conexiunilor ce sunt initiate de catre
client. După acceptarea unei conexiuni și obținerea socketului asociat
respectivului client (returnat de *accept()*), serverul creeaza un
proces copil prin fork(). In acest proces copil serverul va comunica cu
baza de date. Totodata procesul parinte va incerca sa curete procesele
copil terminate printr-un waitpid neblocant.

while (waitpid(-1, NULL, WNOHANG))

Pentru alti clienti ce se vor conecta la server se va relua bucla de
acceptare. Conexiunea se va termina prin inchiderea socketului.

-\>**CLIENT SIDE**

Clientului TCP va creea si deschide socketul apoi va initia conexiunea
la server deoarece serverul asteapta pasiv conectarea. Dupa care se va
realiza transferul de date intre client si procesul copil creat de
server pentru clientul respectiv. La final conexiunea terminandu-se prin
inchiderea socketului. Toate acestea sunt implementate intr-o bucla ce
se va incheia la scrierea comenzii “quit”.

**Clienit obisnuit vs Clienti administrator**

-\>Client obisnuit

Acesta va avea acces doar la portul serverului principal , astfel acesta
nu are optiunea de modificare in orice fel a bazei de date meteo.

Comenzi:

-help – afiseaza comenzi

-info nume_oras –informatii despre orasul respectiv

-list – afiseaza lista de orase , in maniera default toatea orasele sunt
listate

-modifList – modifica lista de orase ce vor fi listate cu ajutorul
comenzii ‘list’

-defaultList – revine la lista default

-\>Client administrator

Acesta are acces la ambele porturi avand astfel in plus puterea de a
modifica date din baza de date.

Comenzi:

-updateCity arg1 arg2 arg3 ; arg1 – numele orasului, agr2-temperatura ,
arg3-precipitatii

-deleteCity arg1 ; sterge datele orasului arg1 trimitandu-le la un numar
default.

**Baza de date**

In proiect voi implementa <u>SQLite</u> deoarece:

-este usor de utilizat , nu necesita instalari complexe sau un server
separat, baza de date salvandu-se pe calculatorul ce ruleaza aplicatia,
astfel la prima pornire a serverului, aceasta va trebui configurata.

-accepta tranzactii , un element important in cazul in care 2 clienti
administratori vor modifica in acelasi timp anumite date din baza de
date.

Model:

<img src="/media/image3.png" style="width:5in;height:0.91667in" />

**Bibliografie**

<https://profs.info.uaic.ro/~computernetworks/cursullaboratorul.php>

<https://docs.google.com/presentation/d/1reUzYxEYVd1WjqvNNy-wKNurJsS57_Oxn7ciMss6KU8/edit#slide=id.g261900c4cb6_0_125>
