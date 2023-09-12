#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

struct position{
    unsigned int ligne, colonne;
};

int maxNbreJetonsIdentiques(struct position depart, char **tabJetons, char jeton);
void annonceVictoire(char jeton);
void placeJeton(char **tabJetons, int numeroColonne, int numeroJoueur);
int sauvegarde(char **tabJetons, int numeroJoueur);
int restaurationSauvegarde(char **tabJetons);
void videTampon();
char **afficheFichiers(char *directoryPath);
int routineDeJeu(char **tabJetons, short int numeroJoueur);

char **allocationTableau(){
    char **tabJetons;
    tabJetons = malloc(sizeof(char *) * 6);
        if(tabJetons == NULL)
            return NULL;
    for(int i=0; i<6; i++){
        tabJetons[i] = malloc(sizeof(char) * 7);
        if(tabJetons[i] == NULL){
            for(int j=i-1; j>=0; j--)
                free(tabJetons[j]);
            free(tabJetons);
            break;
            return NULL;
        }
    }

    return tabJetons;
}



void initialiseTableau(char **tabJetons){
    for(int i=0; i<6; i++)
        for(int j=0; j<7; j++)
            tabJetons[i][j] = ' ';

} 



void dessineTableau(char **tabJetons){
    printf("  1   2   3   4   5   6   7  \n");
    for(int i=0; i<6; i++)
        printf("+---+---+---+---+---+---+---+\n\
| %c | %c | %c | %c | %c | %c | %c |\n", tabJetons[i][0], tabJetons[i][1], tabJetons[i][2], tabJetons[i][3], tabJetons[i][4], tabJetons[i][5], tabJetons[i][6] );
    printf("+---+---+---+---+---+---+---+\n");
    printf("  1   2   3   4   5   6   7  \n\n");
}



void quitter(char **tabJetons){
    // Si l'utilisateur entre 'q' ou 'Q', alors on quitte puissance4 (true)
    char ch = getchar();
    if(ch == 'q' || ch == 'Q')
        return exit(0);
    if(ungetc(ch, stdin) == EOF)
        printf("Attention, caractère non remis dans le buffer");

    // Si le tableau est plein, on quitte puissance4
    
    bool tabPlein(){ // Renvoie true si le tableau est plein
        for(int i=0; i<7; i++)
            if(tabJetons[0][i] == ' ')
                return false;
        return true;
    }
    
    if(tabPlein()){
        printf("Le tableau est plein, personne n'a gagné !\n");
        exit(0);
    }    
        
}
char *donneJourMoisAnneeAJD(){
    static char date[12];
    time_t t;
    time(&t);

    struct tm *local = localtime(&t);
 
    int hours = local->tm_hour;         // obtenir les heures depuis minuit (0-23)
    int minutes = local->tm_min;        // récupère les minutes passées après l'heure (0-59)
    int seconds = local->tm_sec;        // obtient les secondes passées après une minute (0-59)
 
    int day = local->tm_mday;            // récupère le jour du mois (1 à 31)
    int month = local->tm_mon + 1;      // obtient le mois de l'année (0 à 11)
    int year = local->tm_year + 1900;

    sprintf(date,"%02d-%02d-%d", day, month, year);
    return date;
}

bool dateValide(int day, int month, int year){
    if(day < 0 || day > 31)
        return false;
    else if(month < 0, month > 12)
        return false;
    else if(year < 0 || year > 9999)
        return false;

    return true;
}

char* donneJourMoisAnnee(){
    // Mets la date entrée par l'utilisateur dans la chaine de charactères 'date
    
    static char date[12];
    int day, month, year, err;
    boucle:
    printf("Entrez une date sous le format 'DD/MM/YYYY' (ou ajd pour avoir la date d'aujourd'hui)\n");
    err = scanf("%d/%d/%d", &day, &month, &year);
    if(err == 3){
        if(!dateValide(day, month, year)){
        fprintf(stderr, "format de date non-valide !\n");
        videTampon();
        goto boucle;
        }
        else{
            struct tm time;
            time.tm_year = year;
            time.tm_mon = month;
            time.tm_mday = day;
            sprintf(date, "%02d-%02d-%d", time.tm_mday, time.tm_mon, time.tm_year);
            return date;
        }
    }
    else{
        scanf("%s", date);
        if(strstr("ajd", date) != NULL){
            printf("date = ajd\n");
            return donneJourMoisAnneeAJD();
        }
        else{
            fprintf(stderr, "format de date non-valide !\n");
            videTampon();
            goto boucle;
        }
    }
}



int sauvegarder(char **tabJetons, int numeroJoueur){
    char ch = getchar();
    int err;
    if(ch == 's' || ch == 'S'){
        printf("souhaitez vous:\n 1. Sauvegarder la partie\n 2. Charger une partie\n 3. Quitter le menu\n");
        printf("entrez un chiffre pour choisir l'option: ");
        int choix;
        videTampon();
        boucle:
        err = scanf("%d", &choix);
        while(choix < 1 || choix > 3 || err != 1){
            fprintf(stderr, "entrez un chiffre compris entre 1 et 3\n");
            videTampon();
            goto boucle;
        }
        if(choix == 1)
            sauvegarde(tabJetons, numeroJoueur);
        else if(choix == 2){
            restaurationSauvegarde(tabJetons);
            return 1;
        }
        else if(choix == 3){
            videTampon();
            return 1;
        }
        else
            fprintf(stderr, "sauvegarde: je suis perdu\n");
    videTampon();
    return 1;
    }
    else 
        if(ungetc(ch, stdin) == EOF)
            printf("Attention, caractère non remis dans le buffer");
    return 0;
}

int sauvegarde(char **tabJetons, int numeroJoueur){
    printf("sauvegarde\n");
    char bufferWriter[7];
    char nomDuFichier[25];
    char *jourMoisAnnee = donneJourMoisAnneeAJD();
    sprintf(nomDuFichier, "sauvegardes/sauvegarde(%s)", jourMoisAnnee);
    FILE *f = fopen(nomDuFichier, "w");

    if (f == NULL)
    {
        fprintf(stderr, "Le fichier de sauvegarde n'a pas pu être ouvert pour écrire\n");
        return EXIT_FAILURE;
    }
    for(int i=0; i<6; i++){
        char c;
        for(int j=0; j<7; j++)
        bufferWriter[j] = tabJetons[i][j];
        if(fwrite(bufferWriter, sizeof bufferWriter[0], 7, f) != 7){
            fprintf(stderr, "Erreur lors de l'écriture du tableau\n");
            return EXIT_FAILURE;
        }
    }
    if(fputc((char) numeroJoueur, f) == EOF)
        fprintf(stderr, "Erreur lors de l'écriture du dernier élement du tableau\n");
    if (fclose(f) == EOF)
    {
        fprintf(stderr, "Erreur lors de la fermeture du flux\n");
        return EXIT_FAILURE;
    }
    printf("Partie sauvegardée !\n");
    exit(0);
}

int restaurationSauvegarde(char **tabJetons){
    // Charger une partie
    printf("Restauration sauvegarde:\n");
    char rep[50];
    int err;
    char **nomsDesFichiers;
    int nbreChoisi;
    nomsDesFichiers = afficheFichiers("sauvegardes");
    printf("10. quitter le menu\n");
    boucle:
    printf("Quel fichier voulez-vous restaurer ? (entrez le numéro correspondant)\n");
    err = scanf("%d", &nbreChoisi);
    if(err != 1 || nbreChoisi<0 || nbreChoisi>10){
        videTampon();
        goto boucle;
    }
    else if(nbreChoisi == 10)
        return 0;

    sprintf(rep, "sauvegardes/%s", nomsDesFichiers[nbreChoisi]);
    FILE *f = fopen(rep, "r");
    
    if(f == NULL){
        fprintf(stderr, "Le fichier de sauvegarde n'a pas pu être ouvert\n");
        return 1;
    }
    int i, j = 0;
    char c;
    for(int i=0; i<6; i++)
        for(int j=0; j<7; j++){
            tabJetons[i][j] = getc(f);
            if(tabJetons[i][j] == EOF)
                printf("Problème lors du getc(f)\n");
        }
    int numeroJoueur = getc(f);
    if (fclose(f) == EOF)
    {
        fprintf(stderr, "Erreur lors de la fermeture du flux\n");
        return EXIT_FAILURE;       
    }
    if(numeroJoueur){
        routineDeJeu(tabJetons, 1);
        return 1;
    }
    dessineTableau(tabJetons);
    printf("tableau affiché\n");

    return 0;
    printf("Fin restauration sauvegarde\n");
}


void videTampon(){
    char poubelle; 
    do{
        poubelle =  getchar();
    }
    while(poubelle != '\n');
}

int routineDeJeu(char **tabJetons, short int numeroJoueur){ // retourne 1 si erreur
    int numeroColonne;
    int err;
    int s;
    boucle:
    dessineTableau(tabJetons);
    printf("Joueur %hd: ", numeroJoueur+1);
    err = scanf("%d", &numeroColonne);
    quitter(tabJetons); // On vérifie que l'utilisateur ne veuille pas quitter
    s = sauvegarder(tabJetons, numeroJoueur); // On vérifie que l'utilisateur ne veuille pas sauvegarder
    if(s == 1)
        goto boucle;
    if(err != 1 || numeroColonne < 1 || numeroColonne > 7){
        fprintf(stderr, "Entrez un chiffre valide !\n");
        videTampon();
        goto boucle;
    }
    if(s!=1)
        placeJeton(tabJetons, numeroColonne, numeroJoueur);

    return 0;
}

void placeJeton(char **tabJetons, int numeroColonne, int numeroJoueur){
    int numeroLigne;
    if(tabJetons[0][numeroColonne-1] != ' '){
        printf("Vous ne pouvez plus mettre de jetons là\n");
        routineDeJeu(tabJetons, numeroJoueur);
    }
    for(int i=1; i<6; i++){
        if(tabJetons[i][numeroColonne-1] != ' '){
            tabJetons[i-1][numeroColonne-1] = numeroJoueur ? 'O' : 'X';
            numeroLigne = i;
            break;
        }
    }
    if(tabJetons[5][numeroColonne-1] == ' '){
        tabJetons[5][numeroColonne-1] = numeroJoueur ? 'O' : 'X';
        numeroLigne = 6;
    }
        
    struct position pos = {numeroLigne, numeroColonne};
    int max = maxNbreJetonsIdentiques(pos, tabJetons, numeroJoueur ? 'O' : 'X');
    if(max >= 4){
        dessineTableau(tabJetons);
        annonceVictoire(numeroJoueur ? 'O' : 'X');
    }
}

bool positionValide(struct position pos){
    if(pos.ligne<1 || pos.ligne>6)
        return false;
    if(pos.colonne<1 || pos.colonne>7)
        return false;
    return true;
}

int calculNbreJetonsDepuisVers(struct position depart, unsigned int deplacementH, unsigned int deplacementV, char **tabJetons, char jeton){
    if(!positionValide(depart)){
        printf("position initiale non-valide");
        return 0;
    }
    struct position pos = depart;

    unsigned int nb = 0;
    while(positionValide(pos) && tabJetons[pos.ligne-1][pos.colonne-1] == jeton){
        pos.ligne += deplacementV;
        pos.colonne += deplacementH;
        nb++;
    }
    return nb;
}

void annonceVictoire(char jeton){
    switch (jeton)
    {
    case 'X':
        printf("Le joueur 1 à gagné la partie !\n");
        exit(0);
        break;
    case 'O':
        printf("Le joueur 2 à gagné la partie !\n");
        exit(0);
        break;
    default:
        printf("Erreur: personne n'a gagné la partie ? \n:( \n");
        break;
    }
}

char **afficheFichiers(char *directoryPath){
    struct dirent *dir;
    static char *nomsDesFichiers[10];
    // opendir() renvoie un pointeur de type DIR. 
    DIR *d = opendir(directoryPath); 
    if (d)
    {
        int i = 0;
        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0 && i<10){
                fprintf(stdout, "%d. %s\n", i, dir->d_name);
                nomsDesFichiers[i] = dir->d_name;
                i++;
            }
        }
        closedir(d);
    }
    return nomsDesFichiers;
}

int maxNbreJetonsIdentiques(struct position depart, char **tabJetons, char jeton){
// Position contient depart.ligne et depart.colonne allant respectivement de 1 à 6 et de 1 à 7
// Pour avoir l'emplacement dans le tableau, faites (-1)
    if(!positionValide(depart)){
        return -1;
        printf("position non valide !\n");
    }

    int tabMax[4];
    tabMax[0] = calculNbreJetonsDepuisVers(depart, 1, 0, tabJetons, jeton);
    tabMax[0] += (calculNbreJetonsDepuisVers(depart, -1, 0, tabJetons, jeton)-1);
    tabMax[1] = calculNbreJetonsDepuisVers(depart, 0, 1, tabJetons, jeton);
    tabMax[1] += (calculNbreJetonsDepuisVers(depart, 0, -1, tabJetons, jeton)-1);
    tabMax[2] = calculNbreJetonsDepuisVers(depart, 1, 1, tabJetons, jeton);
    tabMax[2] += (calculNbreJetonsDepuisVers(depart, -1, -1, tabJetons, jeton)-1);
    tabMax[3] = calculNbreJetonsDepuisVers(depart, 1, -1, tabJetons, jeton);
    tabMax[3] += (calculNbreJetonsDepuisVers(depart, -1, 1, tabJetons, jeton)-1);


    int max = tabMax[0];
    for(int i=1; i<4; i++){
        if(tabMax[i] > max)
            max = tabMax[i];
    }

    return max;
}

int *emplacementsPossibles(char **tabJetons){
    // Attention, les emplacements possibles sont rendus sous forme d'indice dans le tableau tabJetons
    static int ligneEmplacementPossible[7];
    for(int i=0; i<7; i++){
        for(int j=0; j<6; j++)
            if(tabJetons[j][i] != ' '){
                ligneEmplacementPossible[i] = j-1;
                break;
            }
            else if(tabJetons[5][i] == ' ')
                ligneEmplacementPossible[i] = 5;
    }
    // Si l'emplacement est impossible -> renvoie (-1)
    return ligneEmplacementPossible;
}

int choisiChiffreAleatoire(int maxPourColonne[7], int max){
    /*  Cette fonction prend en entrée un tableau avec le nombres de jetons simililaires maximum par colonne si 
    le jeton était placé à la colonne i et le nombre de jetons similaires max.
    Elle rend l'indice d'un nombre du tableau maxPourColonne qui vaut max aléatoirement
   */
    int nbreDeMax = 0;
    for(int i=0; i<7; i++)
        if(maxPourColonne[i] == max){
            nbreDeMax++;
    }
    int indicesDesMax[nbreDeMax];

    int i=0;
    for(int j=0; j<7; j++)
        if(maxPourColonne[j] == max){
            indicesDesMax[i] = j;
            i++;
        }
    if(nbreDeMax == 1)
        return indicesDesMax[0];

// il faut choisir un nombre aléatoire de 1 à nbreDeMax
    int nbreAleatoire;

    nbreAleatoire = (int)(rand()/(RAND_MAX +1.) * nbreDeMax); // varie de 0 à nbreDeMax-1

    return indicesDesMax[nbreAleatoire];
}

int choisirEmplacement(char **tabJetons, int* ligneEmplacementPossible){
    
    int maxJetonsIdentiquesX[7];
    int maxJetonsIdentiquesO[7];
    int maxJetonsIdentiques[7];
    for(int i=0; i<7; i++){
        if(ligneEmplacementPossible[i] < 0){ // Si on ne peut plus rajouter de jetons là
            maxJetonsIdentiquesX[i] = -1;
            continue;
        }
        struct position depart = {ligneEmplacementPossible[i]+1, i+1};
        tabJetons[depart.ligne-1][depart.colonne-1] = 'X';
        maxJetonsIdentiquesX[i] = maxNbreJetonsIdentiques(depart, tabJetons, 'X');
        tabJetons[depart.ligne-1][depart.colonne-1] = ' ';
    }
    for(int i=0; i<7; i++){
        if(ligneEmplacementPossible[i] < 0){ // Si on ne peut plus rajouter de jetons là
            maxJetonsIdentiquesO[i] = -1;
            continue;
        }
        
        struct position depart = {ligneEmplacementPossible[i]+1, i+1};
        tabJetons[depart.ligne-1][depart.colonne-1] = 'O';
        maxJetonsIdentiquesO[i] = maxNbreJetonsIdentiques(depart, tabJetons, 'O');
        tabJetons[depart.ligne-1][depart.colonne-1] = ' ';
    }
    
    // 1ère sélection : On retient le jeton qui à le plus de Jetons identiques à ses côtés
    for(int i=0; i<7; i++){
        maxJetonsIdentiques[i] = maxJetonsIdentiquesX[i] >= maxJetonsIdentiquesO[i] ? maxJetonsIdentiquesX[i] : maxJetonsIdentiquesO[i];
    }
    // 2ème sélection: on cherche la colonne où il y a le plus de jetons similaires ensembles
    int max = maxJetonsIdentiques[0];
    for(int i=1; i<7; i++)
        if(maxJetonsIdentiques[i] > max){
            max = maxJetonsIdentiques[i];
        }

    int indice_choisi = choisiChiffreAleatoire(maxJetonsIdentiques, max);

    return indice_choisi;
}

void ia(char **tabJetons, int numeroJoueur){
    dessineTableau(tabJetons);
    int *ligneEmplacement = emplacementsPossibles(tabJetons);
    int meilleurEmplacement = choisirEmplacement(tabJetons, ligneEmplacement)+1;
    placeJeton(tabJetons, meilleurEmplacement, numeroJoueur);   
    // dessineTableau(tabJetons);
}

int main(){
    short int nbreJoueurs;
    printf("Combien de joueurs souhaitent-ils jouer ? (1 ou 2 joueurs)\n");
    short unsigned int fonctionne = scanf("%hd", &nbreJoueurs);
    while(fonctionne != 1 || nbreJoueurs>2 || nbreJoueurs<1){
        videTampon();
        printf("Entrez un chiffre entre 1 et 2 !\n");
        fonctionne = scanf("%hd", &nbreJoueurs);
    }
    time_t t;

    if (time(&t) == (time_t)-1)
    {
        fprintf(stderr, "Impossible d'obtenir la date courante\n");
        return EXIT_FAILURE;
    }

    srand((unsigned)t);

    char **tabJetons = allocationTableau(); // Tableau des différentes pièces jouées par les adversaires
    initialiseTableau(tabJetons);
    unsigned int tour = 0;
    dessineTableau(tabJetons);
    printf("Où souhaitez vous jouer votre pièce ?\n");
    printf("choisissez le numéro de la case ou vous voulez placer le jeton\n\n");
    while(1){
        printf("                          TOUR %d:\n\n", tour);
        
        routineDeJeu(tabJetons, 0);
        
        if(nbreJoueurs == 2)
            routineDeJeu(tabJetons, 1);
        else
            ia(tabJetons, 1);
        tour++;
    }

    return 0;
}