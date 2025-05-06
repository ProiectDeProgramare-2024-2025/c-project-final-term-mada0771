#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

// For Windows colors
#ifdef _WIN32
#include <windows.h>
#define COLOR_RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY)
#define COLOR_GREEN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COLOR_BLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define COLOR_YELLOW SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COLOR_CYAN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COLOR_RESET SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#else
// ANSI color codes for Linux/Mac
#define COLOR_RED printf("\033[1;31m")
#define COLOR_GREEN printf("\033[1;32m")
#define COLOR_BLUE printf("\033[1;34m")
#define COLOR_YELLOW printf("\033[1;33m")
#define COLOR_CYAN printf("\033[1;36m")
#define COLOR_RESET printf("\033[0m")
#endif

#define FISIER "filename.txt"
#define MAX_PRODUSE 100
#define MAX_NUME 50

typedef struct {
    char nume[MAX_NUME];
    float pret;
    int disponibilitate;
    int rezervat;
    char data_adaugare[11]; // zz/ll/aaaa format
} Produs;

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

bool isValidDate(const char *date) {
    if (strlen(date) != 10) return false;
    if (date[2] != '/' || date[5] != '/') return false;

    int day, month, year;
    if (sscanf(date, "%d/%d/%d", &day, &month, &year) != 3) return false;

    if (day < 1 || day > 31) return false;
    if (month < 1 || month > 12) return false;
    if (year < 1900 || year > 2100) return false;

    return true;
}

void getCurrentDate(char *dateStr) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dateStr, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

int citireProduseDinFisier(Produs produse[]) {
    FILE *f = fopen(FISIER, "r");
    if (!f) {
        COLOR_RED;
        printf("Eroare la deschiderea fisierului %s.\n", FISIER);
        COLOR_RESET;
        return 0;
    }

    int i = 0;
    while (i < MAX_PRODUSE &&
           fscanf(f, "%49s %f %d %d %10s",
                  produse[i].nume,
                  &produse[i].pret,
                  &produse[i].disponibilitate,
                  &produse[i].rezervat,
                  produse[i].data_adaugare) == 5) {
        i++;
    }

    fclose(f);
    return i;
}

void salvareProduseInFisier(Produs produse[], int numarProduse) {
    FILE *f = fopen(FISIER, "w");
    if (!f) {
        COLOR_RED;
        printf("Eroare la deschiderea fisierului %s.\n", FISIER);
        COLOR_RESET;
        return;
    }

    for (int i = 0; i < numarProduse; i++) {
        fprintf(f, "%s %.2f %d %d %s\n",
                produse[i].nume,
                produse[i].pret,
                produse[i].disponibilitate,
                produse[i].rezervat,
                produse[i].data_adaugare);
    }

    fclose(f);
}

void afisareProdus(Produs p) {
    COLOR_CYAN;
    printf("Nume: ");
    COLOR_YELLOW;
    printf("%s", p.nume);
    COLOR_CYAN;
    printf(", Pret: ");
    COLOR_GREEN;
    printf("%.2f", p.pret);
    COLOR_CYAN;
    printf(", Disponibilitate: ");
    COLOR_BLUE;
    printf("%d", p.disponibilitate);
    COLOR_CYAN;
    printf(", Rezervat: ");
    if (p.rezervat) {
        COLOR_RED;
        printf("Da");
    } else {
        COLOR_GREEN;
        printf("Nu");
    }
    COLOR_CYAN;
    printf(", Data adaugare: ");
    COLOR_YELLOW;
    printf("%s", p.data_adaugare);
    COLOR_RESET;
    printf("\n");
}

void vizualizareProduseDisponibile(Produs produse[], int numarProduse) {
    printf("\n");
    COLOR_GREEN;
    printf("=== Produse disponibile ===\n");
    COLOR_RESET;

    bool found = false;
    for (int i = 0; i < numarProduse; i++) {
        if (produse[i].disponibilitate > 0 && !produse[i].rezervat) {
            afisareProdus(produse[i]);
            found = true;
        }
    }

    if (!found) {
        COLOR_YELLOW;
        printf("Nu exista produse disponibile momentan.\n");
        COLOR_RESET;
    }

    printf("\nApasati Enter pentru a reveni la meniu...");
    clearInputBuffer();
    getchar();
}

void rezervaProdus(Produs produse[], int numarProduse) {
    char numeProdus[MAX_NUME];
    printf("\n");
    COLOR_BLUE;
    printf("Introduceti numele produsului pe care doriti sa-l rezervati: ");
    COLOR_RESET;
    scanf("%49s", numeProdus);

    for (int i = 0; i < numarProduse; i++) {
        if (strcmp(produse[i].nume, numeProdus) == 0) {
            if (produse[i].disponibilitate > 0 && !produse[i].rezervat) {
                produse[i].rezervat = 1;
                salvareProduseInFisier(produse, numarProduse);
                COLOR_GREEN;
                printf("Produsul ");
                COLOR_YELLOW;
                printf("%s", produse[i].nume);
                COLOR_GREEN;
                printf(" a fost rezervat cu succes!\n");
                COLOR_RESET;
            } else {
                COLOR_RED;
                printf("Produsul nu este disponibil sau a fost deja rezervat.\n");
                COLOR_RESET;
            }
            printf("\nApasati Enter pentru a reveni la meniu...");
            clearInputBuffer();
            getchar();
            return;
        }
    }

    COLOR_RED;
    printf("Produsul nu a fost gasit!\n");
    COLOR_RESET;
    printf("\nApasati Enter pentru a reveni la meniu...");
    clearInputBuffer();
    getchar();
}

void cautaProdus(Produs produse[], int numarProduse) {
    char numeProdus[MAX_NUME];
    printf("\n");
    COLOR_BLUE;
    printf("Introduceti numele produsului pe care doriti sa-l cautati: ");
    COLOR_RESET;
    scanf("%49s", numeProdus);

    for (int i = 0; i < numarProduse; i++) {
        if (strcmp(produse[i].nume, numeProdus) == 0) {
            printf("\n");
            COLOR_GREEN;
            printf("=== Rezultate cautare ===\n");
            COLOR_RESET;
            afisareProdus(produse[i]);
            printf("\nApasati Enter pentru a reveni la meniu...");
            clearInputBuffer();
            getchar();
            return;
        }
    }

    COLOR_RED;
    printf("Produsul nu a fost gasit!\n");
    COLOR_RESET;
    printf("\nApasati Enter pentru a reveni la meniu...");
    clearInputBuffer();
    getchar();
}

bool validareNumeProdus(const char *nume) {
    if (strlen(nume) == 0 || strlen(nume) >= MAX_NUME) return false;
    for (int i = 0; nume[i]; i++) {
        if (!isalnum(nume[i]) && nume[i] != '-' && nume[i] != '_') {
            return false;
        }
    }
    return true;
}

void adaugaProdus(Produs produse[], int *numarProduse) {
    if (*numarProduse >= MAX_PRODUSE) {
        COLOR_RED;
        printf("Numarul maxim de produse a fost atins!\n");
        COLOR_RESET;
        printf("\nApasati Enter pentru a reveni la meniu...");
        clearInputBuffer();
        getchar();
        return;
    }

    Produs produsNou;

    // Validare nume produs
    do {
        printf("\n");
        COLOR_BLUE;
        printf("Introduceti numele produsului (litere, cifre, '-', '_'): ");
        COLOR_RESET;
        scanf("%49s", produsNou.nume);
        clearInputBuffer();

        if (!validareNumeProdus(produsNou.nume)) {
            COLOR_RED;
            printf("Nume invalid! Folositi doar litere, cifre, '-' sau '_' (maxim %d caractere).\n", MAX_NUME-1);
            COLOR_RESET;
        }
    } while (!validareNumeProdus(produsNou.nume));

    // Validare pret
    do {
        COLOR_BLUE;
        printf("Introduceti pretul produsului (tip float, Ex: 14.99): ");
        COLOR_RESET;
        while (scanf("%f", &produsNou.pret) != 1 || produsNou.pret <= 0) {
            COLOR_RED;
            printf("Pret invalid! Introduceti o valoare numerica pozitiva: ");
            COLOR_RESET;
            clearInputBuffer();
        }
        clearInputBuffer();
    } while (produsNou.pret <= 0);

    // Validare disponibilitate
    do {
        COLOR_BLUE;
        printf("Introduceti disponibilitatea produsului (numar intreg pozitiv): ");
        COLOR_RESET;
        while (scanf("%d", &produsNou.disponibilitate) != 1 || produsNou.disponibilitate < 0) {
            COLOR_RED;
            printf("Valoare invalida! Introduceti un numar intreg pozitiv: ");
            COLOR_RESET;
            clearInputBuffer();
        }
        clearInputBuffer();
    } while (produsNou.disponibilitate < 0);

    produsNou.rezervat = 0;
    getCurrentDate(produsNou.data_adaugare);

    produse[*numarProduse] = produsNou;
    (*numarProduse)++;

    salvareProduseInFisier(produse, *numarProduse);

    COLOR_GREEN;
    printf("\nProdusul ");
    COLOR_YELLOW;
    printf("%s", produsNou.nume);
    COLOR_GREEN;
    printf(" a fost adaugat cu succes!\n");
    COLOR_RESET;
    printf("\nApasati Enter pentru a reveni la meniu...");
    getchar();
}

void stergeProdus(Produs produse[], int *numarProduse) {
    char numeProdus[MAX_NUME];
    printf("\n");
    COLOR_BLUE;
    printf("Introduceti numele produsului pe care doriti sa-l stergeti: ");
    COLOR_RESET;
    scanf("%49s", numeProdus);

    for (int i = 0; i < *numarProduse; i++) {
        if (strcmp(produse[i].nume, numeProdus) == 0) {
            for (int j = i; j < *numarProduse - 1; j++) {
                produse[j] = produse[j + 1];
            }
            (*numarProduse)--;

            salvareProduseInFisier(produse, *numarProduse);

            COLOR_GREEN;
            printf("Produsul ");
            COLOR_YELLOW;
            printf("%s", numeProdus);
            COLOR_GREEN;
            printf(" a fost sters cu succes!\n");
            COLOR_RESET;
            printf("\nApasati Enter pentru a reveni la meniu...");
            clearInputBuffer();
            getchar();
            return;
        }
    }

    COLOR_RED;
    printf("Produsul nu a fost gasit!\n");
    COLOR_RESET;
    printf("\nApasati Enter pentru a reveni la meniu...");
    clearInputBuffer();
    getchar();
}

void vizualizareProduseRezervate(Produs produse[], int numarProduse) {
    printf("\n");
    COLOR_GREEN;
    printf("=== Produse rezervate ===\n");
    COLOR_RESET;

    bool found = false;
    for (int i = 0; i < numarProduse; i++) {
        if (produse[i].rezervat) {
            afisareProdus(produse[i]);
            found = true;
        }
    }

    if (!found) {
        COLOR_YELLOW;
        printf("Nu exista produse rezervate momentan.\n");
        COLOR_RESET;
    }

    printf("\nApasati Enter pentru a reveni la meniu...");
    clearInputBuffer();
    getchar();
}

void afisareMeniu() {
    system("cls || clear");

    COLOR_CYAN;
    printf("\n=== MENIU PRINCIPAL ===\n");
    COLOR_RESET;

    printf("1. ");
    COLOR_YELLOW;
    printf("Vizualizeaza produsele disponibile\n");
    COLOR_RESET;

    printf("2. ");
    COLOR_YELLOW;
    printf("Rezerva un produs\n");
    COLOR_RESET;

    printf("3. ");
    COLOR_YELLOW;
    printf("Cautare produs\n");
    COLOR_RESET;

    printf("4. ");
    COLOR_YELLOW;
    printf("Adauga produs\n");
    COLOR_RESET;

    printf("5. ");
    COLOR_YELLOW;
    printf("Sterge produs\n");
    COLOR_RESET;

    printf("6. ");
    COLOR_YELLOW;
    printf("Vizualizeaza produsele rezervate\n");
    COLOR_RESET;

    printf("0. ");
    COLOR_RED;
    printf("Iesi din meniu\n");
    COLOR_RESET;

    printf("\nAlegeti o optiune: ");
}

int main() {
    Produs produse[MAX_PRODUSE];
    int numarProduse = citireProduseDinFisier(produse);
    int optiune;

    do {
        afisareMeniu();

        while (scanf("%d", &optiune) != 1 || optiune < 0 || optiune > 6) {
            COLOR_RED;
            printf("Optiune invalida! Alegeti un numar intre 0 si 6: ");
            COLOR_RESET;
            clearInputBuffer();
        }

        switch (optiune) {
            case 1:
                vizualizareProduseDisponibile(produse, numarProduse);
                break;
            case 2:
                rezervaProdus(produse, numarProduse);
                break;
            case 3:
                cautaProdus(produse, numarProduse);
                break;
            case 4:
                adaugaProdus(produse, &numarProduse);
                break;
            case 5:
                stergeProdus(produse, &numarProduse);
                break;
            case 6:
                vizualizareProduseRezervate(produse, numarProduse);
                break;
            case 0:
                salvareProduseInFisier(produse, numarProduse);
                printf("\n");
                COLOR_GREEN;
                printf("La revedere!\n");
                COLOR_RESET;
                break;
            default:
                COLOR_RED;
                printf("\nOptiune invalida!\n");
                COLOR_RESET;
                printf("\nApasati Enter pentru a reveni la meniu...");
                clearInputBuffer();
                getchar();
        }

    } while (optiune != 0);

    return 0;
}
