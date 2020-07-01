#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

int v[1010], a[1010], k, t[1010], V[1010], consultatii;
int available_resources, pat, maxtime, nrpat, doc;
pthread_mutex_t mtx;

int intra_cabinet(int x) {
    pthread_mutex_lock(&mtx);
    a[x] = ++nrpat;
    printf ("Patientul %d intra la consultatie la doctorul %d\n", nrpat, x);
    pthread_mutex_unlock(&mtx);
    return 0;
}

int iese_cabinet(int x) {
    pthread_mutex_lock(&mtx);
    printf ("Patientul %d a stat la consultatie %d la doctorul %d si a asteptat %d\n", a[x], V[ a[x] ], x, t[ a[x] ]);
    a[x] = 0;
    consultatii++;
    pthread_mutex_unlock(&mtx);
    return 0;
}

void *fct(void *x) {
    if (nrpat >= pat) {
        for (int i = 1; i <= doc; ++i) {
            if (a[i] != 0) {
                v[ a[i]  ]--;
                if (v[ a[i] ] == 0) {
                    iese_cabinet(i);
                }
            }
        }
    } else {
        for (int i = 1; i <= doc && nrpat < pat; ++i) {
            if (a[i] == 0) {
                intra_cabinet(i);
            }
        }
        int maxim = a[1];
        for (int i = 1; i <= doc; ++i) {
            v[ a[i]  ]--;
            if (a[i] > maxim) {
                maxim = a[i];
            }
            if (v[ a[i] ] == 0) {
                iese_cabinet(i);
            }
        }
        for (int i = maxim + 1; i <= pat; ++i) {
            ++t[i];
        }
    }
    return NULL;
}

int maxim() {
    int m = 0;
    for (int i = 1; i <= pat; ++i) {
        if (t[i] + V[i] > m) {
            m = t[i] + V[i];
        }
    }
    return m;
}

int main(int argc, char *argv[]) {
    doc = atoi(argv[1]);
    available_resources = doc;
    pat = atoi(argv[2]);
    maxtime = atoi(argv[3]);
    srand (time(NULL));
    for (int i = 1; i <= pat; ++i) {
        v[i] = rand() % maxtime;
        V[i] = v[i];
    }
    if ( pthread_mutex_init(&mtx, NULL)) {
          perror (NULL);
          return errno;
    }
    pthread_t tid[1000010];
    int *rez;
    int j = 0;
    while (consultatii < pat) {
        int *p = (int*)malloc(1);
        *p = j;
        if (pthread_create(&tid[j], NULL, fct, p)) {
            perror (NULL);
            return errno;

        }
        if (pthread_join(tid[j], &rez)) {
            perror (NULL);
            return errno;
        }
        j++;
    }
    printf("Cabinetul se inchide dupa %d minute\n", maxim());
//    pthread_mutex_destry(&mtx);
    return 0;
}
