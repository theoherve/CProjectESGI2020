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

void getApiViaCurl(FILE *fp){

    CURL *curl;
    int result;
    char errbuf[CURL_ERROR_SIZE];

    fp = fopen("testCurlApiExport/script1.json", "wb");

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
            }else{
                fprintf(stderr, "%s\n", curl_easy_strerror(result));
                printf("Download successful !\n");
            }
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }

}

char *getData(FILE *fp, char *data){
	char buffer;
	int i;

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
}

/*void compareFile(FILE *fp){

    FILE *fp2;

    fp = fopen("testCurlApiExport/script1.json", "rb");     //open the existing file
    if(fp != NULL){
        newfile = malloc(sizeof(char)*strlen(fileName)+1);
    }

    fp2 = fopen("testCurlApiExport/script1.json", "rb");    //open the new downloaded file
    if(fp2 != NULL){
        fseek(fp, 0, SEEK_END);
        position = ftell(fp);
        fseek(fp, 0, SEEK_SET);

    }

}*/

void readFile(FILE *fp){

    char buffer;
	char *key;
	char *data;
	int i;			// Permet de naviguer dans la variable key pour construire le mot clé
	int iData;		// Stock un nombre qui correspond au nombre de champ récupérer

	iData = 0;

	key = malloc(sizeof(char) * 255);
	data = malloc(sizeof(char) * 255);

	// Ouvre le fichier "script1.json" en lecture bit
	fp = fopen("script1.json", "rb");

	while(fread(&buffer, sizeof(char), 1, fp)){// Lit un caractère qui sera stocké dans un buffer puis fait avancer le curseur de lecture
		if(buffer == '"'){ // => Début de construction du mot clé
			i = 0;
			while(fread(&buffer, sizeof(char), 1, fp) && buffer != '"'){ // => Continue la construction du mot clé tant que le buffer ne lit pas '"'
				key[i] = buffer;
				i++;
			}
			key[i] = '\0';

			if(!strcmp(key, "geo_point_2d") || !strcmp(key, "date_periode") || !strcmp(key, "lieu1") || !strcmp(key, "libelle_type")) // Vérifie si le mot key est identique à un des trois mots clés choisis
				data = getData(fp, data); // Appel de la fonction getData

			printf("%s = %s\n", key, data);

			iData++;			//
								// Permet de faire un saut de
			if(iData % 3 == 0)	// ligne tout les 3 champs lus
				printf("\n");	// (juste pour la lisibilité dans la console)
		}
	}

}


/*
	Le programme a pour but d'afficher les différentes champs d'informations de chaque restaurant
	écrit dans un fichier.
*/
int main(int argc, char **argv){

    FILE *fp;

    getApiViaCurl(fp);

    //compareFile(fp);

    readFile(fp);

    return 0;

}
