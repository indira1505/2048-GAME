In cadrul acestei teme am implemetat jocul 2048. Functia "facem_meniu" 
realizeaza afisajul meniului, cu cele 3 optiuni ale sale. Am folosit 
functii din biblioteca ncurses pentru afisarea si manipularea ferestrei. 
    Functia "desen_contur", realizeaza afisajul tablei de joc. Tot aici 
afisez si legenda comenzilor valide.
    Functia "data_ora" afiseaza ora curenta.
    Functia "doi_patru" genereaza aleator un numar egal cu 2 sau 4.
    Functia "celule_goale" verifica daca exista vreo celula goala pe tabla.
    Functiile "mutare_jos", "mutare_sus","mutare_stanga" si "mutare_dreapta"
verifica daca se pot efectua mutari valide in directiile respective, in 
timp ce functiile "sus","jos","stanga","dreapta" realizeaza mutarile in
sine, miscand toate piesele de pe tabla in sensul indicat.
    Functia "cronometru" verifica daca distanta de timp dintre doua mutari
este mai mare de 10 secunde. Folosesc aceasta functie pentru
implementarea mutarilor automate din functia "muta_automat".Acest cronometru
va fi reactualizat dupa orice apasare a unei taste.
    Toate aceste functii au fost utilizare in functia "execut meniu".
Aceasta functie este cea mai importanta din intreg codul. In ea
am implementat desfasurarea efectiva a jocului. In functie de intrarile 
de la tastatura eu voi putea naviga prin meniu si, la apasarea tastei
enter, voi putea selecta una din cele 3 functii ale meniului (fiecare 
optiune din meniu a fost codificata cu unul din numerele 0,1 sau 2, pentru
a identifica mai usor optiunea selectata). 
        -daca selectez optiunea 0 (NEW_GAME):
                Cu ajutorul variabilei joc_resume vad daca am un joc pe
            care sa il pot relua sau nu(de fiecare data cand apas new game
            si efectuez modificari in tabla de joc, optiunea resume devine
            activa, deci variabila joc_resume primeste valoarea 1)
                Cu ajutorul variabilei "start" ma asigur ca generez doua
            valori de 2 sau 4 doar atunci cand incep un joc nou
                Cu ajutorul variabilei "in_joc", retin daca ma aflu sau nu
            intr-un joc in desfasurare
                Cu ajutorul tastelor W,A,S,D, voi efectua mutarile pe tabla
        -daca selectez optiunea 1 (RESUME):
                Daca variabila joc_resume e 1(am un joc activ, pe care il
            pot relua), apelez functia "apas_resume", care imi reia jocul de
            unde am ramas. Aceasta functie utilizeaza aproape in mod identic 
            comenzile pe care le folosesc pentru optiunea 0(new game).
                Daca variabila joc_resume e 0, inseamna ca nu am un joc pe
            care sa il pot relua. Voi afisa pe ecran un mesaj care sa ii
            indice jucatorului acest lucru si ma voi intoarce la meniu.
        -daca selectez optiunea 2(quit):
                Jocul se inchide
    In functia main afisez meniul si execut comenzile primite de la tastatura
intr-o bucla infinita, din care ies doar dupa ce selectez optiunea 2(QUIT).

    