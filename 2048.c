#include <ctype.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
// construiesc meniul si optiunile lui, colorez alegerea curenta
void facem_meniu(WINDOW *wnd, int optiune) 
{
    int i;
    wclear(wnd);
    mvwaddstr(wnd, 0, 0, "Meniu:");
    char m[3][25] = {"New game", "Resume", "Quit"};
    for (i = 0; i < 3; i++) 
    {
        if (i == optiune) 
        {
            wattron(wnd, A_REVERSE);
            mvprintw(i + 2, 2, "%s", m[i]);
            wattroff(wnd, A_REVERSE);
        } 
        else 
        {
            mvwprintw(wnd, i + 2, 2, "%s", m[i]);
        }
    }
    wrefresh(wnd);
}
//aflam data si ora curente
void data_ora(WINDOW *wnd) 
{
    time_t t;
    struct tm *data;
    time(&t);
    data = localtime(&t);
    mvwprintw(wnd, 2, 56, "%02d-%02d-%4d %02d:%02d:%02d", data->tm_mday,
              data->tm_mon + 1, data->tm_year + 1900, data->tm_hour,
              data->tm_min, data->tm_sec);
}
//generam un numar 2 sau 4, in mod aleatoriu
void doi_patru(int tabel[4][4]) 
{
    int i, j;
    srand(time(NULL));
    do 
    {
        i = rand() % 4;
        j = rand() % 4;
    } while (tabel[i][j] != 0);
    tabel[i][j] = (rand() % 2 + 1) * 2;
}
// verificam daca exista celule goale pe tabla de joc
int celule_goale(int tabel[4][4]) 
{
    int i, j, ok;
    ok = 1;
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            if (tabel[i][j] == 0) 
            {
                ok = 0;
                break;
            }
        }
    }
    if (ok == 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
//verificam daca putem efectua o mutare in jos
int mutare_jos(int tabel[4][4]) 
{
    int i, j;
    for (i = 0; i < 3; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            if ((tabel[i][j] == tabel[i + 1][j] && tabel[i][j] != 0) ||
                (tabel[i + 1][j] == 0 && tabel[i][j] != 0)) 
            {
                return 1;
            }
        }
    }
    return 0;
}
//verificam daca putem efectua o mutare in sus
int mutare_sus(int tabel[4][4]) 
{
    int i, j;
    for (i = 1; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            if ((tabel[i][j] == tabel[i - 1][j] && tabel[i][j] != 0) ||
                (tabel[i - 1][j] == 0 && tabel[i][j] != 0)) 
            {
                return 1;
            }
        }
    }
    return 0;
}
//verificam daca putem efectua o mutare la stanga
int mutare_stanga(int tabel[4][4]) 
{
    int i, j;
    for (i = 0; i < 4; i++) 
    {
        for (j = 1; j < 4; j++) 
        {
            if ((tabel[i][j] == tabel[i][j - 1] && tabel[i][j] != 0) ||
                (tabel[i][j - 1] == 0 && tabel[i][j] != 0)) 
            {
                return 1;
            }
        }
    }
    return 0;
}
//verificam daca putem efectua o mutare la dreapta
int mutare_dreapta(int tabel[4][4]) 
{
    int i, j;
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 3; j++) 
        {
            if ((tabel[i][j] == tabel[i][j + 1] && tabel[i][j] != 0) ||
                (tabel[i][j + 1] == 0 && tabel[i][j] != 0)) 
            {
                return 1;
            }
        }
    }
    return 0;
}
// verificam daca jocul a fost pierdut, adica daca nu mai exista mutari valide
//sau celule goale
int sfarsit(int tabel[4][4]) 
{
    if (mutare_stanga(tabel) == 0 && mutare_dreapta(tabel) == 0 &&
        mutare_sus(tabel) == 0 && mutare_jos(tabel) == 0 &&
        celule_goale(tabel) == 0) 
    {
        return 1;
    }
    return 0;
}
// verificam daca jocul a fost castigat(daca numarul 2048 se gaseste pe tabla)
int ai_castigat(int tabel[4][4]) 
{
    int i, j;
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            if (tabel[i][j] == 2048) return 1;
        }
    }
    return 0;
}
//efectuam mutarea la stanga
void stanga(int tabel[4][4], int *punctaj) 
{
    int i, j, culoare;
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 3; j++) 
        {
            if (tabel[i][j] != 0) 
            {
                int k = j + 1;
                // daca celulele sunt 0, nu efectuam nicio actiune asupta lor,
                // trecem mai departe
                while (k < 4 && tabel[i][k] == 0) 
                {
                    k++;
                }
                // daca doua celule identice se unesc, ii dublam valoarea si o
                // coloram corespunzator
                if (k < 4 && tabel[i][j] == tabel[i][k]) 
                {
                    culoare = tabel[i][j] * 2;
                    if (culoare == 2) culoare = 1;
                    if (culoare == 4) culoare = 2;
                    if (culoare == 8) culoare = 3;
                    if (culoare == 16) culoare = 4;
                    if (culoare == 32) culoare = 5;
                    if (culoare == 64) culoare = 6;
                    if (culoare == 128) culoare = 7;
                    if (culoare == 256) culoare = 8;
                    if (culoare == 512) culoare = 9;
                    if (culoare == 1024) culoare = 10;
                    if (culoare == 2048) culoare = 11;
                    attron(COLOR_PAIR(culoare));
                    tabel[i][j] = tabel[i][j] + tabel[i][k];
                    *punctaj = *punctaj + tabel[i][j];
                    tabel[i][k] = 0;
                    attroff(COLOR_PAIR(culoare));
                }
            }
        }

        // Mută celulele la stânga
        for (j = 0; j < 3; j++) 
        {
            if (tabel[i][j] == 0) 
            {
                int k = j + 1;
                while (k < 4 && tabel[i][k] == 0) 
                {
                    k++;
                }
                if (k < 4) 
                {
                    tabel[i][j] = tabel[i][k];
                    tabel[i][k] = 0;
                }
            }
        }
    }
}
//efectuam mutarea la dreapta
void dreapta(int tabel[4][4], int *punctaj) 
{
    int i, j, culoare;
    for (i = 0; i < 4; i++) 
    {
        for (j = 3; j > 0; j--) 
        {
            if (tabel[i][j] != 0) 
            {
                // verificam ca celula sa nu fie goala
                int k = j - 1;
                while (k >= 0 && tabel[i][k] == 0) 
                {
                    k--;
                }
                // verificam daca celula curenta si urmatoarea celula
                // nenula se pot uni
                if (k >= 0 && tabel[i][j] == tabel[i][k]) 
                {
                    culoare = tabel[i][j] * 2;
                    if (culoare == 2) culoare = 1;
                    if (culoare == 4) culoare = 2;
                    if (culoare == 8) culoare = 3;
                    if (culoare == 16) culoare = 4;
                    if (culoare == 32) culoare = 5;
                    if (culoare == 64) culoare = 6;
                    if (culoare == 128) culoare = 7;
                    if (culoare == 256) culoare = 8;
                    if (culoare == 512) culoare = 9;
                    if (culoare == 1024) culoare = 10;
                    if (culoare == 2048) culoare = 11;
                    attron(COLOR_PAIR(culoare));
                    tabel[i][j] = tabel[i][j] + tabel[i][k];
                    *punctaj = *punctaj + tabel[i][j];
                    tabel[i][k] = 0;
                    attroff(COLOR_PAIR(culoare));
                }
            }
        }
         // Mută celulele la dreapta
        for (j = 3; j > 0; j--) 
        {
            if (tabel[i][j] == 0) 
            {
                int k = j - 1;
                while (k >= 0 && tabel[i][k] == 0) 
                {
                    k--;
                }
                if (k >= 0) 
                {
                    tabel[i][j] = tabel[i][k];
                    tabel[i][k] = 0;
                }
            }
        }
    }
}
//efectuam mutarea in jos
void jos(int tabel[4][4], int *punctaj) 
{
    int i, j, culoare;
    for (j = 0; j < 4; j++) 
    {
        for (i = 3; i > 0; i--) 
        {
            // verificam ca celula sa nu fie goala
            if (tabel[i][j] != 0) 
            {
                // gasim urmatoarea celula care nu e goala
                int k = i - 1;
                while (k >= 0 && tabel[k][j] == 0) 
                {
                    k--;
                }
                // verificam daca celula curenta si urmatoarea celula
                // nenula se pot uni
                if (k >= 0 && tabel[i][j] == tabel[k][j]) 
                {
                    culoare = tabel[i][j] * 2;
                    if (culoare == 2) culoare = 1;
                    if (culoare == 4) culoare = 2;
                    if (culoare == 8) culoare = 3;
                    if (culoare == 16) culoare = 4;
                    if (culoare == 32) culoare = 5;
                    if (culoare == 64) culoare = 6;
                    if (culoare == 128) culoare = 7;
                    if (culoare == 256) culoare = 8;
                    if (culoare == 512) culoare = 9;
                    if (culoare == 1024) culoare = 10;
                    if (culoare == 2048) culoare = 11;
                    attron(COLOR_PAIR(culoare));
                    tabel[i][j] = tabel[i][j] + tabel[k][j];
                    *punctaj = *punctaj + tabel[i][j];
                    tabel[k][j] = 0;
                    attroff(COLOR_PAIR(culoare));
                }
            }
        }

        // Mută celulele în jos
        for (i = 3; i > 0; i--) 
        {
            if (tabel[i][j] == 0) 
            {
                int k = i - 1;
                while (k >= 0 && tabel[k][j] == 0) 
                {
                    k--;
                }
                if (k >= 0) 
                {
                    tabel[i][j] = tabel[k][j];
                    tabel[k][j] = 0;
                }
            }
        }
    }
}
//efectuam mutarea in sus
void sus(int tabel[4][4], int *punctaj) 
{
    int i, j, culoare;
    for (j = 0; j < 4; j++) 
    {
        for (i = 0; i < 3; i++) 
        {
            // verificam ca celula sa nu fie goala
            if (tabel[i][j] != 0) 
            {
                // gasim urmatoarea celula care nu e goala
                int k = i + 1;
                while (k < 4 && tabel[k][j] == 0) 
                {
                    k++;
                }
                // verificam daca celula curenta si urmatoarea celula
                // nenula se pot uni
                if (k < 4 && tabel[i][j] == tabel[k][j]) 
                {
                    culoare = tabel[i][j] * 2;
                    if (culoare == 2) culoare = 1;
                    if (culoare == 4) culoare = 2;
                    if (culoare == 8) culoare = 3;
                    if (culoare == 16) culoare = 4;
                    if (culoare == 32) culoare = 5;
                    if (culoare == 64) culoare = 6;
                    if (culoare == 128) culoare = 7;
                    if (culoare == 256) culoare = 8;
                    if (culoare == 512) culoare = 9;
                    if (culoare == 1024) culoare = 10;
                    if (culoare == 2048) culoare = 11;
                    attron(COLOR_PAIR(culoare));
                    tabel[i][j] = tabel[i][j] + tabel[k][j];
                    *punctaj = *punctaj + tabel[i][j];
                    tabel[k][j] = 0;
                    attroff(COLOR_PAIR(culoare));
                }
            }
        }

        // Mută celulele în sus
        for (i = 0; i < 3; i++) 
        {
            if (tabel[i][j] == 0) 
            {
                int k = i + 1;
                while (k < 4 && tabel[k][j] == 0) 
                {
                    k++;
                }
                if (k < 4) 
                {
                    tabel[i][j] = tabel[k][j];
                    tabel[k][j] = 0;
                }
            }
        }
    }
}
int mutare_dreapta(int tabel[4][4]);
int mutare_stanga(int tabel[4][4]);
int mutare_jos(int tabel[4][4]);
int mutare_sus(int tabel[4][4]);
// Desenam conturul tablei de joc si o coloram.
// Tot aici realizez si legenda comenzilor
void desen_contur(WINDOW *wnd, int tabel[4][4], int punctaj) 
{
    int i, j, x, y, culoare;
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            y = i * 3;
            x = j * 7;
            culoare = tabel[i][j];
            if (culoare == 2) culoare = 1;
            if (culoare == 4) culoare = 2;
            if (culoare == 8) culoare = 3;
            if (culoare == 16) culoare = 4;
            if (culoare == 32) culoare = 5;
            if (culoare == 64) culoare = 6;
            if (culoare == 128) culoare = 7;
            if (culoare == 256) culoare = 8;
            if (culoare == 512) culoare = 9;
            if (culoare == 1024) culoare = 10;
            if (culoare == 2048) culoare = 11;
            wattron(wnd, COLOR_PAIR(culoare));
            mvwprintw(wnd, y + 1, x + 1, " ----- ");
            mvwprintw(wnd, y + 2, x + 1, "|  %2d  |", tabel[i][j]);
            mvwprintw(wnd, y + 3, x + 1, " ----- ");
            wattroff(wnd, COLOR_PAIR(culoare));
        }
    }
    mvwprintw(wnd, 2, 40, "Data si ora: ");
    data_ora(wnd);
    mvwprintw(wnd, 4, 40, "Punctaj: %d", punctaj);
    mvwaddstr(wnd, 6, 40, "Taste utilizate pentru mutare:");
    mvwaddstr(wnd, 8, 42, "W - sus");
    mvwaddstr(wnd, 9, 42, "S - jos");
    mvwaddstr(wnd, 10, 42, "A - stanga");
    mvwaddstr(wnd, 11, 42, "D - dreapta");
    mvwaddstr(wnd, 12, 42, "Q - iesire");
    mvwaddstr(wnd, 14, 42, "Acum puteti muta in directiile:");
    if(mutare_stanga(tabel))
    {
        mvwaddstr(wnd, 15, 44, "- STANGA");
    }
    if(mutare_dreapta(tabel))
    {
        mvwaddstr(wnd, 16, 44, "- DREAPTA");
    }
    if(mutare_jos(tabel))
    {
        mvwaddstr(wnd, 17, 44, "- JOS");
    }
    if(mutare_sus(tabel))
    {
        mvwaddstr(wnd, 18, 44, "- SUS");
    }
    wrefresh(wnd);
}
// numaram cate casute goale se gasesc pe tabla
int casute_goale(int tabel[4][4]) 
{
    int nr, i, j;
    nr = 0;
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            if (tabel[i][j] == 0) nr++;
        }
    }
    return nr;
}
// gasim mutarea care elibereaza un numar maxim de celule
int mutare_ideala(int tabel[4][4], int *punctaj) 
{
    int a[4][4], i, j, mx;
    int v[4] = {0};
    // realizez o copie a tablei de joc, unde voi simula fiecare muatre. In
    // acest fel, voi vedea care e directa care va elibera cele mai multe
    // patratele
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            a[i][j] = tabel[i][j];
        }
    }
    if (mutare_stanga(a) == 1) 
    {
        stanga(a, punctaj);
        v[0] = casute_goale(a);
    }
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            a[i][j] = tabel[i][j];
        }
    }
    if (mutare_dreapta(a) == 1) 
    {
        dreapta(a, punctaj);
        v[1] = casute_goale(a);
    }
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            a[i][j] = tabel[i][j];
        }
    }
    if (mutare_jos(a) == 1) 
    {
        jos(a, punctaj);
        v[2] = casute_goale(a);
    }
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            a[i][j] = tabel[i][j];
        }
    }
    if (mutare_sus(a) == 1) 
    {
        sus(a, punctaj);
        v[3] = casute_goale(a);
    }
    mx = -1;
    for (i = 0; i < 4; i++) 
    {
        if (v[i] > mx) mx = v[i];
    }
    int p;
    for (i = 0; i < 4; i++) 
    {
        if (v[i] == mx && mx > 0) 
        {
            p = i;
            break;
        }
    }
    return p;
}
// efectuam mutarea automata care elibereaza un numar maxim de celule
void muta_automat(int tabel[4][4], int *punctaj) 
{
    if (mutare_ideala(tabel, punctaj) == 0) 
    {
        stanga(tabel, punctaj);
        mutare_stanga(tabel);
        doi_patru(tabel);
    } 
    else 
    if (mutare_ideala(tabel, punctaj) == 1) 
    {
        dreapta(tabel, punctaj);
        mutare_dreapta(tabel);
        doi_patru(tabel);
    } 
    else 
    if (mutare_ideala(tabel, punctaj) == 2) 
    {
        jos(tabel, punctaj);
        mutare_jos(tabel);
        doi_patru(tabel);
    } 
    else 
    if (mutare_ideala(tabel, punctaj) == 3) 
    {
        sus(tabel, punctaj);
        mutare_sus(tabel);
        doi_patru(tabel);
    }
}
// realizam un cronometru care verifica daca diferenta de timp dintre ultima
// mutare si ora curenta este mai mare de 10 secunde
int cronometru(time_t *start_time) 
{
    time_t timp_actual = time(NULL);
    return (timp_actual - *start_time >= 10);
}
// realizam comenzile ce vor fi efectuate dupa apasarea butonului de resume
void apas_resume(WINDOW *wnd, int tabel[4][4], int *punctaj, int *joc_resume) 
{
    int ch, start = 1;
    static time_t start_timp;
    werase(wnd);
    wrefresh(wnd);
    WINDOW *joc = newwin(30, 90, 0, 30);
    nodelay(joc, TRUE);
    int in_joc = 1;
    if (start == 0) 
    {
        doi_patru(tabel);
        doi_patru(tabel);
    }
    desen_contur(joc, tabel, *punctaj);
    start_timp = time(NULL);
    while (in_joc == 1) 
    {
        werase(joc);
        desen_contur(joc, tabel, *punctaj);
        wrefresh(joc);
        ch = wgetch(joc);
        // efectuez mutarile in functie de tastele pe care le apas
        switch (ch) 
        {
            case 'a':
            case 'A':
                start_timp = time(NULL);
                if (mutare_stanga(tabel) == 1) 
                {
                    stanga(tabel, punctaj);
                    doi_patru(tabel);
                }
                break;
            case 'd':
            case 'D':
                start_timp = time(NULL);
                if (mutare_dreapta(tabel) == 1) 
                {
                    dreapta(tabel, punctaj);
                    doi_patru(tabel);
                }
                break;
            case 'w':
            case 'W':
                start_timp = time(NULL);
                if (mutare_sus(tabel) == 1) 
                {
                    sus(tabel, punctaj);
                    doi_patru(tabel);
                }
                break;
            case 's':
            case 'S':
                start_timp = time(NULL);
                if (mutare_jos(tabel) == 1) 
                {
                    jos(tabel, punctaj);
                    doi_patru(tabel);
                }
                break;
            case 'q':
            case 'Q':
                start_timp = time(NULL);
                in_joc = 0;
                return;
        }
        // verific daca am pierdut
        if (sfarsit(tabel) == 1) 
        {
            werase(joc);
            wrefresh(joc);
            mvwaddstr(joc, 8, 13, "Imi pare rau, ai pierdut!");
            mvwaddstr(joc, 10, 13, "Apasa Q pentru a reveni la meniu");
            wrefresh(joc);
            sleep(1);
            ch = wgetch(joc);
            if (ch == 'q' || ch == 'Q')
            {
                in_joc = 0;
                break;
            }
        }
        // verific daca am castigat
        if (ai_castigat(tabel) == 1) 
        {
            werase(joc);
            wrefresh(joc);
            mvwaddstr(joc, 8, 15, "Bravooo!Ai castigat!");
            wrefresh(joc);
            sleep(1);
            ch = wgetch(joc);
            if (ch == 'q' || ch == 'Q') 
            {
                in_joc = 0;
                break;
            }
        }
        if (cronometru(&start_timp)) 
        {
            muta_automat(tabel, punctaj);
            start_timp = time(NULL);
        }
    }
    werase(joc);
    wrefresh(joc);
    delwin(joc);
}

// execut optiunea aleasa pe meniu
void execut_meniu(WINDOW *wnd, int *optiune, int tabel[4][4], int *punctaj,
                  int *joc_resume) 
{
    int c, start, ch, i, j;
    c = wgetch(wnd);
    static time_t start_timp;
    switch (c) 
    {
        case KEY_UP:
            *optiune = (*optiune - 1 + 3) % 3;
            break;
        case KEY_DOWN:
            *optiune = (*optiune + 1) % 3;
            break;
        // am apasat enter pe o optiune
        case 10:
            // new game
            if (*optiune == 0) 
            {
                *joc_resume = 0;
                *punctaj = 0;
                start = 0;
                werase(wnd);
                wrefresh(wnd);
                WINDOW *joc = newwin(30, 90, 0, 30);
                nodelay(joc, TRUE);
                int in_joc = 1;
                for (i = 0; i < 4; i++) 
                {
                    for (j = 0; j < 4; j++) 
                    {
                        tabel[i][j] = 0;
                    }
                }
                // daca jocul este abia inceput, pun doua numere pe tabla
                if (start == 0) 
                {
                    doi_patru(tabel);
                    doi_patru(tabel);
                }
                desen_contur(joc, tabel, *punctaj);
                start_timp = time(NULL);
                while (in_joc == 1) 
                {
                    werase(joc);
                    desen_contur(joc, tabel, *punctaj);
                    wrefresh(joc);
                    ch = wgetch(joc);
                    // efectuez mutarile in functie de tastele pe care le apas
                    switch (ch) {
                        case 'a':
                        case 'A':
                            start_timp = time(NULL);
                            if (mutare_stanga(tabel) == 1) 
                            {
                                start = 1;
                                stanga(tabel, punctaj);
                                doi_patru(tabel);
                            }
                            break;
                        case 'd':
                        case 'D':
                            start_timp = time(NULL);
                            if (mutare_dreapta(tabel) == 1) 
                            {
                                start = 1;
                                dreapta(tabel, punctaj);
                                doi_patru(tabel);
                            }
                            break;
                        case 'w':
                        case 'W':
                            start_timp = time(NULL);
                            if (mutare_sus(tabel) == 1) 
                            {
                                start = 1;
                                sus(tabel, punctaj);
                                doi_patru(tabel);
                            }
                            break;
                        case 's':
                        case 'S':
                             start_timp = time(NULL);
                            if (mutare_jos(tabel) == 1) 
                            {
                                start = 1;
                                jos(tabel, punctaj);
                                doi_patru(tabel);
                            }
                            break;
                        case 'q':
                        case 'Q':
                            start_timp = time(NULL);
                            *joc_resume = 1;
                            in_joc = 0;
                            break;
                    }
                    if (sfarsit(tabel) == 1) 
                    {
                        werase(joc);
                        wrefresh(joc);
                        mvwaddstr(joc, 8, 14, "Imi pare rau, ai pierdut!");
                        mvwaddstr(joc, 10, 14,
                                  "Apasa 'Q' pentru a reveni la meniu");
                        wrefresh(joc);
                        sleep(1);
                        ch = wgetch(joc);
                        if (ch == 'q' || ch == 'Q') 
                        {
                            in_joc = 0;
                            break;
                        }
                    }
                    if (ai_castigat(tabel) == 1) 
                    {
                        werase(joc);
                        wrefresh(joc);
                        mvwaddstr(joc, 8, 15, "Bravooo!Ai castigat!");
                        wrefresh(joc);
                        sleep(1);
                        ch = wgetch(joc);
                        if (ch == 'q' || ch == 'Q') 
                        {
                            in_joc = 0;
                            break;
                        }
                    }
                    if (cronometru(&start_timp)) 
                    {
                        muta_automat(tabel, punctaj);
                        start_timp = time(NULL);
                    }
                }
                werase(joc);
                wrefresh(joc);
                delwin(joc);
            } 
            else 
            //resume
            if (*optiune == 1) 
            {
                if (*joc_resume == 1) 
                {
                    apas_resume(wnd, tabel, punctaj, joc_resume);
                } 
                else 
                {
                    // Mesaj pentru utilizator, în cazul în care nu există un
                    // joc la care sa revenim
                    werase(wnd);
                    wrefresh(wnd);
                    mvwaddstr(wnd, 8, 14, "Nu ai un joc pe care sa il reiei!");
                    mvwaddstr(wnd, 10, 14,
                              "Apasa orice tasta pentru a reveni la meniu");
                    wrefresh(wnd);
                    wgetch(wnd);
                    werase(wnd);
                    wrefresh(wnd);
                }
            }
            else
            //quit
            if (*optiune == 2) 
            {
                endwin();
                exit(0);
            }
            break;
            *optiune = 0;
            werase(wnd);
            wrefresh(wnd);
            facem_meniu(wnd, *optiune);
            break;
    }
    if (*optiune != 0) 
    {
        facem_meniu(wnd, *optiune);
    }
}
int main(void) 
{
    int optiune, tabel[4][4], i, j, joc_resume;
    optiune = 0;
    static int punctaj = 0;
    WINDOW *principal = initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            tabel[i][j] = 0;
        }
    }
    //initializam culorile pe care le vom folosi pe tabla
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);
    init_pair(6, COLOR_BLACK, COLOR_CYAN);
    init_pair(7, COLOR_BLACK, COLOR_BLUE);
    init_pair(8, COLOR_BLACK, COLOR_BLUE);
    init_pair(9, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(10, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(11, COLOR_BLACK, COLOR_WHITE);
    // implementam desfasurarea jocului, afisam meniul si executam comenzile
    // primite
    while (1) 
    {
        facem_meniu(principal, optiune);
        execut_meniu(principal, &optiune, tabel, &punctaj, &joc_resume);
    }
    endwin();
    return 0;
}