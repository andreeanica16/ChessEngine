# Chess Engine

Implementarea engine-ul de sah se bazeaza pe urmatoarea codificare:
- Tabla de sah va fi reprezentata in memorie ca o matrice de 9 x 9, incepand 
numaratoarea liniilor si coloanelor de la 1
- Casutele goale vor avea valoarea 0 in ele
- Piesele albe vor fi reprezentate ca si numere pozitive
- Piesele negre vor fi reprezentate ca si numere negative
- Fiecare piesa va avea asociat un numar, prin care se va identifica unic
piesa respectiva. Numarul este format astfel: 
	```
	numarUnic = (-1)^culoare * (tipPiesa * 10 + nrOrdine)
	```
Astfel, asocierea pentru tipul pieselor este urmatoarea:
```		
		1 ... Rege/Regina
		2 ... Nebun
		3 ... Cal
		4 ... Tura
		5 ... Pion 
```
Tabla la momentul initial va arata in felul urmator:
```
		-41 -31 -21 -11 -10 -22 -32 -42 
		-51 -52 -53 -54 -55 -56 -57 -58 
		  0   0   0   0   0   0   0   0 
		  0   0   0   0   0   0   0   0 	
		  0   0   0   0   0   0   0   0 
		  0   0   0   0   0   0   0   0 
		 51  52  53  54  55  56  57  58 
		 41  31  21  11  10  22  32  42
```

Dupa citirea comenzilor "xboard" si "protover", engine-ul intra intr-o bucla
infinta, asteptand comenzi de la xboard. Pentru a implementa functionalitatile
necesare, vom folosi urmatoarele functii si variabile :
- colorToMove = specifica cine trebuie sa efectueze urmatoarea mutare
- engineColor = specifica culoarea cu care joaca engine-ul
- force       = specifica daca ne gasim in modul force
- lastMove    = retine ultima mutare efectuata
- positions   = un map care contine toate piesele existente pe tabla la un
		moment dat, cheia fiind numarul piesei, iar valoarea fiind
		pozitia ei curenta pe tabla. Pozitia unei piese va fi 
		codificata linie*10 + coloana

- translatePosition() = actualizeaza o pozitie de pe tabla in map-ul position 
- initializeBoard()   = initializeaza tabla de sah, aseaza piesele la locul lor
			si initializeaza map-ul position
- readFromInput()     = citeste o comanda de la Xboard
- makeMove()          = traduce o mutare intr-un sir de caractere, pentru a fi 
			trimis catre Xboard. Totodata actualizeaza si matricea
			si mapul cu pozitii pentru piesa mutata. In cazul in 
			care o piesa este capturata, functia o sterge din map
- markMove()	      = traduce un sir de caractere primit de la Xboard repre
			zentand o mutare si actualizeaza matricea si mapul
- checkCell()          = verifica daca o pozitie data prin linie si coloana 
			apartine limitelor tablei de joc
- checkPiecePosition() = verifica daca o piesa a fost capturata sau nu
- movePawn()	      = muta un pion dat ca parametru, gasind prima modalitate
			posibila
- applyStrategy()      = aplica Strategia. Daca nu exista nici o mutare posibila,
			da resign pentru culoarea engine-ului


Am ales sa ne bazam intreaga implementare pe o matrice
numita attacked[color]. Aceasta contine campurile "atacate" de o anumita, 
culoare adica aria de acoperire a unei culori pe tabla. Spre exemplu, 
o tura ataca intreaga linie si intreaga coloana pe care se afla, pana
cand intalneste o piesa care sa o blocheze. Aceasta matrice va contine
si piesele care ataca un anumit camp, pentru a putea mereu identifica cu
usurinta vulnerabilitatea unui camp. 

Aceasta matrice ne poate ajuta destul de mult in calcului functiei de evaluare.
Pentru a putea fi sah, regele trebuie sa aiba toate campurile din prejurul
lui atacate(adica sa nu mai poata muta) si sa fie atacat la randul lui. 
Calculculul acestei matrici ne va spune in orice moment cat de aproape suntem
de sah, pentru o functie de evaluare care sa valorifice aceste lucruri.

## Implementare reguli sah:
#### 1. Mutari
Implementarea regulilor de sah sunt realizate prin implementarea functiilor 
care ne genereaza toate mutarile ( atat cele obisnuite, cate si cele speciale
precum enpassant, pawnPromotion si castling) si verificarea daca o culoare
se afla in mat. Pentru rege si cal tinem in memorie niste vectori globali
cu offset-urile fata de pozitia curenta, pe cand pentru pion, regina, tura
si nebun generam de fiecare data offset-urile fata de pozitia curenta in
functiile insertRookMoves si insertBishopMoves.
Functia getPieceMoves genereaza pozitiile exacte pe care o piesa se poate
muta. Acelasi lucru face si getPieceMovesPawn, dar pentru cazul particularizat
de pion (acesta poate muta in fata doar daca este liber, si in fata-diagonala
doar daca pozitia este ocupata de o piesa adversa). De asemenea, acesta nu
ataca pozitia din fata lui, dar le ataca pe cele din fata-diagonala.
Functia markAttacked genereaza mutarile posibile pentru fiecare piesa de pe
tabla de sah, apoi le insereaza in tabla de atacate pentru ambele culori.
De asemenea, in antetul functiei este trimis si un hash de mutari, in care
vor fi inserate mutarile posibile pentru toate piesele de culoarea care
este la mutare. Mutarile regelui sunt mai complicate si se fac la sfarsit,
deoarece acesta poate muta doar daca nu intra in sah efectuand acea mutare.
In functia applyMoveMinimax este aplicata mutarea (se sterge piesa de pe
tabla de la pozitia curenta, se updateaza map-ul cu positions, daca se
captureaza ceva se intoarce piesa capturata si se sterge din map).
In functia undoMoveMinimax este facut procedeul invers (vom avea nevoie
in minimax sa "desfacem" miscarea anterioara).

#### 2. En passant (functia applyMoveMinimax)
Pentru en passant, tinem minte in variabila globala panw2moves daca ultima
mutare a fost efectuata de un pion (advers) care a mutat doua casute in fata.
Daca aceasta variabila globala nu este empty, si mutarea curenta este facuta
de un pion inainteaza de pe randul pawn2moves la randul urmator si coloana
pawn2moves, atunci are loc miscarea de en passant, iar captured devine
pawn2moves.

#### 3. Rocada
Functia markAttacked apeleaza, la sfarsit, canDoCastling care insereaza in
hash-ul de miscari 0, 1 sau 2 rocade, daca acestea sunt permise.
Variabilele kingMoved, rookLeftMoved, rookRightMoved retin numarul de miscari
efectuate pana acum de regele si turele care se afla in discutie.
Daca acestea nu au mai fost mutate, atunci se apeleaza pe rand functia
checkLeftRightCastling pentru dreapta, apoi pentru stanga. Aici se verifica
faptul ca intre rege si tura nu este nicio piesa (adica regele este atacat de
tura), si ca niciun camp dintre rege si tura nu este atacat de vreo piesa a
adversarului.

#### 4. Sah
Functia isCheck verifica daca regele se afla in sah, adica daca pozitia pe
care se afla este atacata de vreo piesa a oponentului.

#### 5. Sah mat
Functia isMat returneaza true daca este sah mat pentru color.
Se apeleaza intai functia kingMobility, care insereaza in hash-ul de miscari
daca regele poate sa se miste pentru a captura piesa care ii da sah, sau
pentru a fugi de sah.
Daca regele nu poate face nicio miscare si este atacat de cel putin 2 piese
adverse, atunci este mat.
Daca poate face totusi miscari si este atacat de cel putin 2 piese, nu e mat.
Daca pozitia pe care este atacatorul este atacata de piese de culoarea mea,
atunci inserez mutarile corespunzatoare in hash-ul de mutari care ma scot din
sah.
Se analizeaza apoi cazurile in care se poate bloca atacatorul (doar daca e
tura, regina sau nebun). Se verifica daca vreunul din campurile dintre
atacator si rege poate fi blocat de vreo piesa de culoarea atacata. Daca da,
atunci se insereaza in hash-ul de mutari.

## Implementare algoritm Alpha-Beta Pruning:

Pentru functia de evaluare, am folosit o varianta care imbina numarul de piese
de un anumit fel de pe tabla (fiecare piesa are o valoare asociata), dar si
pozitia pe care se afla piesele de tipul respectiv pe tabla (matricile cu
valori sunt declarate in eval.h).
Pentru matrici si valorile pieselor ne-am inspirat de pe
https://www.chessprogramming.org/Simplified_Evaluation_Function
Am imbunatatit aceasta functie de evaluare adaugand scoruri suplimentare pentru
cazuri particulare (spre exemplu, incurajam engine-ul sa dea mereu sah).

## Modaliate de compilare:
```
cmake .
make
xboard -debug -fcp "./ladybugs"
```

sau
```
./run.sh
```
Versiune de compilator pe care am testat: clang-1100.0.33.17
Am folosit varianta c++ 17.

### Specificatii tehnice:
standard c++ folosit: c++17
