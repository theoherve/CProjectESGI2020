/* //test code SDL
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

void sdl_error(char *error)
{
    fprintf(stderr, "SDL Error : %s\n", error);
}

int main(int argc, char **argv)
{
    SDL_Window    *window;
    SDL_Renderer *renderer;

    window = NULL;
    renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        sdl_error("initialization failed");
        return 0;
    }

    window = SDL_CreateWindow(
        "Tétris",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        500,
        500,
        SDL_WINDOW_SHOWN
    );

    if (!window)
    {
        sdl_error("window creation failed");
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        sdl_error("renderer creation failed");
        return 0;
    }

    sdl_error("renderer creation ok");

    SDL_Event event;

    while(1)
    { // SDL loop
        while( SDL_PollEvent( &event ) != 0 )
        {
            if( event.type == SDL_QUIT )
            {
                // Ctrl + C in console !
            }
        } // end of handling event.
    }

    return 1;
} */

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


/*
	Le programme a pour but d'afficher les différents champs d'informations de chaques restaurants
	écrit dans un fichier.
*/
int main(int argc, char **argv){

    FILE *fp = NULL;

    getApiViaCurl(fp);

    readFile(fp);

    return 0;

}
