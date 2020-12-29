/*
Programmed by HERVÉ Théo and DE FARIA LEITE Armand
December 2020
Display Paris's bars (retrieve Api information via Curl on a file, sort the information we need and put them in database)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl.h>
#include <MYSQL/mysql.h>

void getApiViaCurl(FILE *fp){

    CURL *curl;
    int result;
    char errbuf[CURL_ERROR_SIZE];

    fp = fopen("exportFolder/DownloadedAPI.json", "wb");

    curl = curl_easy_init(); //initialize CURL fonction
    if(curl){

        //curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
        curl_easy_setopt(curl, CURLOPT_URL, "https://opendata.paris.fr/explore/dataset/etalages-et-terrasses/download/?format=json&timezone=Europe/Berlin&lang=fr"); //CURLOPT_ULR allow us to enter the url of the file we want to dl
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //alow to write on the file we dl
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = 0;
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);//enable progress meter

        result = curl_easy_perform(curl); //return if the dl was successful (might take few seconds)

        if(result != CURLE_OK) {
            size_t len = strlen(errbuf);
            fprintf(stderr, "\nlibcurl: (%d) ", result);
            if(len){
                fprintf(stderr, "%s%s", errbuf, ((errbuf[len - 1] != '\n') ? "\n" : ""));
            }
        }else{
                fprintf(stderr, "%s\n", curl_easy_strerror(result));
                printf("Download successful !\n");
            }

        fclose(fp);
        curl_easy_cleanup(curl);
    }

}

char *getData(FILE *fp, char *data){
	char buffer;

	while(fread(&buffer, sizeof(char), 1, fp)){
		if(buffer == '"' || buffer == '['){
			int i = 0;
			while(fread(&buffer, sizeof(char), 1, fp) && buffer != '"' && buffer != ']'){
				data[i] = buffer;
				i++;
			}
			data[i] = '\0';
			return data;
		}
	}
	return NULL;
}

void sendBDD(MYSQL mysql, char *data_geo_point_2d, char *data_date_periode, char *data_lieu1, char *data_libelle_type){
    char query[255];
    MYSQL_RES *result=NULL;
    MYSQL_ROW row;

    if(strcmp(data_date_periode,"Toute l'ann\u00e9e"))
        strcpy(data_date_periode,"Toute l\\'année");

    strcpy(query, "SELECT * FROM BAR WHERE geo_point_2d='");
    strcat(query, data_geo_point_2d);
    strcat(query, "' AND date_periode='");
    strcat(query, data_date_periode);
    strcat(query, "' AND lieu1='");
    strcat(query, data_lieu1);
    strcat(query, "' AND libelle_type='");
    strcat(query, data_libelle_type);
    strcat(query, "'");

    mysql_query(&mysql,query);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);

    if(!row){
        strcpy(query,"INSERT INTO BAR (geo_point_2d, libelle_type, date_periode, lieu1) VALUES('");
        strcat(query,data_geo_point_2d);
        strcat(query,"','");
        strcat(query,data_libelle_type);
        strcat(query,"','");
        strcat(query,data_date_periode);
        strcat(query,"','");
        strcat(query,data_lieu1);
        strcat(query,"')");
        mysql_query(&mysql,query);
        //printf("%s\n", query);
    }else{
        //printf("already exist\n");
    }
}

void readFile(FILE *fp){

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    char *data_geo_point_2d;
    char *data_libelle_type;
    char *data_date_periode;
    char *data_lieu1;

    char buffer;
	char *key;
	int i;			// Permet de naviguer dans la variable key pour construire le mot clé
	int iData;		// Stock un nombre qui correspond au nombre de champ récupérer

	iData = 0;

	key = malloc(sizeof(char) * 255);

	data_geo_point_2d = malloc(sizeof(char) * 255);
    data_libelle_type = malloc(sizeof(char) * 255);
    data_date_periode = malloc(sizeof(char) * 255);
    data_lieu1 = malloc(sizeof(char) * 255);

	if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        // Ouvre le fichier "DownloadedAPI.json" en lecture bit
        fp = fopen("exportFolder/DownloadedAPI.json", "rb");

        while(fread(&buffer, sizeof(char), 1, fp)){// Lit un caractère qui sera stocké dans un buffer puis fait avancer le curseur de lecture
            if(buffer == '"'){ // => Début de construction du mot clé
                i = 0;
                while(fread(&buffer, sizeof(char), 1, fp) && buffer != '"'){ // => Continue la construction du mot clé tant que le buffer ne lit pas '"'
                    key[i] = buffer;
                    i++;
                }
                key[i] = '\0';
                if(!strcmp(key, "geo_point_2d") || !strcmp(key, "date_periode") || !strcmp(key, "lieu1") || !strcmp(key, "libelle_type")){
                  if(!strcmp(key, "geo_point_2d")){ // Vérifie si le mot key est identique à un des trois mots clés choisis
                    data_geo_point_2d = getData(fp, data_geo_point_2d); // Appel de la fonction getData
                  }else if(!strcmp(key, "date_periode")){
                    data_date_periode = getData(fp, data_date_periode); // Appel de la fonction getData
                  }else if(!strcmp(key, "lieu1")){
                    data_lieu1 = getData(fp, data_lieu1); // Appel de la fonction getData
                  }else if(!strcmp(key, "libelle_type")){
                    data_libelle_type = getData(fp, data_libelle_type); // Appel de la fonction getData
                  }

                    iData++;			//
                                        // Permet de faire un saut de
                    if(iData % 4 == 0){	// ligne tout les 4 champs lus
                        //printf("%s = %s\n", key, data);
                        //printf("\n");	// (juste pour la lisibilité dans la console)

                        sendBDD(mysql, data_geo_point_2d, data_date_periode, data_lieu1, data_libelle_type);    //call sendBDD
                    }
                }
            }
        }
	}else{
        printf("\nERROR: An error occurred while connecting to the DB!\n\n");
	}

}

void loadBars(){

    FILE *fp = NULL;

    getApiViaCurl(fp);

    readFile(fp);

    return;
}

void printSelectionCategory(char* type, MYSQL mysql){
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    char query[255];
    char printer[255];
    int count_row = 0;

    strcpy(printer, "here's are all the ");
    strcat(printer, type);
    strcat(printer, " of the city of Paris :\n\n");
    printf("%s\n", printer);

    strcpy(query, "SELECT id, lieu1, date_periode FROM BAR WHERE libelle_type='");
    strcat(query, type);
    strcat(query, "'");
    printf("%s\n", query);

    mysql_query(&mysql,query);
    result = mysql_store_result(&mysql);
    while((row = mysql_fetch_row(result))){
        printf("[%d] %s - %s\n",count_row+1,row[1],row[2]);
        count_row++;
    }

}

void selectFromCategory(MYSQL mysql){
    system("cls");

    int categorySelected;
    int count_row = 0;

    printf("Here you can find the type of place you want to go !\n");

    do{
        count_row=0;
        printf("\n\nChoose the type of place among what there is :\n\n");
        printf("1 - COMMERCE ACCESSOIRE\n\n");
        printf("2 - CONTRE ETALAGE\n\n");
        printf("3 - CONTRE TERRASSE\n\n");
        printf("4 - ETALAGE\n\n");
        printf("5 - PLANCHER MOBILE\n\n");
        printf("6 - TERRASSE FERMEE\n\n");
        printf("7 - TERRASSE OUVERTE\n\n");
        printf("0 - Back\n\n");
        scanf("%d", &categorySelected);

        switch(categorySelected){
            case 0:
                return;
                break;
            case 1:
                printSelectionCategory("COMMERCE ACCESSOIRE", mysql);
                break;
            case 2:
                printSelectionCategory("CONTRE ETALAGE", mysql);
                break;
            case 3:
                printSelectionCategory("CONTRE TERRASSE", mysql);
                break;
            case 4:
                printSelectionCategory("ETALAGE", mysql);
                break;
            case 5:
                printSelectionCategory("PLANCHER MOBILE", mysql);
                break;
            case 6:
                printSelectionCategory("TERRASSE FERMEE", mysql);
                break;
            case 7:
                printSelectionCategory("TERRASSE OUVERTE", mysql);
                break;
        }
    }while(1);

}

void researchFromAddress(MYSQL mysql){
    system("cls");

    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    char request[255];
    char query[255];
    int count_row = 0;
    int choice;

    printf("Here you can make a research from an entered address !\n\n");

    do{
        printf("\n\nPlease enter the street where you want to look for a bar :\n");
        fflush(stdin);
        fgets(request,255,stdin);
        if(request[strlen(request)-1] == '\n'){
               request[strlen(request)-1]='\0';
           }
        strcpy(query, "SELECT id, lieu1, date_periode, libelle_type FROM BAR WHERE (libelle_type = 'TERRASSE FERMEE' OR libelle_type = 'TERRASSE OUVERTE') AND lieu1 LIKE '%");
        strcat(query, request);
        strcat(query, "%'");
        printf("\n");
        mysql_query(&mysql,query);
        result = mysql_store_result(&mysql);
        while((row = mysql_fetch_row(result))){
            printf("[%d] %s - %s - %s\n",count_row+1,row[1],row[2], row[3]);
            count_row++;
        }
        printf("\n\nDo you want to make another research or get back ?\n\n");
        printf("1 - Another research\n");
        printf("0 - Get back\n");
        scanf("%d", &choice);

        switch(choice){
            case 0:
                return;
                break;
            case 1:
                researchFromAddress(mysql);
                break;
        }

    }while(1);

}

void selectMenu(MYSQL mysql){

    int menuSelection;

    do{
        system("cls");

        printf("*** BARS ***\n\n");
        printf("1 - Load bar list (no need to do it everytime, your favorite bar will not disapear :D)\n\n");
        printf("2 - Selection from a category\n\n");
        printf("3 - Make a research from an entry\n\n");
        printf("0 - Leave the program\n\n");
        scanf("%d", &menuSelection);

        switch(menuSelection){
            case 0:
                printf("\nEnjoy your drinks ;)\nSee you next time !\n");
                return;
                break;
            case 1:
                loadBars();
                break;
            case 2:
                selectFromCategory(mysql);
                break;
            case 3:
                researchFromAddress(mysql);
                break;
        }

    }while(1);
}


/*
	Le programme a pour but d'afficher les différents champs d'informations de chaques restaurants
	écrit dans un fichier.
*/
int main(int argc, char **argv){

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0))
        selectMenu(mysql);
    else
        printf("ERROR: An error occurred while connecting to the DB!");

    return 0;

}
