/*
Programmed by HERV� Th�o and DE FARIA LEITE Armand
December 2020
Display Paris's bars (retrieve Api information via Curl on a file, sort the information we need and put them in database)
*/

//start real code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <winsock.h>
#include <MYSQL/mysql.h>
#include <time.h>
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <curl.h>

SDL_Window    *window=NULL;
SDL_Renderer *renderer=NULL;
TTF_Font *font = NULL;
SDL_Surface *text=NULL;
SDL_Surface *surface=NULL;
SDL_Texture *texture;
SDL_Color font_color = {0, 0, 0};
SDL_Event event;
SDL_Rect position;
SDL_Color background={213,115,51,255};
SDL_Color items={129,120,115,255};
int r_color=129;
int g_color=120;
int b_color=115;

int app_mod;
char txt_font[255];

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
                fprintf(stderr, "\n%s\n", curl_easy_strerror(result));
                printf("Download successful !\nLoading...\n");
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

void delDuplicateBDD(MYSQL mysql){

    char query[255];
    char query2[255];
    MYSQL_RES *result=NULL;
    MYSQL_ROW row;

    MYSQL_RES *result2=NULL;
    MYSQL_ROW row2;

    int row_counter = 0;
    char ***tabI;
    int isDuplicated;
    int i2 = 0;
    int j2;
    int nbrRow = 0;

    strcpy(query, "SELECT id, lieu1, geo_point_2d FROM BAR");
    mysql_query(&mysql, query);
    result = mysql_store_result(&mysql);
    while((row = mysql_fetch_row(result))){
        row_counter++;
    }

    tabI = malloc(sizeof(char**)*row_counter+1);
    for(int i = 0; i < row_counter+1 ; i++){
        tabI[i] = malloc(sizeof(char*)*2);
        for(int j = 0; j < 2 ; j++){
            tabI[i][j] = malloc(sizeof(char)*255);
        }
    }

    mysql_query(&mysql, query);
    result = mysql_store_result(&mysql);
    while((row = mysql_fetch_row(result))){
        isDuplicated = 0;
        for(j2 = 0; j2 < i2; j2++){
            if(!strcmp(tabI[j2][0], row[1]) && !strcmp(tabI[j2][1], row[2])){
                isDuplicated = 1;
                strcpy(query2, "DELETE FROM BAR WHERE id='");
                strcat(query2, row[0]);
                strcat(query2, "'");
                mysql_query(&mysql, query2);
            }
        }if(!isDuplicated){
            nbrRow++;
            i2++;
            tabI[i2][0] = row[1];
            tabI[i2][1] = row[2];
        }
    }
    return;
}

void sendBDD(MYSQL mysql, char *data_geo_point_2d, char *data_date_periode, char *data_lieu1, char *data_libelle_type){
    char query[255];
    MYSQL_RES *result=NULL;
    MYSQL_ROW row;

    if(strcmp(data_date_periode,"Toute l'ann\u00e9e"))
        strcpy(data_date_periode,"Toute l\\'ann�e");

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
	int i;			// Permet de naviguer dans la variable key pour construire le mot cl�
	int iData;		// Stock un nombre qui correspond au nombre de champ r�cup�rer

	iData = 0;

	key = malloc(sizeof(char) * 255);

	data_geo_point_2d = malloc(sizeof(char) * 255);
    data_libelle_type = malloc(sizeof(char) * 255);
    data_date_periode = malloc(sizeof(char) * 255);
    data_lieu1 = malloc(sizeof(char) * 255);

	if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        // Ouvre le fichier "DownloadedAPI.json" en lecture bit
        fp = fopen("exportFolder/DownloadedAPI.json", "rb");

        while(fread(&buffer, sizeof(char), 1, fp)){// Lit un caract�re qui sera stock� dans un buffer puis fait avancer le curseur de lecture
            if(buffer == '"'){ // => D�but de construction du mot cl�
                i = 0;
                while(fread(&buffer, sizeof(char), 1, fp) && buffer != '"'){ // => Continue la construction du mot cl� tant que le buffer ne lit pas '"'
                    key[i] = buffer;
                    i++;
                }
                key[i] = '\0';
                if(!strcmp(key, "geo_point_2d") || !strcmp(key, "date_periode") || !strcmp(key, "lieu1") || !strcmp(key, "libelle_type")){
                  if(!strcmp(key, "geo_point_2d")){ // V�rifie si le mot key est identique � un des trois mots cl�s choisis
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
                        //printf("\n");	// (juste pour la lisibilit� dans la console)

                        sendBDD(mysql, data_geo_point_2d, data_date_periode, data_lieu1, data_libelle_type);    //call sendBDD
                    }
                }
            }
        }
	}else{
        printf("\nERROR: An error occurred while connecting to the DB!\n\n");
	}

}

void loadBars(MYSQL mysql){

    FILE *fp = NULL;

    getApiViaCurl(fp);

    readFile(fp);

    delDuplicateBDD(mysql);

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

        while(categorySelected <0 || categorySelected > 7){
            printf("\nYour choice does not correspond to the expectations !\n");
            printf("Please retype your selection :\n");
            scanf("%d", &categorySelected);
        }

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

void runNavigator(MYSQL mysql, char query[255]){
    int choice;
    int idChoice;
    int count_row = 0;
    char query2[255];
    char httpRequest[255];
    int i = 0;
    int tmp;

    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    printf("\n\nChoose the search you want to run.\n");
    printf("Select the search by its number :\n");
    scanf("%d", &choice);
    mysql_query(&mysql,query);
    result = mysql_store_result(&mysql);
    while((row = mysql_fetch_row(result))){
        printf("[%d] %s - %s - %s\n",count_row+1,row[1],row[2], row[3]);
        if((count_row+1) == choice){
            idChoice = row[0];
        }

        count_row++;
    }

    strcpy(query2, "SELECT id, lieu1, date_periode, libelle_type, geo_point_2d FROM BAR WHERE id= '");
    strcat(query2, idChoice);
    strcat(query2, "'");
    mysql_query(&mysql,query2);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);

    strcpy(httpRequest, "start https://www.google.fr/maps/search/");
    if(row){
        printf("[%s] %s - %s - %s - %s\n",row[0], row[1], row[2], row[3], row[4]);
        while(row[4][i++]){
            if(row[4][i] == ' '){
                strncat(httpRequest, row[4], i);
                strcat(httpRequest, "+");
                strcat(httpRequest, row[4]+(i+1));
            }
        }
        printf("httpRequest : %s", httpRequest);
        system(httpRequest);
    }
    selectMenu(mysql);
}


void researchFromAddress(MYSQL mysql){
    system("cls");

    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    char request[255];
    char query[255];
    int choice;

    printf("Here you can make a research from an entered address !\n\n");

    do{
        int count_row = 0;
        printf("\n\nPlease enter the street where you want to look for a bar :\n");
        fflush(stdin);
        fgets(request,255,stdin);
        if(request[strlen(request)-1] == '\n'){
               request[strlen(request)-1]='\0';
           }
        strcpy(query, "SELECT id, lieu1, date_periode, libelle_type, geo_point_2d FROM BAR WHERE (libelle_type = 'TERRASSE FERMEE' OR libelle_type = 'TERRASSE OUVERTE') AND lieu1 LIKE '%");
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
        printf("1 - Make another research\n");
        printf("2 - Run the research on your navigator\n");
        printf("0 - Get back\n");
        scanf("%d", &choice);

        while(choice <0 || choice > 2){
            printf("\nYour choice does not correspond to the expectations !\n");
            printf("Please retype your choice :\n");
            scanf("%d", &choice);
        }

         switch(choice){
            case 0:
                return;
                break;
            case 1:
                researchFromAddress(mysql);
                break;
            case 2:
                runNavigator(mysql, query);
                break;
        }

    }while(1);

}



int SignIn(){

    char pseudo[50];
    char password[100];
    char query[255];
    int check=1;
    MYSQL_RES *result=NULL;
    MYSQL_ROW row;
    int id;


    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");


    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        printf("\nSign in:\n");

        do{
           check=1;
           printf("Enter your pseudo:\n\n");
           fflush(stdin);
           fgets(pseudo,50,stdin);
           if(pseudo[strlen(pseudo)-1]=='\n'){
               pseudo[strlen(pseudo)-1]='\0';
           }

            printf("Enter your password:\n\n");
            fflush(stdin);
            fgets(password,100,stdin);
            if(password[strlen(password)-1]=='\n'){
                password[strlen(password)-1]='\0';
            }

            strcpy(query,"SELECT id,password FROM USER WHERE pseudo='");
            strcat(query,pseudo);
            strcat(query,"'");

            mysql_query(&mysql,query);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);

            if(!row){
                printf("Incorrect pseudo");
                check=0;
            }

            if(check!=0){
                if(strstr(password,row[1])==NULL){
                    printf("Incorrect password");
                    check=0;
                }
            }

        }while(check!=1);

        printf("You are connected");
        sscanf(row[0],"%d",&id);

    }else{
        printf("ERROR: An error occurred while connecting to the DB!");
    }
    return id;

}

int SignIn_SDL(){

    char pseudo[50];
    char password[100];
    char query[255];
    int check=1;
    MYSQL_RES *result=NULL;
    MYSQL_ROW row;
    int id;

    //SDL
    int check_input=0;
    char hide_password[100];


    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");


    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        do{
           check=1;
           check_input=0;
           strcpy(pseudo,"");
           strcpy(password,"");
           strcpy(hide_password,"");

           do{

                SDL_WaitEvent(&event);
                if(event.type == SDL_TEXTINPUT){
                    strcat(pseudo,event.text.text);
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                    pseudo[strlen(pseudo)-1]='\0';
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                    check_input=1;
                }

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign In",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Enter your pseudo:",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                text=TTF_RenderText_Blended(font,pseudo,font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=150;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);

            }while(check_input!=1);

            check_input=0;

            do{

                SDL_WaitEvent(&event);
                if(event.type == SDL_TEXTINPUT){
                    strcat(password,event.text.text);
                    strcat(hide_password,"*");
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                    password[strlen(password)-1]='\0';
                    hide_password[strlen(hide_password)-1]='\0';
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                    check_input=1;
                }

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign In",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Enter your password:",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                text=TTF_RenderText_Blended(font,hide_password,font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=150;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);

            }while(check_input!=1);

            strcpy(query,"SELECT id,password FROM USER WHERE pseudo='");
            strcat(query,pseudo);
            strcat(query,"'");

            mysql_query(&mysql,query);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);

            if(!row){

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign In",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Incorect pseudo",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);

                check=0;
            }

            if(check!=0){
                if(strstr(password,row[1])==NULL){

                    SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);

                    font=TTF_OpenFont(txt_font, 35);
                    text=TTF_RenderText_Blended(font,"Sign In",font_color);
                    position.x=0;
                    position.y=0;
                    texture= SDL_CreateTextureFromSurface(renderer,text);
                    SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                    position.x=20;
                    position.y=50;
                    SDL_RenderCopy(renderer, texture, NULL, &position);

                    font=TTF_OpenFont(txt_font, 20);
                    text=TTF_RenderText_Blended(font,"Incorect password",font_color);
                    position.x=0;
                    position.y=0;
                    texture= SDL_CreateTextureFromSurface(renderer,text);
                    SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                    position.x=15;
                    position.y=130;
                    SDL_RenderCopy(renderer, texture, NULL, &position);

                    SDL_RenderPresent(renderer);
                    SDL_Delay(3000);
                    check=0;
                }
            }

        }while(check!=1);

        printf("You are connected");
        sscanf(row[0],"%d",&id);

    }else{
        printf("ERROR: An error occurred while connecting to the DB!");
    }
    return id;

}

int SignUp(){

    char pseudo[50];
    char mail[100];
    char city[100];
    char password[100];
    char confirm_password[100];
    int check=1;
    char query[255];
    MYSQL_RES *result=NULL;
    MYSQL_ROW row;
    int id;

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        printf("\nSign up:\n");

        do{
            check=1;
            printf("Enter your pseudo:\n\n");
            fflush(stdin);
            fgets(pseudo,50,stdin);
            if(pseudo[strlen(pseudo)-1]=='\n'){
                pseudo[strlen(pseudo)-1]='\0';
            }

            strcpy(query,"SELECT id FROM USER WHERE pseudo='");
            strcat(query,pseudo);
            strcat(query,"'");

            mysql_query(&mysql,query);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);

            if(row){
                printf("\nPseudo déjà utilisé\n");
                check=0;
            }


        }while(check!=1);


        do{
            check=1;
            printf("Enter your mail:\n\n");
            fflush(stdin);
            fgets(mail,100,stdin);
            if(mail[strlen(mail)-1]=='\n'){
                mail[strlen(mail)-1]='\0';
            }

            if(strchr(mail,'@')==NULL){
                printf("\nThe mail forma is incorect\n");
                check=0;
            }

            if(strchr(mail,'.')==NULL && check!=0){
                printf("\nThe mail forma is incorect\n");
                check=0;
            }

            strcpy(query,"SELECT id FROM USER WHERE mail='");
            strcat(query,mail);
            strcat(query,"'");

            mysql_query(&mysql,query);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);

            if(row){
                printf("\nMail already used\n");
                check=0;
            }



        }while(check!=1);

            printf("Enter your city:\n\n");
            fflush(stdin);
            fgets(city,100,stdin);
            if(city[strlen(city)-1]=='\n'){
                city[strlen(city)-1]='\0';
            }


        do{
            check=1;
            printf("Enter your password:\n\n");
            fflush(stdin);
            fgets(password,100,stdin);
            if(password[strlen(password)-1]=='\n'){
                password[strlen(password)-1]='\0';
            }
            if(strlen(password)<8){
                printf("Password to short\n");
                check=0;
            }

            if(password[0]<65 || password[0]>90 && check!=0 ){
                printf("The first letter must be a capital letter\n");
                check=0;
            }

            if(strpbrk(password,"0123456789")==NULL && check!=0){
                printf("Your password must contain a number\n");
                check=0;
            }

        }while(check!=1);


        do{
            check=1;
           printf("Confirm your password:\n\n");
           fflush(stdin);
           fgets(confirm_password,100,stdin);
           if(confirm_password[strlen(confirm_password)-1]=='\n'){
              confirm_password[strlen(confirm_password)-1]='\0';
           }

           if(strstr(password,confirm_password)==NULL){
                printf("Both passwords must match\n");
                check=0;
           }

        }while(check!=1);

        printf("Pseudo: %s\nMail: %s\nCity: %s\nPassword %s\n",pseudo,mail,city,password);

        strcpy(query,"INSERT INTO USER (pseudo,mail,city,password) VALUES('");
        strcat(query,pseudo);
        strcat(query,"','");
        strcat(query,mail);
        strcat(query,"','");
        strcat(query,city);
        strcat(query,"','");
        strcat(query,password);
        strcat(query,"')");

        mysql_query(&mysql,query);

        strcpy(query,"SELECT id FROM USER WHERE pseudo='");
        strcat(query,pseudo);
        strcat(query,"'");

        mysql_query(&mysql,query);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);

        if(row){
            sscanf(row[0],"%d",&id);
            printf("\nWelcome %s, your account has been successfully created\n",pseudo);
        }else{
            printf("\nERROR: Your account can't be create\n");
        }

         mysql_close(&mysql);

    }else{
        printf("ERROR: An error occurred while connecting to the DB!");
    }

    return id;

}

int SignUp_SDL(){

    char pseudo[50];
    char mail[100];
    char city[100];
    char password[100];
    char confirm_password[100];
    int check=1;
    char query[255];
    MYSQL_RES *result=NULL;
    MYSQL_ROW row;
    int id;
    //SDL
    int check_input=0;
    char hide_password[100];

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        do{
            check=1;
            check_input=0;
            strcpy(pseudo,"");

            do{

                SDL_WaitEvent(&event);
                if(event.type == SDL_TEXTINPUT){
                    strcat(pseudo,event.text.text);
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                    pseudo[strlen(pseudo)-1]='\0';
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                    check_input=1;
                }

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Enter your pseudo:",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                text=TTF_RenderText_Blended(font,pseudo,font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=150;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);

            }while(check_input!=1);

            strcpy(query,"SELECT id FROM USER WHERE pseudo='");
            strcat(query,pseudo);
            strcat(query,"'");

            mysql_query(&mysql,query);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);

            if(row){
                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Pseudo already used",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);

                check=0;
            }


        }while(check!=1);


        do{
            check=1;
            check_input=0;
            strcpy(mail,"");

            do{

                SDL_WaitEvent(&event);
                if(event.type == SDL_TEXTINPUT){
                    strcat(mail,event.text.text);
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                    mail[strlen(mail)-1]='\0';
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                    check_input=1;
                }

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Enter your mail:",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                text=TTF_RenderText_Blended(font,mail,font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=150;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);

            }while(check_input!=1);

            if(strchr(mail,'@')==NULL){
                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"The mail forma is incorect",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);
                check=0;
            }

            if(strchr(mail,'.')==NULL && check!=0){
                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"The mail forma is incorect",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);
                check=0;
            }

            strcpy(query,"SELECT id FROM USER WHERE mail='");
            strcat(query,mail);
            strcat(query,"'");

            mysql_query(&mysql,query);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);

            if(row){
                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Mail already used",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);

                check=0;
            }



        }while(check!=1);

        check_input=0;

        do{

            SDL_WaitEvent(&event);
            if(event.type == SDL_TEXTINPUT){
                strcat(city,event.text.text);
            }

            if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                city[strlen(city)-1]='\0';
            }

            if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                check_input=1;
            }

            SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            font=TTF_OpenFont(txt_font, 35);
            text=TTF_RenderText_Blended(font,"Sign Up",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=20;
            position.y=50;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            font=TTF_OpenFont(txt_font, 20);
            text=TTF_RenderText_Blended(font,"Enter your City:",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=15;
            position.y=130;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            text=TTF_RenderText_Blended(font,city,font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=15;
            position.y=150;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            SDL_RenderPresent(renderer);

        }while(check_input!=1);


        do{
            check=1;
            check_input=0;

            do{

                SDL_WaitEvent(&event);
                if(event.type == SDL_TEXTINPUT){
                    strcat(password,event.text.text);
                    strcat(hide_password,"*");
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                    password[strlen(password)-1]='\0';
                    hide_password[strlen(hide_password)-1]='\0';
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                    check_input=1;
                }

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Enter your password:",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                text=TTF_RenderText_Blended(font,hide_password,font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=150;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);

            }while(check_input!=1);

            if(strlen(password)<8){

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Password to short",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);

                check=0;
            }

            if(password[0]<65 || password[0]>90 && check!=0 ){

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"The first letter must be a capital letter",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);

                check=0;
            }

            if(strpbrk(password,"0123456789")==NULL && check!=0){

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Your password must contain a number",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);

                check=0;
            }

        }while(check!=1);


        do{
            check=1;
            check_input=0;
            strcpy(confirm_password,"");
            strcpy(hide_password,"");

           do{

                SDL_WaitEvent(&event);
                if(event.type == SDL_TEXTINPUT){
                    strcat(confirm_password,event.text.text);
                    strcat(hide_password,"*");
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                    confirm_password[strlen(confirm_password)-1]='\0';
                    hide_password[strlen(hide_password)-1]='\0';
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                    check_input=1;
                }

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Confirm your password:",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                text=TTF_RenderText_Blended(font,hide_password,font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=150;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);

            }while(check_input!=1);

           if(strstr(password,confirm_password)==NULL){

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 35);
                text=TTF_RenderText_Blended(font,"Sign Up",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);
                text=TTF_RenderText_Blended(font,"Both passwords must match",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=15;
                position.y=130;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);
                SDL_Delay(3000);

                check=0;
           }

        }while(check!=1);

        printf("Pseudo: %s\nMail: %s\nCity: %s\nPassword %s\n",pseudo,mail,city,password);

        strcpy(query,"INSERT INTO USER (pseudo,mail,city,password) VALUES('");
        strcat(query,pseudo);
        strcat(query,"','");
        strcat(query,mail);
        strcat(query,"','");
        strcat(query,city);
        strcat(query,"','");
        strcat(query,password);
        strcat(query,"')");

        mysql_query(&mysql,query);

        strcpy(query,"SELECT id FROM USER WHERE pseudo='");
        strcat(query,pseudo);
        strcat(query,"'");

        mysql_query(&mysql,query);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);

        if(row){
            sscanf(row[0],"%d",&id);
            printf("\nWelcome %s, your account has been successfully created\n",pseudo);
        }else{
            printf("\nERROR: Your account can't be create\n");
        }

         mysql_close(&mysql);

    }else{
        printf("ERROR: An error occurred while connecting to the DB!");
    }

    return id;

}

void cocktails(int id){
    int choice;//Cette variable va nous servir à pouvoir naviger dans les différents menu de l'application.

    //Cette fonction est un sous menu qui nous permet de choisir entre deux autres fonctionnalité lié aux cocktail: créer un cocktail ou consulter la liste des cocktails existant

    printf("--COCKTAILS--\n");

    do{
        printf("\n1:Create a coktails\n2:List of users Cocktails\n3:Return to menu\n");//On a donc ici une boucle pour choisir entre ces deux fonctionnalité et l'utilisateur rentre le numéro associer pour s'y rendre.
        scanf("%d",&choice);
        if(choice==1){
            createCocktails(id);
        }

        if(choice==2){
            listCocktails(id);
        }
    }while(choice!=3);//On reste dans la boucle tant que l'utilisateur n'a pas décidé de revenir au menu principal
}

void cocktails_SDL(int id){
    int choice;
    int x_mouse;
    int y_mouse;

    do{
        choice=0;
        x_mouse=0;
        y_mouse=0;

        SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        font=TTF_OpenFont(txt_font, 45);
        text=TTF_RenderText_Blended(font,"COCKTAILS",font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=75;
        position.y=50;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        font=TTF_OpenFont(txt_font, 30);
        text=TTF_RenderText_Blended(font,"Create Cocktails",font_color);
        surface=NULL;
        surface = SDL_CreateRGBSurface(0, 280, 70, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
        position.x=20;
        position.y=15;
        SDL_BlitSurface(text,NULL,surface,&position);
        texture= SDL_CreateTextureFromSurface(renderer,surface);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=60;
        position.y=170;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        text=TTF_RenderText_Blended(font,"List of users",font_color);
        surface=NULL;
        surface = SDL_CreateRGBSurface(0, 280, 100, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
        position.x=50;
        position.y=15;
        SDL_BlitSurface(text,NULL,surface,&position);
        texture= SDL_CreateTextureFromSurface(renderer,surface);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=60;
        position.y=255;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        text=TTF_RenderText_Blended(font,"cocktails",font_color);
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=135;
        position.y=300;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        text=TTF_RenderText_Blended(font,"Menu",font_color);
        surface=NULL;
        surface = SDL_CreateRGBSurface(0, 120, 70, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
        position.x=20;
        position.y=15;
        SDL_BlitSurface(text,NULL,surface,&position);
        texture= SDL_CreateTextureFromSurface(renderer,surface);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=20;
        position.y=610;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        SDL_RenderPresent(renderer);

        do{

            SDL_WaitEvent(&event);

            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                SDL_GetMouseState(&x_mouse,&y_mouse);
            }

            if(x_mouse>=60 && x_mouse<=340 && y_mouse>=170 && y_mouse<=240){
                choice=1;
            }

            if(x_mouse>=60 && x_mouse<=340 && y_mouse>=255 && y_mouse<=355){
                choice=2;
            }

            if(x_mouse>=20 && x_mouse<=140 && y_mouse>=610 && y_mouse<=680){
                choice=3;
            }

        }while(choice!=1 && choice!=2 && choice!=3);

        if(choice==1){
            createCocktails_SDL(id);
        }

        if(choice==2){
            listCocktails_SDL(id);
        }

    }while(choice!=3);

}

void createCocktails(int id){

    char name_cocktail[100];//Cette variable va stocker le nom du Cocktail à créer.
    char txt_tmp[10];//Cette variable va nous servir à stocker les versions string de certaines valeur pour les mettre dans une requette sql.
    int id_cocktails;//Cette variable va stocker l'id du cocktail créer.
    int tab_ingredient[10];//Ce tableau va contenir les id des ingrédients à mettre dans notre cocktail.
    int tab_quantity[10];//Ce tableau va contenir la quantité de chaque ingrédient à mettre dans le cocktail
    char **tab_choice_ingredient;//Un double pointeur de char qui va devenir un tableau dynamique contenant l'id de chaque ingrédient
    int max=0;//Une variable qui va nous servir à limiter à 10 le nombre d'ingredient que l'on peut mettre dans le cocktail mais aussi à savoir combien d'ingrédient on a choisi.
    int choice;//C'est avec cette variable que l'utilisateur va séléctionner un ingrédient on décider de revenir au menu des cocktails.
    int quantity;//Cette variable va contenir la quantité d'un ingrédiant pour le cocktail
    char query[255];//Va contenir nos diiférente requête SQL
    unsigned int i = 0;
    int count_row=0;//Une variable tempon qui nous sert à savoir combien de ligne on traitre
    int check_count_row=0;//Variable tempon qui va nous servir à par la suite

    //SQL
    MYSQL mysql;//
    MYSQL_RES *result = NULL;//Ce pointeur va contenir le jeu de résultat de notre requête
    MYSQL_ROW row;//Cette variable va contenir les lignes de nos tables que l'on va traiter
    mysql_init(&mysql);//On initialise notre bibliothèque de BDD
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");//On choisie nos options

    //Cette fonction va nous servir à créer un cocktail avec pour chaque cocktail différents ingrédient avec leur propre quantité.

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){//On se connecte à la base de donné et si on réussi on continue le programme.

        printf("| Creation of a cocktail |\n");
        printf("Choose the name of your cocktails\n");
        fflush(stdin);
        fgets(name_cocktail,100,stdin);//Ici l'utilisateur choisi le nom de son cocktail (on fait au préalable un "fflush" sur stdin).
        if(name_cocktail[strlen(name_cocktail)-1]=='\n'){//On met bien le marqueur de fin de chaine à la fin de notre chaîne de charactère.
            name_cocktail[strlen(name_cocktail)-1]='\0';
        }

        strcpy(query,"SELECT * FROM ingredient");//Ici on va envoyer une reqête afin de récupérer toute les lignes de notre table ingrédient.
        mysql_query(&mysql,query);

        result = mysql_use_result(&mysql);
        while((row = mysql_fetch_row(result))){//Et on fait une boucle qui pour chaque ligne incrémente la variable "count_row"--> On obtient donc le nombre d'ingrédient dans notre table.
             count_row++;
        }

        tab_choice_ingredient=malloc(sizeof(char*)*count_row);//Avec "count_row" on construit le tableau dynamique de string qui va contenire l'id de tout nos ingrédient.
        if(tab_choice_ingredient!=NULL){

             for(i=0;i<count_row;i++){
                 tab_choice_ingredient[i]=malloc(sizeof(char)*11);
             }
        }

        printf("Choose an ingredient(max 10)\n");
        do{//C'est dans cette boucle qu'on va afficher la liste des ingrédients et que l'utilisateur va pouvoir les choisir
            count_row=0;
            printf("Choose a number (or 0 if the list is finish)\n");
            printf("(%d/10)\n)",max);
            strcpy(query,"SELECT * FROM ingredient");//On envoie une requête pour récuperer tous les ingrédients avec leur id et leur nom.
            mysql_query(&mysql,query);

            result = mysql_use_result(&mysql);
            while((row = mysql_fetch_row(result))){//On boucle pour chaque ligne

                printf("[%d] [%s]",count_row+1,row[1]);//Pour chaque ligne on affiche le nom de l'ingrédient et "count_row+1" qui correspondra au chiffre que l'utilisateur devra renter pour choisir cet ingrédient
                printf("\n");

                if(check_count_row==0){//C'est ici que notre variable tempon va être utile: on affecte dans notre tableau dynamique les id de nos ingrédient à chaque tour de boucle et la variable "check_count_row" passe à 1 dès le premier tour de boucle ce qui permet de faire l'affectation qu'une seul fois.

                    strcpy(tab_choice_ingredient[count_row],row[0]);

                }

                count_row++;//On incrémente à chaque tour de boucle
            }

            check_count_row=1;

            scanf("%d",&choice);//L'utilisateur séléctionne le numéro correspondant à l'ingrédient.
            if(choice<=count_row && choice>0){//On vérifie que ce numéro est bien dans notre intervalle d'ingrédient dispo
                if(choice!=0){
                    tab_ingredient[max]=choice;//On stock son choix dans un tableau
                    printf("Enter the quantity (in cl)\n");//On lui propose de rentrer la quantité.
                    scanf("%d",&quantity);
                    tab_quantity[max]=quantity;//et on l'a met dans un tableau
                    max++;//Ici on incrémente notre variable max, elle nous sert à savoir combien d'ingrédient on a choisie et à vérifier que l'on dépasse pas le nombre d'ingrédient.
                }
            }

        }while(choice!=0 && max<10 || max==0);//on boucle tant que l'utilisateur ne veut pas sortir, qu'il n'a pas tapé le nombre maximum d'ingrédient ou tant qu'il n'a pas rentré au moins un ingrédient.

        //On va créer le cocktail dans la table "cocktail" en insérant son nom et l'id de l'utilisateur qui l'a créé.
        itoa(id,txt_tmp,10);//Sur cette ligne on convertie l'id de l'utilisateur en string pour le metre dans la requête.
        strcpy(query,"INSERT INTO COCKTAILS (name,id_user) VALUES('");
        strcat(query,name_cocktail);
        strcat(query,"','");
        strcat(query,txt_tmp);
        strcat(query,"')");

        mysql_query(&mysql,query);

        strcpy(query,"SELECT LAST_INSERT_ID() FROM COCKTAILS");//Pour la suite on doit récuperer l'id du cocktail qui vient d'être créé: on le fait avec cette requête.
        mysql_query(&mysql,query);

        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);

        if(row){
            sscanf(row[0],"%d",&id_cocktails);//On place l'id récupéré dans "id_cocktail".
        }

        //C'est ici qu'on va crée la recette de notre cocktail en fonction du nombre d'ingrédient que l'on a créé
        for(i=0;i<max;i++){//On boucle en fonction du nombre d'ingrédient qu'on a rentré
            strcpy(query,"INSERT INTO RECIPE (id_cocktail,id_ingredient,quantity) VALUES('");//Dans une ligne on met l'id du cocktail, l'id de l'ingrédient et la quantité
            itoa(id_cocktails,txt_tmp,10);
            strcat(query,txt_tmp);
            strcat(query,"','");
            strcat(query,tab_choice_ingredient[tab_ingredient[i]-1]);//Pour récupérer l'id de l'ingrédient on le fait avec le tableau contenant nos choix remplie précédement.
            strcat(query,"','");
            itoa(tab_quantity[i],txt_tmp,10);
            strcat(query,txt_tmp);
            strcat(query,"')");

            mysql_query(&mysql,query);
        }

        free(tab_choice_ingredient);//On libère la mémoire prise par notre tableau dynamique.
        mysql_close(&mysql);//On ferme la connection avec notre BDD.

    }else{

        printf("ERROR: An error occurred while connecting to the DB!");

    }


}

void createCocktails_SDL(int id){

    char name_cocktail[100];
    char txt_tmp[10];
    int id_cocktails;
    int tab_ingredient[10];
    int tab_quantity[10];
    char **tab_choice_ingredient;
    int max=0;
    int choice;
    int quantity;
    char query[255];
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    unsigned int i = 0;
    int count_row=0;
    int check_count_row=0;

    //SDL
    int x_mouse;
    int y_mouse;
    int loop=6;
    char gui_txt[255];
    char txt_max[10];
    int y_position_increment;
    char txt_count_row[10];
    int check_input=0;
    char txt_quantity[10];

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        do{

            SDL_WaitEvent(&event);
            if(event.type == SDL_TEXTINPUT){
                strcat(name_cocktail,event.text.text);
            }

            if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                name_cocktail[strlen(name_cocktail)-1]='\0';
            }

            if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                check_input=1;
            }

            SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            font=TTF_OpenFont(txt_font, 35);
            text=TTF_RenderText_Blended(font,"Creation of a cocktail",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=20;
            position.y=50;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            font=TTF_OpenFont(txt_font, 20);
            text=TTF_RenderText_Blended(font,"Choose the name of your cocktails:",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=15;
            position.y=130;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            text=TTF_RenderText_Blended(font,name_cocktail,font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=15;
            position.y=150;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            SDL_RenderPresent(renderer);

        }while(check_input!=1);

        if(name_cocktail[strlen(name_cocktail)-1]=='\n'){
            name_cocktail[strlen(name_cocktail)-1]='\0';
        }

        strcpy(query,"SELECT * FROM ingredient");
        mysql_query(&mysql,query);

        result = mysql_use_result(&mysql);
        while((row = mysql_fetch_row(result))){
             count_row++;
        }

        tab_choice_ingredient=malloc(sizeof(char*)*count_row);
        if(tab_choice_ingredient!=NULL){

             for(i=0;i<count_row;i++){
                 tab_choice_ingredient[i]=malloc(sizeof(char)*11);
             }
        }

        do{
            count_row=0;
            y_position_increment=170;
            choice=-1;
            x_mouse=0;
            y_mouse=0;

            SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            font=TTF_OpenFont(txt_font, 35);
            text=TTF_RenderText_Blended(font,"Creation of a cocktail",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=20;
            position.y=50;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            font=TTF_OpenFont(txt_font, 20);
            strcpy(gui_txt,"Choose an ingredient (");
            itoa(max,txt_max,10);
            strcat(gui_txt,txt_max);
            strcat(gui_txt,"/10)");
            text=TTF_RenderText_Blended(font,gui_txt,font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=15;
            position.y=130;
            SDL_RenderCopy(renderer, texture, NULL, &position);
            strcpy(query,"SELECT * FROM ingredient");
            mysql_query(&mysql,query);

            result = mysql_use_result(&mysql);
            while((row = mysql_fetch_row(result))){

                if(count_row<loop && count_row>=loop-6){//Ce qui va changer par rapport à l'affichage en ligne de commande c'est qu'on va afficher 6 par 6 et naviguer sur l'interface grâce à la variable loop
                    strcpy(gui_txt,"[");
                    itoa(count_row+1,txt_count_row,10);
                    strcat(gui_txt,txt_count_row);
                    strcat(gui_txt,"] ");
                    strcat(gui_txt,row[1]);
                    text=TTF_RenderText_Blended(font,gui_txt,font_color);
                    position.x=0;
                    position.y=0;
                    texture= SDL_CreateTextureFromSurface(renderer,text);
                    SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                    position.x=20;
                    position.y=y_position_increment;
                    SDL_RenderCopy(renderer, texture, NULL, &position);
                    SDL_RenderPresent(renderer);

                    y_position_increment+=70;
                }

                if(check_count_row==0){
                    strcpy(tab_choice_ingredient[count_row],row[0]);

                }

                count_row++;
            }

            check_count_row=1;

            text=TTF_RenderText_Blended(font,"Finish",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 120, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
            position.x=20;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=20;
            position.y=610;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            SDL_RenderPresent(renderer);

            do{

                SDL_WaitEvent(&event);

                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                    SDL_GetMouseState(&x_mouse,&y_mouse);
                }

                //C'est avec les deux events suivants qu'on navigue dans la liste en pressant la touche "flêche du haut" ou "flêche du bas"

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_UP){
                    choice=-2;
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_DOWN){
                    choice=-3;
                }

                if(x_mouse>=20 && x_mouse<=390 && y_mouse>=170 && y_mouse<=198){
                    choice=6;
                }

                if(x_mouse>=60 && x_mouse<=340 && y_mouse>=240 && y_mouse<=268){
                    choice=5;
                }

                if(x_mouse>=20 && x_mouse<=340 && y_mouse>=310 && y_mouse<=338){
                    choice=4;
                }

                if(x_mouse>=20 && x_mouse<=340 && y_mouse>=380 && y_mouse<=408){
                    choice=3;
                }

                if(x_mouse>=20 && x_mouse<=340 && y_mouse>=450 && y_mouse<=478){
                    choice=2;
                }

                if(x_mouse>=20 && x_mouse<=340 && y_mouse>=520 && y_mouse<=548){
                    choice=1;
                }

                if(x_mouse>=20 && x_mouse<=140 && y_mouse>=610 && y_mouse<=680){
                    choice=0;
                }

            }while(choice==-1);

            if(choice<=count_row && choice>0){
                if(choice!=0){
                    tab_ingredient[max]=loop-choice+1;
                    check_input=0;
                    do{

                        SDL_WaitEvent(&event);
                        if(event.type == SDL_TEXTINPUT){
                            strcat(txt_quantity,event.text.text);
                        }

                        if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                            txt_quantity[strlen(txt_quantity)-1]='\0';
                        }

                        if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                            check_input=1;
                        }

                        SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                        SDL_RenderClear(renderer);
                        SDL_RenderPresent(renderer);

                        font=TTF_OpenFont(txt_font, 35);
                        text=TTF_RenderText_Blended(font,"Creation of a cocktail",font_color);
                        position.x=0;
                        position.y=0;
                        texture= SDL_CreateTextureFromSurface(renderer,text);
                        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                        position.x=20;
                        position.y=50;
                        SDL_RenderCopy(renderer, texture, NULL, &position);

                        font=TTF_OpenFont(txt_font, 20);
                        text=TTF_RenderText_Blended(font,"Enter the quantity (cl):",font_color);
                        position.x=0;
                        position.y=0;
                        texture= SDL_CreateTextureFromSurface(renderer,text);
                        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                        position.x=15;
                        position.y=130;
                        SDL_RenderCopy(renderer, texture, NULL, &position);

                        text=TTF_RenderText_Blended(font,txt_quantity,font_color);
                        position.x=0;
                        position.y=0;
                        texture= SDL_CreateTextureFromSurface(renderer,text);
                        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                        position.x=15;
                        position.y=150;
                        SDL_RenderCopy(renderer, texture, NULL, &position);

                        SDL_RenderPresent(renderer);

                    }while(check_input!=1);
                    quantity=atoi(txt_quantity);
                    tab_quantity[max]=quantity;
                    max++;
                }
            }


            //Lorsque l'on veut aller vers le haut au décrémente loop pour afficher les élèment plus haut
            if(choice==-2){
                if(loop>6){
                    loop-=1;
                }
            }

            //Lorsque l'on veut aller vers le bas au incrémente loop pour afficher les élèment plus bas
            if(choice==-3){

                if(loop+1<=count_row){
                    loop+=1;
                }
            }

        }while(choice!=0 && max<10 || max==0);

        itoa(id,txt_tmp,10);
        strcpy(query,"INSERT INTO COCKTAILS (name,id_user) VALUES('");
        strcat(query,name_cocktail);
        strcat(query,"','");
        strcat(query,txt_tmp);
        strcat(query,"')");

        mysql_query(&mysql,query);

        strcpy(query,"SELECT LAST_INSERT_ID() FROM COCKTAILS");
        mysql_query(&mysql,query);

        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);

        if(row){
            sscanf(row[0],"%d",&id_cocktails);
        }

        for(i=0;i<max;i++){
            strcpy(query,"INSERT INTO RECIPE (id_cocktail,id_ingredient,quantity) VALUES('");
            itoa(id_cocktails,txt_tmp,10);
            strcat(query,txt_tmp);
            strcat(query,"','");
            strcat(query,tab_choice_ingredient[tab_ingredient[i]-1]);
            strcat(query,"','");
            itoa(tab_quantity[i],txt_tmp,10);
            strcat(query,txt_tmp);
            strcat(query,"')");
            mysql_query(&mysql,query);
        }

        free(tab_choice_ingredient);
        mysql_close(&mysql);

    }


}

void listCocktails(int id){

    char query[255];//Va contenir nos diiférente requête SQL
    char **tab_coktails;//Un double pointeur de char qui va devenir un tableau dynamique contenant l'id de chaque cocktail
    int count_row=0;//Une variable tempon qui nous sert à savoir combien de ligne on traitre
    unsigned int i = 0;
    int choice;//C'est avec cette variable que l'utilisateur va séléctionner un cockatil on décider de revenir au menu des cocktails.
    int choice2;//Avec cette variable l'utilisateur va pouvoir revenir à la liste des cocktails
    int check=0;//Variable tempon
    //char id_cocktail[10];

    MYSQL mysql;
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    //Dans cette fonction on va pouvoir afficher la liste des cocktails créé et les séléctionner pour savoir les ingrédients nécéssaire à leur conception + la quantité.

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){//On se connecte à la BDD

       printf("| List of the cocktail |\n");

       strcpy(query,"SELECT * FROM cocktails");//Ici on va envoyer une requète qui va séléctionner toute les lignes de notre table coktails.
       mysql_query(&mysql,query);//On envoi la requête

       result = mysql_use_result(&mysql);//On récupère le jeu de résultat
       while((row = mysql_fetch_row(result))){//Avec cette requête on va boucler pour chaque ligne récupéré et on va incrémenté un couteur pour connaître le nombre de cocktail dans notre table.
            count_row++;
       }

       tab_coktails=malloc(sizeof(char*)*count_row);//Avec notre conteur on créer un tableau dynamique de string qui va contenir l'id de nos cocktail
       if(tab_coktails!=NULL){

            for(i=0;i<count_row;i++){
                tab_coktails[i]=malloc(sizeof(char)*11);
            }
       }

       do{//Dans cette boucle on va afficher la liste des cocktail et les utilisateurs qui les ont créé
            count_row=0;

            strcpy(query,"SELECT cocktails.id,name,user.pseudo FROM cocktails INNER JOIN user ON cocktails.id_user = user.id ORDER BY(name)");//Avec cette requête on récupère l'id du cocktail, son nom, et l'utilisateur qui l'a créé.
            mysql_query(&mysql,query);

            result = mysql_use_result(&mysql);
            while((row = mysql_fetch_row(result))){

                printf("[%d] |%s| created by %s",count_row+1,row[1],row[2]);//Pour chaque ligne on affiche le nom du cocktail , l'utilisateur qui l' a créé et "count_row+1" qui correspondra au chiffre que l'utilisateur devra renter pour voir les détails
                printf("\n");
                if(check==0){//C'est ici que notre variable tempon va être utile: on affecte dans notre tableau dynamique les id de nos cocktails à chaque tour de boucle et la variable "check" passe à 1 dès le premier tour de boucle ce qui permet de faire l'affectation qu'une seul fois.
                    strcpy(tab_coktails[count_row],row[0]);

                }
                count_row++;
            }

            check=1;

            printf("Choose a cocktails (enter 0 to return to the coktails menu)\n");//On propose à l'utilisateur de choisir un cocktail ou de sortir
            scanf("%d",&choice);

            if(choice>0 && choice<=count_row){//On vérifie que l'utilisateur à rentré un numéro valide
                //strcpy(id_cocktail,tab_coktails[choice-1]);
                printf("Recipe\n");
                strcpy(query,"SELECT ingredient.name,quantity FROM recipe INNER JOIN cocktails ON recipe.id_cocktail = '");//Avec cette requête on va séléctionner les ingrédient et la quantité correspondante du cocktail que l'on a séléctionné.
                strcat(query,tab_coktails[choice-1]);//On fait cette requête en fonction du cockatil que l'on a séléctionner, on va donc chercher son id dans le tableau "tab_cocktail" avec comme indice notre choix-1.
                strcat(query,"' and cocktails.id='");
                strcat(query,tab_coktails[choice-1]);
                strcat(query,"' INNER JOIN ingredient ON recipe.id_ingredient = ingredient.id");
                 mysql_query(&mysql,query);

                result = mysql_use_result(&mysql);
                while((row = mysql_fetch_row(result))){//On affiche par la suite le résultat

                    printf("|%s| quantity: %s cl",row[0],row[1]);
                    printf("\n");
                }

                do{//Ici on propose à l'utilisateur de revenir à la liste des cocktails.
                    printf("Return to cocktails list ?(enter 0)");
                    scanf("%d",&choice2);
                }while(choice2!=0);
            }

        }while(choice!=0);//On boucle tant que l'utilisateur ne souhaite pas revenir à la liste des cocktails

       free(tab_coktails);//On libère la mémoire prise par notre tableau dynamique
       mysql_close(&mysql);//On met fin à la connection avec la BDD

    }else{

        printf("ERROR: An error occurred while connecting to the DB!");

    }

}

void listCocktails_SDL(int id){

    char query[255];
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char **tab_coktails;
    int count_row=0;
    unsigned int i = 0;
    int choice;
    int choice2;
    int check=0;
    //char id_cocktail[10];

    //SDL
    char txt_count_row[10];
    char gui_txt[255];
    int y_position_increment;
    int x_mouse;
    int y_mouse;
    int loop=6;

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

       strcpy(query,"SELECT * FROM cocktails");
       mysql_query(&mysql,query);

       result = mysql_use_result(&mysql);
       while((row = mysql_fetch_row(result))){
            count_row++;
       }

       tab_coktails=malloc(sizeof(char*)*count_row);
       if(tab_coktails!=NULL){

            for(i=0;i<count_row;i++){
                tab_coktails[i]=malloc(sizeof(char)*11);
            }
       }

       do{
            count_row=0;
            choice=-1;
            x_mouse=0;
            y_mouse=0;
            y_position_increment=170;

            SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            font=TTF_OpenFont(txt_font, 45);
            text=TTF_RenderText_Blended(font,"List Cocktail",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=75;
            position.y=50;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            SDL_RenderPresent(renderer);

            font=TTF_OpenFont(txt_font, 20);

            strcpy(query,"SELECT cocktails.id,name,user.pseudo FROM cocktails INNER JOIN user ON cocktails.id_user = user.id ORDER BY(name)");
            mysql_query(&mysql,query);

            result = mysql_use_result(&mysql);
            while((row = mysql_fetch_row(result))){

                if(count_row<loop && count_row>=loop-6){
                    itoa(count_row+1,txt_count_row,10);
                    strcpy(gui_txt,"[");
                    strcat(gui_txt,txt_count_row);
                    strcat(gui_txt,"] ");
                    strcat(gui_txt,row[1]);
                    strcat(gui_txt," created by ");
                    strcat(gui_txt,row[2]);

                    text=TTF_RenderText_Blended(font,gui_txt,font_color);
                    position.x=0;
                    position.y=0;
                    texture= SDL_CreateTextureFromSurface(renderer,text);
                    SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                    position.x=20;
                    position.y=y_position_increment;
                    SDL_RenderCopy(renderer, texture, NULL, &position);

                    SDL_RenderPresent(renderer);

                    y_position_increment+=70;
                }

                if(check==0){
                    strcpy(tab_coktails[count_row],row[0]);

                }
                count_row++;
            }

            check=1;

            text=TTF_RenderText_Blended(font,"Return",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 120, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
            position.x=20;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=20;
            position.y=610;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            SDL_RenderPresent(renderer);

            do{

                SDL_WaitEvent(&event);

                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                    SDL_GetMouseState(&x_mouse,&y_mouse);
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_UP){
                    choice=-2;
                }

                if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_DOWN){
                    choice=-3;
                }

                if(x_mouse>=20 && x_mouse<=390 && y_mouse>=170 && y_mouse<=198){
                    choice=6;
                }

                if(x_mouse>=60 && x_mouse<=340 && y_mouse>=240 && y_mouse<=268){
                    choice=5;
                }

                if(x_mouse>=20 && x_mouse<=340 && y_mouse>=310 && y_mouse<=338){
                    choice=4;
                }

                if(x_mouse>=20 && x_mouse<=340 && y_mouse>=380 && y_mouse<=408){
                    choice=3;
                }

                if(x_mouse>=20 && x_mouse<=340 && y_mouse>=450 && y_mouse<=478){
                    choice=2;
                }

                if(x_mouse>=20 && x_mouse<=340 && y_mouse>=520 && y_mouse<=548){
                    choice=1;
                }

                if(x_mouse>=20 && x_mouse<=140 && y_mouse>=610 && y_mouse<=680){
                    choice=0;
                }

            }while(choice==-1);

            if(choice>0 && choice<=count_row){

                y_position_increment=170;
                choice2=1;

                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);

                font=TTF_OpenFont(txt_font, 45);
                text=TTF_RenderText_Blended(font,"List Cocktail",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=75;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont(txt_font, 20);

                //strcpy(id_cocktail,tab_coktails[loop-choice]);
                //printf("Recipe\n");

                strcpy(query,"SELECT ingredient.name,quantity FROM recipe INNER JOIN cocktails ON recipe.id_cocktail = '");
                strcat(query,tab_coktails[loop-choice]);
                strcat(query,"' and cocktails.id='");
                strcat(query,tab_coktails[loop-choice]);
                strcat(query,"' INNER JOIN ingredient ON recipe.id_ingredient = ingredient.id");
                 mysql_query(&mysql,query);

                result = mysql_use_result(&mysql);
                while((row = mysql_fetch_row(result))){

                    strcpy(gui_txt,"|");
                    strcat(gui_txt,row[0]);
                    strcat(gui_txt,"| quantity: ");
                    strcat(gui_txt,row[1]);
                    strcat(gui_txt," cl");

                    text=TTF_RenderText_Blended(font,gui_txt,font_color);
                    position.x=0;
                    position.y=0;
                    texture= SDL_CreateTextureFromSurface(renderer,text);
                    SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                    position.x=20;
                    position.y=y_position_increment;
                    SDL_RenderCopy(renderer, texture, NULL, &position);

                    SDL_RenderPresent(renderer);

                    y_position_increment+=70;
                }

                text=TTF_RenderText_Blended(font,"Return",font_color);
                surface=NULL;
                surface = SDL_CreateRGBSurface(0, 120, 70, 32, 0, 0, 0, 0);
                SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
                position.x=20;
                position.y=15;
                SDL_BlitSurface(text,NULL,surface,&position);
                texture= SDL_CreateTextureFromSurface(renderer,surface);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=20;
                position.y=610;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                SDL_RenderPresent(renderer);

                do{
                    SDL_WaitEvent(&event);

                    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                        SDL_GetMouseState(&x_mouse,&y_mouse);
                    }

                    if(x_mouse>=20 && x_mouse<=140 && y_mouse>=610 && y_mouse<=680){
                    choice2=0;
                    }

                }while(choice2!=0);
            }

            if(choice==-2){
                if(loop>6){
                    loop-=1;
                }
            }

            if(choice==-3){

                if(loop+1<=count_row){
                    loop+=1;
                }
            }

        }while(choice!=0);

       free(tab_coktails);
       mysql_close(&mysql);

    }else{

        printf("ERROR: An error occurred while connecting to the DB!");

    }

}

void game(){


    int choice;
    char **tab_id_game;
    char query[255];
    int count_row=0;
    unsigned int i;
    unsigned int y;
    unsigned int number;
    char txt_number[5];
    char question[500];
    int id_used[15];
    int check=0;

    //SQL
    MYSQL mysql;
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    srand(time(NULL));

    //Cette fonction va lancer un jeu: le "je n'ai jamais". Il permet de lancer 15 question aléatoire sue le thème du jeu.

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){
        printf("--GAME--\n");

        do{//Dans cette boucle on demande à l'utilisateur si il veut commencer le jeu ou si il veut revenir au menu
            printf("1: Start a game\n2: Return to the menu\n");
            scanf("%d",&choice);

            if(choice==1){
                        count_row=0;
                        strcpy(query,"SELECT id FROM game");//On fait une requête pour récupérer le nombre de ligne dans la table game pour pouvoir initialiser un tableau dynamique de string.
                        mysql_query(&mysql,query);

                        result = mysql_use_result(&mysql);
                        while((row = mysql_fetch_row(result))){
                            count_row++;
                        }

                        tab_id_game=malloc(sizeof(char*)*count_row);
                        if(tab_id_game!=NULL){

                            for(i=0;i<count_row;i++){
                                tab_id_game[i]=malloc(sizeof(char)*11);
                            }
                        }

                        mysql_query(&mysql,query);

                        i=0;
                        result = mysql_use_result(&mysql);
                        while((row = mysql_fetch_row(result))){
                           strcpy(tab_id_game[i],row[0]);
                           i++;

                        }

                        for(i=0;i<15;i++){//On boucle pour afficher 15 question

                            do{//Avec cette boucle on s'assure que les questions sont posé de manière aléatoire et qu'on ne répète jamais deux fois la même question
                                check=1;
                                number=rand()%16;

                                if(i!=0){
                                    for(y=0;y<i;y++){
                                        if(id_used[y]==number){
                                            check=0;
                                        }
                                    }

                                    if(check==1){
                                        id_used[i]=number;
                                    }

                                }else{
                                    id_used[i]=number;
                                    check=1;
                                }

                            }while(check!=1);

                            //On récupère la question
                            strcpy(txt_number,tab_id_game[number]);
                            strcpy(query,"SELECT question FROM game WHERE id='");
                            strcat(query,txt_number);
                            strcat(query,"'");

                            mysql_query(&mysql,query);
                            result = mysql_use_result(&mysql);
                            while((row = mysql_fetch_row(result))){

                                strcpy(question,row[0]);
                            }

                            do{//et on l'affiche.
                                printf("%d: %s\n",i+1,question);
                                printf("1: Next question\n");

                                scanf("%d",&choice);
                            }while(choice!=1);

                            choice=0;
                        }





                        free(tab_id_game);


            }
        }while(choice!=2);

        mysql_close(&mysql);

    }else{

        printf("ERROR: An error occurred while connecting to the DB!");

    }

}

void game_SDL(){


    int choice;
    char **tab_id_game;
    char query[255];
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    int count_row=0;
    unsigned int i;
    unsigned int y;
    unsigned int number;
    char txt_number[5];
    char question[500];
    int id_used[15];
    int check=0;

    //SDL
    int x_mouse;
    int y_mouse;
    int count_space;

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    srand(time(NULL));

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        do{

            choice=-1;
            x_mouse=0;
            y_mouse=0;

            SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            font=TTF_OpenFont(txt_font, 45);
            text=TTF_RenderText_Blended(font,"GAME",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=130;
            position.y=50;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            font=TTF_OpenFont(txt_font, 30);
            text=TTF_RenderText_Blended(font,"Start the game",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 280, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
            position.x=20;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=60;
            position.y=170;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            text=TTF_RenderText_Blended(font,"Menu",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 120, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
            position.x=20;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=20;
            position.y=610;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            SDL_RenderPresent(renderer);

            do{

                SDL_WaitEvent(&event);

                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                    SDL_GetMouseState(&x_mouse,&y_mouse);
                }

                if(x_mouse>=60 && x_mouse<=340 && y_mouse>=170 && y_mouse<=240){
                    choice=1;
                }

                if(x_mouse>=20 && x_mouse<=140 && y_mouse>=610 && y_mouse<=680){
                    choice=2;
                }

            }while(choice==-1);

            if(choice==1){
                        count_row=0;
                        strcpy(query,"SELECT id FROM game");
                        mysql_query(&mysql,query);

                        result = mysql_use_result(&mysql);
                        while((row = mysql_fetch_row(result))){
                            count_row++;
                        }

                        tab_id_game=malloc(sizeof(char*)*count_row);
                        if(tab_id_game!=NULL){

                            for(i=0;i<count_row;i++){
                                tab_id_game[i]=malloc(sizeof(char)*11);
                            }
                        }

                        mysql_query(&mysql,query);

                        i=0;
                        result = mysql_use_result(&mysql);
                        while((row = mysql_fetch_row(result))){
                           strcpy(tab_id_game[i],row[0]);
                           i++;

                        }

                        for(i=0;i<15;i++){


                            do{
                                check=1;
                                number=rand()%16;

                                if(i!=0){
                                    for(y=0;y<i;y++){
                                        if(id_used[y]==number){
                                            check=0;
                                        }
                                    }

                                    if(check==1){
                                        id_used[i]=number;
                                    }

                                }else{
                                    id_used[i]=number;
                                    check=1;
                                }

                            }while(check!=1);

                            strcpy(txt_number,tab_id_game[number]);
                            strcpy(query,"SELECT question FROM game WHERE id='");
                            strcat(query,txt_number);
                            strcat(query,"'");

                            mysql_query(&mysql,query);
                            result = mysql_use_result(&mysql);
                            while((row = mysql_fetch_row(result))){

                                strcpy(question,row[0]);
                            }

                            count_space=0;

                            if(strlen(question)>35){
                                for(y=0;y<strlen(question);y++){
                                    if(question[y]==' '){
                                        if(count_space==4){
                                           question[y]='\n';
                                           count_space=0;
                                        }else{
                                            count_space++;
                                        }
                                    }
                                }
                                //printf("%s\n\n",question);
                            }

                            do{

                                choice=-1;
                                x_mouse=0;
                                y_mouse=0;

                                SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
                                SDL_RenderClear(renderer);
                                SDL_RenderPresent(renderer);

                                font=TTF_OpenFont(txt_font, 45);
                                text=TTF_RenderText_Blended(font,"GAME",font_color);
                                position.x=0;
                                position.y=0;
                                texture= SDL_CreateTextureFromSurface(renderer,text);
                                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                                position.x=130;
                                position.y=50;
                                SDL_RenderCopy(renderer, texture, NULL, &position);

                                font=TTF_OpenFont(txt_font, 14);
                                text=TTF_RenderText_Blended(font,question,font_color);
                                position.x=0;
                                position.y=0;
                                texture= SDL_CreateTextureFromSurface(renderer,text);
                                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                                position.x=60;
                                position.y=170;
                                SDL_RenderCopy(renderer, texture, NULL, &position);

                                SDL_RenderPresent(renderer);

                                do{

                                    SDL_WaitEvent(&event);

                                    if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN){
                                        choice=1;
                                    }

                                }while(choice==-1);
                            }while(choice!=1);

                            choice=0;
                        }

                        free(tab_id_game);

            }
        }while(choice!=2);

        mysql_close(&mysql);

    }else{

        printf("ERROR: An error occurred while connecting to the DB!");

    }

}

int verifConfTxt(){

    FILE *fp;
    char conf_txt[255];
    int check_verif_conf=1;

    fp=fopen("conf.txt","rb");
    if(fp!=NULL){

        //Dans la suite d'instruction suivante on va récupèrer les différentes lignes de notre fichier de configuration et faire des comparaisons de chaines pour à la fois vérifier le bon format du fichier mais aussi quelle paramètre on doit mettre pour le programme
        //Option interface graphique/ligne de commande
        fgets(conf_txt,255,fp);
        if((strncmp(conf_txt,"app_mod:",8))==0){
            if((strncmp(conf_txt+8,"GUI",3))==0){
                app_mod=1;
            }else if((strncmp(conf_txt+8,"command_line",12))==0){
                app_mod=2;
            }else{
                app_mod=1;
            }
        }else{
            printf("#1#");
            check_verif_conf=0;
        }

        //Option police d'écriture
        fgets(conf_txt,255,fp);
        if((strncmp(conf_txt,"font:",5))==0 && check_verif_conf!=0){
            if((strncmp(conf_txt+5,"poppins-Regular.ttf",19))==0){
                strcpy(txt_font,"poppins-Regular.ttf");
            }else if((strncmp(conf_txt+5,"redressed-Regular.ttf",21))==0){
                strcpy(txt_font,"redressed-Regular.ttf");
            }else{
                strcpy(txt_font,"poppins-Regular.ttf");
            }
        }else{
            printf("#2#");
            check_verif_conf=0;
        }

        //Option couleur normal/inverser
        fgets(conf_txt,255,fp);
        if((strncmp(conf_txt,"color:",6))==0 && check_verif_conf!=0){
            if((strncmp(conf_txt+6,"normal",6))==0){

                background.r=213;
                background.g=115;
                background.b=51;

                r_color=129;
                g_color=120;
                b_color=115;

            }else if((strncmp(conf_txt+6,"inversed",8))==0){

                background.r=129;
                background.g=120;
                background.b=115;

                r_color=213;
                g_color=115;
                b_color=51;

            }else{

               background.r=213;
                background.g=115;
                background.b=51;

                r_color=129;
                g_color=120;
                b_color=115;

            }

        }else{
            printf("#3#");
            check_verif_conf=0;
        }

        //Option renderer avec le CPU/GPU
        fgets(conf_txt,255,fp);
        if((strncmp(conf_txt,"renderer:",9))==0 && check_verif_conf!=0){
            if((strncmp(conf_txt+9,"software",8))==0){
                fclose(fp);
                return 1;
            }else if((strncmp(conf_txt+9,"accelerate",10))==0){
                return 2;
            }else{
                fclose(fp);
                return 1;
            }
        }else{
            printf("#4#");
            fclose(fp);
            check_verif_conf=0;
        }

        if(check_verif_conf==0){
            printf("ERROR: the conf file format is wrong");
            fclose(fp);
            return 0;
        }


    }else{
        printf("ERROR: Conf file does not exist");
        fclose(fp);
        return 0;
    }

}

void menu(){

    int choice;//Cette variable va nous servir à pouvoir naviger dans les différents menu de l'application.
    int id;//Cette variable va contenir l'id de l'utilisateur une fois qu'il se sera connécté ou qu'il aura créé son compte.
    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    //Cette fonction nous sert de menu principal c'est ici qu'on va pouvoir accéder au différentes fonctionnalité de l'appli

    do{
        printf("1: Sign in\n2: Sign up\n");//On demande à l'utilisateur de choisir entre se connecter et s'inscrire.
        scanf("%d",&choice);
    }while(choice!=1 && choice!=2);

    if(choice==1){//En fonction du choix on se dirige vers la fonction SignIn ou SignUp
        id=SignIn();
    }else if(choice==2){
        id=SignUp();
    }

    do{
        printf("--MENU--\n1: Cocktails\n2: Bar\n3: Game\n4 Setting\n5:EXIT\n");//On propose ici de choisr entre 5 choix: la fonctionalité "cocktail",la fonctionalité "game",la fonctionalité "bar",rentrer dans les paramètre ou sortir de l'appli.Il rentre au clavier le numéro indiqué pour chaque fonction
        scanf("%d",&choice);

        if(choice==1){
            cocktails(id);
        }

        if(choice==2){
            if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0))
                selectMenu(mysql);
            else
                printf("ERROR: An error occurred while connecting to the DB!");
        }

        if(choice==3){
            game();
        }

        if(choice==4){
            setting();
        }

    }while(choice!=5);//On répète cette boucle tant que le joueur n'a pas décidé de sortir de l'appli.



}

void menu_SDL(){
    int choice;
    int id;
    int x_mouse;
    int y_mouse;

        SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        font=TTF_OpenFont(txt_font, 30);
        text=TTF_RenderText_Blended(font,"Sign In",font_color);
        surface = SDL_CreateRGBSurface(0, 170, 70, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
        position.x=37;
        position.y=15;
        SDL_BlitSurface(text,NULL,surface,&position);
        texture= SDL_CreateTextureFromSurface(renderer,surface);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=110;
        position.y=150;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        text=TTF_RenderText_Blended(font,"Sign Up",font_color);
        surface=NULL;
        surface = SDL_CreateRGBSurface(0, 170, 70, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
        position.x=35;
        position.y=15;
        SDL_BlitSurface(text,NULL,surface,&position);
        texture= SDL_CreateTextureFromSurface(renderer,surface);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=110;
        position.y=230;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        SDL_RenderPresent(renderer);

        do{

            SDL_WaitEvent(&event);

            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                SDL_GetMouseState(&x_mouse,&y_mouse);
            }

            if(x_mouse>=110 && x_mouse<=280 && y_mouse>=150 && y_mouse<=220){
                choice=1;
            }

            if(x_mouse>=110 && x_mouse<=280 && y_mouse>=230 && y_mouse<=300){
                choice=2;
            }
        }while(choice!=1 && choice!=2);

        if(choice==1){
            id=SignIn_SDL();
        }else if(choice==2){
            id=SignUp_SDL();
        }

        do{

            x_mouse=0;
            y_mouse=0;
            choice=-1;

            SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            font=TTF_OpenFont(txt_font, 50);
            text=TTF_RenderText_Blended(font,"MENU",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=125;
            position.y=50;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            font=TTF_OpenFont(txt_font, 30);
            text=TTF_RenderText_Blended(font,"Cocktails",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 170, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
            position.x=20;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=110;
            position.y=170;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            text=TTF_RenderText_Blended(font,"Game",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 170, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
            position.x=40;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=110;
            position.y=260;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            text=TTF_RenderText_Blended(font,"SETTING",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 140, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
            position.x=15;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=240;
            position.y=610;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            text=TTF_RenderText_Blended(font,"Exit",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 90, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
            position.x=20;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=20;
            position.y=610;
            SDL_RenderCopy(renderer, texture, NULL, &position);


            SDL_RenderPresent(renderer);

            do{
                SDL_WaitEvent(&event);

                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                    SDL_GetMouseState(&x_mouse,&y_mouse);
                }

                if(x_mouse>=110 && x_mouse<=280 && y_mouse>=170 && y_mouse<=240){
                    choice=1;
                    x_mouse=0;
                    y_mouse=0;
                }

                if(x_mouse>=110 && x_mouse<=280 && y_mouse>=260 && y_mouse<=330){
                    choice=3;
                    x_mouse=0;
                    y_mouse=0;
                }

                if(x_mouse>=240 && x_mouse<=380 && y_mouse>=610 && y_mouse<=680){
                    choice=4;
                    x_mouse=0;
                    y_mouse=0;
                }

                if(x_mouse>=20 && x_mouse<=90 && y_mouse>=610 && y_mouse<=680){
                    choice=5;
                    x_mouse=0;
                    y_mouse=0;
                }


            }while(choice==-1);

            if(choice==1){
                cocktails_SDL(id);
            }

            if(choice==3){
                game_SDL();
            }

            if(choice==4){
                setting_SDL();
            }

        }while(choice!=5);

}

void setting(){

    int choice;
    FILE *fp;
    char app_mod_conf[100];
    char font_conf[100];
    char color_conf[100];
    char renderer_conf[100];
    int change=0;
    char final_conf[100];

    //Dans cette fonction on va pouvoir changer les paramètre dans notre fichier de configuration.

    printf("--SETTING--\n");

    fp=fopen("conf.txt","rb");
    if(fp!=NULL){
        //Dans la suite d'instruction suivante on va récupérer le paramètre texte dans notre fichier pour chaque option et on les place dans des variables correspondante

        //mode d'affichage
        fgets(app_mod_conf,100,fp);
        strcpy(app_mod_conf,app_mod_conf+8);
        if(app_mod_conf[strlen(app_mod_conf)-1]=='\n'){
               app_mod_conf[strlen(app_mod_conf)-1]='\0';
           }

        //police d'écriture
        fgets(font_conf,100,fp);
        strcpy(font_conf,font_conf+5);
        if(font_conf[strlen(font_conf)-1]=='\n'){
               font_conf[strlen(font_conf)-1]='\0';
            }

        //Couleur
        fgets(color_conf,100,fp);
        strcpy(color_conf,color_conf+6);
        if(color_conf[strlen(color_conf)-1]=='\n'){
               color_conf[strlen(color_conf)-1]='\0';
           }

        //mode renderer
        fgets(renderer_conf,100,fp);
        strcpy(renderer_conf,renderer_conf+9);
        if(renderer_conf[strlen(renderer_conf)-1]=='\n'){
               renderer_conf[strlen(renderer_conf)-1]='\0';
           }
    }else{
        printf("ERROR: The file can't be open");
    }

    fclose(fp);

    do{
        //On affiche les paramètre actuelle et si l'utilisateur veut changer un paramètre il entre le numéro correspondant.
        printf("Select a number to change the option\n1: %s\n2: %s\n3: %s\n4: %s\n5: Menu\n",app_mod_conf,font_conf,color_conf,renderer_conf);
        scanf("%d",&choice);

        if(choice==1){
            //Ici en fonction du numéro choisie on change le paramètre

            change=1;
            if((strcmp(app_mod_conf,"GUI"))==0){
                strcpy(app_mod_conf,"command_line");
            }else{
                strcpy(app_mod_conf,"GUI");
            }
        }

        if(choice==2){
            change=1;
            if((strcmp(font_conf,"poppins-Regular.ttf"))==0){
                strcpy(font_conf,"redressed-Regular.ttf");
            }else{
                strcpy(font_conf,"poppins-Regular.ttf");
            }
        }

        if(choice==3){
            change=1;
            if((strcmp(color_conf,"normal"))==0){
                strcpy(color_conf,"inversed");
            }else{
                strcpy(color_conf,"normal");
            }
        }

        if(choice==4){
            change=1;
            if((strcmp(renderer_conf,"software"))==0){
                strcpy(renderer_conf,"accelerate");
            }else{
                strcpy(renderer_conf,"software");
            }
        }

    }while(choice!=5);

    //à la fin du programme si un changement à été effectuer on réécrie le fichier avec les changement
    if(change==1){
        fp=fopen("conf.txt","wb");
        if(fp!=NULL){
            strcpy(final_conf,"app_mod:");
            strcat(final_conf,app_mod_conf);
            strcat(final_conf,"\nfont:");
            strcat(final_conf,font_conf);
            strcat(final_conf,"\ncolor:");
            strcat(final_conf,color_conf);
            strcat(final_conf,"\nrenderer:");
            strcat(final_conf,renderer_conf);

            fwrite(final_conf,strlen(final_conf),1,fp);
        }

        fclose(fp);

    }else{
        printf("ERROR: The file can't be open");
    }



}

void setting_SDL(){

    int choice;
    FILE *fp;
    char app_mod_conf[100];
    char font_conf[100];
    char color_conf[100];
    char renderer_conf[100];
    int change=0;
    char final_conf[100];

    //SDL
    int x_mouse;
    int y_mouse;

    //printf("--SETTING--\n");

    fp=fopen("conf.txt","rb");
    if(fp!=NULL){
        fgets(app_mod_conf,100,fp);
        strcpy(app_mod_conf,app_mod_conf+8);
        if(app_mod_conf[strlen(app_mod_conf)-1]=='\n'){
               app_mod_conf[strlen(app_mod_conf)-1]='\0';
           }

        fgets(font_conf,100,fp);
        strcpy(font_conf,font_conf+5);
        if(font_conf[strlen(font_conf)-1]=='\n'){
               font_conf[strlen(font_conf)-1]='\0';
            }

        fgets(color_conf,100,fp);
        strcpy(color_conf,color_conf+6);
        if(color_conf[strlen(color_conf)-1]=='\n'){
               color_conf[strlen(color_conf)-1]='\0';
           }

        fgets(renderer_conf,100,fp);
        strcpy(renderer_conf,renderer_conf+9);
        if(renderer_conf[strlen(renderer_conf)-1]=='\n'){
               renderer_conf[strlen(renderer_conf)-1]='\0';
           }
    }else{
        printf("ERROR: The file can't be open");
    }

    fclose(fp);

    do{
        //printf("Select a number to change the option\n1: %s\n2: %s\n3: %s\n4: %s\n5: Menu\n",app_mod_conf,font_conf,color_conf,renderer_conf);
        //scanf("%d",&choice);

        x_mouse=0;
        y_mouse=0;
        choice=-1;

        SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        font=TTF_OpenFont(txt_font, 45);
        text=TTF_RenderText_Blended(font,"SETTING",font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=75;
        position.y=50;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        font=TTF_OpenFont(txt_font, 20);
        text=TTF_RenderText_Blended(font,"Select an option to change it:",font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=20;
        position.y=170;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        font=TTF_OpenFont(txt_font, 20);
        text=TTF_RenderText_Blended(font,app_mod_conf,font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=20;
        position.y=240;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        font=TTF_OpenFont(txt_font, 20);
        text=TTF_RenderText_Blended(font,font_conf,font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=20;
        position.y=310;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        font=TTF_OpenFont(txt_font, 20);
        text=TTF_RenderText_Blended(font,color_conf,font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=20;
        position.y=380;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        font=TTF_OpenFont(txt_font, 20);
        text=TTF_RenderText_Blended(font,renderer_conf,font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=20;
        position.y=450;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        text=TTF_RenderText_Blended(font,"Menu",font_color);
        surface=NULL;
        surface = SDL_CreateRGBSurface(0, 85, 70, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r_color, g_color, b_color));
        position.x=20;
        position.y=15;
        SDL_BlitSurface(text,NULL,surface,&position);
        texture= SDL_CreateTextureFromSurface(renderer,surface);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=20;
        position.y=610;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        SDL_RenderPresent(renderer);

        do{
            SDL_WaitEvent(&event);

            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                SDL_GetMouseState(&x_mouse,&y_mouse);
            }

            if(x_mouse>=20 && x_mouse<=340 && y_mouse>=240 && y_mouse<=268){
                choice=1;
            }

            if(x_mouse>=20 && x_mouse<=340 && y_mouse>=310 && y_mouse<=338){
                printf("%d-%d\n",x_mouse,y_mouse);
                choice=2;
            }

            if(x_mouse>=20 && x_mouse<=340 && y_mouse>=380 && y_mouse<=408){
                choice=3;
            }

            if(x_mouse>=20 && x_mouse<=340 && y_mouse>=450 && y_mouse<=478){
                choice=4;
            }

            if(x_mouse>=20 && x_mouse<=85 && y_mouse>=610 && y_mouse<=680){
                choice=5;
            }


        }while(choice==-1);

        if(choice==1){
            change=1;
            if((strcmp(app_mod_conf,"GUI"))==0){
                strcpy(app_mod_conf,"command_line");
            }else{
                strcpy(app_mod_conf,"GUI");
            }
        }

        if(choice==2){
            change=1;
            if((strcmp(font_conf,"poppins-Regular.ttf"))==0){
                strcpy(font_conf,"redressed-Regular.ttf");
            }else{
                strcpy(font_conf,"poppins-Regular.ttf");
            }
        }

        if(choice==3){
            change=1;
            if((strcmp(color_conf,"normal"))==0){
                strcpy(color_conf,"inversed");
            }else{
                strcpy(color_conf,"normal");
            }
        }

        if(choice==4){
            change=1;
            if((strcmp(renderer_conf,"software"))==0){
                strcpy(renderer_conf,"accelerate");
            }else{
                strcpy(renderer_conf,"software");
            }
        }

    }while(choice!=5);

    if(change==1){
        fp=fopen("conf.txt","wb");
        if(fp!=NULL){
            strcpy(final_conf,"app_mod:");
            strcat(final_conf,app_mod_conf);
            strcat(final_conf,"\nfont:");
            strcat(final_conf,font_conf);
            strcat(final_conf,"\ncolor:");
            strcat(final_conf,color_conf);
            strcat(final_conf,"\nrenderer:");
            strcat(final_conf,renderer_conf);

            fwrite(final_conf,strlen(final_conf),1,fp);
        }

        fclose(fp);

    }else{
        printf("ERROR: The file can't be open");
    }



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

        while(menuSelection <0 || menuSelection > 3){
            printf("\nYour choice does not correspond to the expectations !\n");
            printf("Please retype your selection :\n");
            scanf("%d", &menuSelection);
        }

        switch(menuSelection){
            case 0:
                printf("\nEnjoy your drinks ;)\nSee you next time !\n");
                return;
                break;
            case 1:
                loadBars(mysql);
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


int main(int argc, char **argv){

    /*int choice;//Cette variable va nous servir à pouvoir naviger dans les différents menu de l'application.
    int id;//Cette variable va contenir l'id de l'utilisateur une fois qu'il s'est connécter ou qu'il a créer son compte.
    SDL_bool quit=SDL_FALSE;
    int x_mouse;//Cette variable va contenir les coordonnées en X de la souris.
    int y_mouse;//Cette variable va contenir les coordonnées en Y de la souris.
    int check=0;*/
    int result;//Cette variable contient la valeur de retour de la fonction verifConfTxt;


    result=verifConfTxt();// On appelle cette fonction pour connaître les infos du fichier de configuration

    if(result!=0){//Si il n'y a pas de problème avec le fichier de configuration on rentre dans le programme.

        if(app_mod==1){
            if (SDL_Init(SDL_INIT_VIDEO)!=0){// A partir d'ici on configure toute la partie SDL (seulement si on à choisie l'option interface graphique).
                fprintf(stderr, "SDL Error : Init failed\n");
                return 0;
            }

            window=SDL_CreateWindow("Picomencer",600,100,400,700,0);
            if(window==NULL){
                printf("SDL ERROR");
            }

            if(result==1){
                renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
            }else if(result==2){
                renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
            }

            if(renderer==NULL){
                printf("SDL renderer ERROR");
            }

            TTF_Init();
        }

        if(app_mod==1){//En fonction de si on a choisi d'avoir une interface graphique ou d'être en ligne de commande on va dans le menu correspondant.
            menu_SDL();
        }else if(app_mod==2){
            menu();
        }



        /*while(!quit){


            if(event.button.button==SDL_BUTTON_LEFT){
                SDL_GetMouseState(&x_mouse,&y_mouse);

            }

            SDL_WaitEvent(&event);
            if(event.type==SDL_QUIT){
                quit=SDL_TRUE;
            }


        }*/

        if(app_mod==1){

            SDL_FreeSurface(surface);//A partir d'ici on va libérer tout nos objet proprement et fermé la librairie.
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            TTF_CloseFont(font);
            TTF_Quit();
            SDL_Quit();

        }

    }

    return 0;

}



