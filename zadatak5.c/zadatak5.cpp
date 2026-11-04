#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definiramo strukturu za čvor stoga preko vezane liste
typedef struct stack* Position;
typedef struct stack {
    int broj;           // Vrijednost broja 
    Position next;      // Pokazivač na sljedeći element u stogu
} stack;

// FUNKCIJA ZA DODAVANJE NA STOG (PUSH)
int Push(Position head, int num) {
    // Alociramo memoriju za novi čvor stoga
    Position newNode = (Position)malloc(sizeof(stack));
    if (newNode == NULL) {
        printf("Greska pri alokaciji memorije!\n");
        return -1;
    }

    // Unosimo podatke u novi čvor
    newNode->broj = num;

    // Dodajemo ga na POČETAK liste (odmah iza head-a)
    newNode->next = head->next; // Novi pokazuje na ono na što je dosad pokazivao head
    head->next = newNode;       // Head sada pokazuje na naš novi čvor

    return 0;
}

// FUNKCIJA ZA SKIDANJE SA STOGA (POP)
int Pop(Position head, int* result) {
    // Provjera je li stog prazan (nema elemenata iza head-a)
    if (head->next == NULL) {
        return -1;
    }

    Position temp = head->next;   // Uzimamo prvi element s vrha
    *result = temp->broj;         // Vraćamo njegovu vrijednost kroz pokazivač 'result'

    head->next = temp->next;      // Prevezujemo head na sljedeći element

    free(temp);                   // Oslobađamo memoriju izbrisanog elementa
    return 0;
}

// FUNKCIJA ZA OSLOBAĐANJE CIJELE LISTE (Krivi unos / kraj)
void FreeStack(Position head) {
    Position current = head->next;

    // Prolazimo kroz cijelu listu i brišemo čvor po čvor da ne ostane 
    while (current != NULL) {
        Position nextNode = current->next; // Zapamtimo sljedeći
        free(current);                     // Oslobodimo trenutni
        current = nextNode;                // Pređemo na sljedeći
    }
    head->next = NULL;
}

// GLAVNA FUNKCIJA ZA ČITANJE IZ DATOTEKE I RAČUNANJE
int calculatePostfix(const char* filename, int* finalResult) {
    // Otvaramo datoteku za čitanje
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Greska: Nije moguce otvoriti datoteku '%s'!\n", filename);
        return -1;
    }

    // Inicijalizacija glave stoga (head)
    stack head_node = { .broj = 0, .next = NULL };
    Position head = &head_node;

    char buffer[64]; // Pomoćni niz (buffer) u koji spremamo svaku proitanu riječ iz datoteke

    // Čitamo element po element iz datoteke dok ih ima
    while (fscanf(fp, "%s", buffer) == 1) {
        int num;

        // Ako je pročitani element je broj
        if (sscanf(buffer, "%d", &num) == 1) {
            Push(head, num); // Stavljamo broj na stog
        }
        else {
            // Ako je pročitani element je operator
            // Provjeravamo je li duljina 1 i je li točno jedan od dozvoljenih operatora
            if (strlen(buffer) == 1 && (buffer[0] == '+' || buffer[0] == '-' || buffer[0] == '*' || buffer[0] == '/')) {
                int op1, op2;

                // Skida se dvoje s vrha stoga. 
                if (Pop(head, &op2) != 0 || Pop(head, &op1) != 0) {
                    printf("Greska: Nedovoljno operanada u izrazu!\n");
                    FreeStack(head); // Oslobađamo memoriju prije izlaza
                    fclose(fp);       // Zatvaramo datoteku
                    return -1;
                }

                int rez = 0;
                // Izvršavamo odgovarajuću matematičku operaciju
                switch (buffer[0]) {
                case '+': rez = op1 + op2; break;
                case '-': rez = op1 - op2; break;
                case '*': rez = op1 * op2; break;
                case '/':
                    if (op2 == 0) {
                        printf("Greska: Dijeljenje s nulom!\n");
                        FreeStack(head);
                        fclose(fp);
                        return -1;
                    }
                    rez = op1 / op2;
                    break;
                }

                // Dobiveni međurezultat vraćamo natrag na stog
                Push(head, rez);
            }
            else {
                // KRIVI UNOS 
                printf("Greska: Krivi unos '%s' u datoteci!\n", buffer);
                FreeStack(head); // Oslobađamo cijelu listu 
                fclose(fp);       // Zatvaramo datoteku
                return -1;        // Prekidamo zadatak
            }
        }
    }

    // Kada petlja završi, zatvaramo datoteku
    fclose(fp);

    // Na kraju u stogu mora ostati točno jedan element - konačni rezultat
    int result;
    if (Pop(head, &result) != 0 || head->next != NULL) {
        printf("Greska (ostalo previse brojeva ili je prazno)!\n");
        FreeStack(head);
        return -1;
    }

    *finalResult = result; // Vraćamo rezultat u main
    return 0;
}

int main() {
    char filename[50];
    printf("Unesi naziv datoteke: ");
    if (scanf("%s", filename) != 1) {
        return 1;
    }

    int rezultat = 0;
    if (calculatePostfix(filename, &rezultat) == 0) {
        printf("Konacni rezultat postfiks izraza je: %d\n", rezultat);
    }
    else {
        printf("Racunanje nije uspjelo zbog pogreske.\n");
    }

    return 0;
}