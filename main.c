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
}*/

/*//test code Curl
#include <stdio.h>
#include <stdlib.h>
#include <curl.h>

int main(void){
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    curl_easy_cleanup(curl);
  }
  return 0;

} */

/*//test code mysql
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

int main(int argc, char **argv){

    MYSQL *con = mysql_init (NULL);

    if(con == NULL){
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    printf("\ncoucou");
    if(mysql_real_connect(con, "localhost", "root", "root", "test", 0, NULL, 0) == NULL){
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    printf("\ncoucou2");
    if(mysql_query(con, "CREATE DATABASE theo")){
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    printf("\ncoucou3");
    mysql_close(con);
    exit(0);
}


#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "root", "root",
          NULL, 0, NULL, 0) == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

  if (mysql_query(con, "CREATE DATABASE testdb"))
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

  mysql_close(con);
  exit(0);
}* */

//start real code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <MYSQL/mysql.h>
#include <SDL.h>
#include <SDL2/SDL_ttf.h>

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

        //sprintf(query,"INSERT INTO USER (pseudo,mail,city,password) VALUES('%s','%s','%s','%s');",pseudo,mail,city,password);

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

        //mysql_query(&mysql,"INSERT INTO USER (pseudo,mail,city,password) VALUES('Izonite','armanddfl@gmail.com','Chatou','Test1234')");

         mysql_close(&mysql);




    }else{
        printf("ERROR: An error occurred while connecting to the DB!");
    }

    return id;

}

void cocktails(int id){
    int choice;
    int x_mouse;
    int y_mouse;

    /*printf("--COCKTAILS--\n");

    do{
        printf("\n1:Create a coktails\n2:List of users Cocktails\n3:Return to menu\n");
        scanf("%d",&choice);
        if(choice==1){
            createCocktails(id);
        }

        if(choice==2){
            listCocktails(id);
        }
    }while(choice!=3);*/



    do{
        choice=0;
        x_mouse=0;
        y_mouse=0;

        SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        font=TTF_OpenFont("poppins-Regular.ttf", 45);
        text=TTF_RenderText_Blended(font,"COCKTAILS",font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=75;
        position.y=50;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        font=TTF_OpenFont("poppins-Regular.ttf", 30);
        text=TTF_RenderText_Blended(font,"Create Cocktails",font_color);
        surface=NULL;
        surface = SDL_CreateRGBSurface(0, 280, 70, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
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
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
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
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
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
            createCocktails(id);
        }

        if(choice==2){
            listCocktails_SDL(id);
        }

    }while(choice!=3);

}

void createCocktails(int id){

    char name_cocktail[100];
    char txt_tmp[10];
    int id_cocktails;
    int tab_ingredient[10];
    int tab_quantity[10];
    int max=0;
    int choice;
    int quantity;
    char query[255];
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    unsigned int i = 0;
    int count_row=0;
    int check_count_row=0;

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        printf("| Creation of a cocktail |\n");
        printf("Choose the name of your cocktails\n");
        fflush(stdin);
        fgets(name_cocktail,100,stdin);
        if(name_cocktail[strlen(name_cocktail)-1]=='\n'){
            name_cocktail[strlen(name_cocktail)-1]='\0';
        }
            printf("Choose an ingredient(max 10)\n");
        do{
            printf("Choose a number (or 0 if the list is finish)\n");
            printf("(%d/10)\n)",max);
            strcpy(query,"SELECT * FROM ingredient");
            mysql_query(&mysql,query);

            result = mysql_use_result(&mysql);
            while((row = mysql_fetch_row(result))){
                if(check_count_row==0){
                    count_row++;
                }
                printf("[%s] [%s]",row[0],row[1]);
                printf("\n");
            }

            check_count_row=1;
            //printf("%d",count_row);

            scanf("%d",&choice);
            if(choice<=count_row && choice>0){
                if(choice!=0){
                    tab_ingredient[max]=choice;
                    printf("Enter the quantity (in milliter)\n");
                    scanf("%d",&quantity);
                    tab_quantity[max]=quantity;
                    max++;
                }
            }

        }while(choice!=0 && max<10 || max==0);

        itoa(id,txt_tmp,10);
        strcpy(query,"INSERT INTO COCKTAILS (name,id_user) VALUES('");
        strcat(query,name_cocktail);
        strcat(query,"','");
        strcat(query,txt_tmp);
        strcat(query,"')");

        //printf("%s",query);

        mysql_query(&mysql,query);

        strcpy(query,"SELECT LAST_INSERT_ID() FROM COCKTAILS");
        mysql_query(&mysql,query);

        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);

        if(row){
            sscanf(row[0],"%d",&id_cocktails);
            //printf("\n---%d---",id_cocktails);
        }

        for(i=0;i<max;i++){
            strcpy(query,"INSERT INTO RECIPE (id_cocktail,id_ingredient,quantity) VALUES('");
            itoa(id_cocktails,txt_tmp,10);
            strcat(query,txt_tmp);
            strcat(query,"','");
            itoa(tab_ingredient[i],txt_tmp,10);
            strcat(query,txt_tmp);
            strcat(query,"','");
            itoa(tab_quantity[i],txt_tmp,10);
            strcat(query,txt_tmp);
            strcat(query,"')");

            //printf("\n%s",query);
            mysql_query(&mysql,query);
        }

        mysql_close(&mysql);

    }


}

void listCocktails(int id){

    char query[255];
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char **tab_coktails;
    int count_row=0;
    unsigned int i = 0;
    int choice;
    int choice2;
    int check=0;
    char id_cocktail[10];

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

       printf("| List of the cocktail |\n");

       SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
       SDL_RenderClear(renderer);
       SDL_RenderPresent(renderer);

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

       //printf("#%d#",count_row);

       do{
            count_row=0;

            strcpy(query,"SELECT cocktails.id,name,user.pseudo FROM cocktails INNER JOIN user ON cocktails.id_user = user.id");
            mysql_query(&mysql,query);

            result = mysql_use_result(&mysql);
            while((row = mysql_fetch_row(result))){

                printf("[%d] |%s| created by %s",count_row+1,row[1],row[2]);
                printf("\n");
                if(check==0){
                    strcpy(tab_coktails[count_row],row[0]);

                }
                count_row++;
            }

            check=1;

            /*for(i=0;i<count_row;i++){
                printf("%s\n",tab_coktails[i]);
            }*/

            printf("Choose a cocktails (enter 0 to return to the coktails menu)\n");
            scanf("%d",&choice);

            if(choice>0 && choice<=count_row){
                strcpy(id_cocktail,tab_coktails[choice-1]);
                printf("Recipe\n");
                //printf("%s\n",id_cocktail);
                strcpy(query,"SELECT ingredient.name,quantity FROM recipe INNER JOIN cocktails ON recipe.id_cocktail = '");
                strcat(query,id_cocktail);
                strcat(query,"' and cocktails.id='");
                strcat(query,id_cocktail);
                strcat(query,"' INNER JOIN ingredient ON recipe.id_ingredient = ingredient.id");
                 mysql_query(&mysql,query);

                result = mysql_use_result(&mysql);
                while((row = mysql_fetch_row(result))){

                    printf("|%s| quantity: %s",row[0],row[1]);
                    printf("\n");
                }

                do{
                    printf("Return to cocktails list ?(enter 0)");
                    scanf("%d",&choice2);
                }while(choice2!=0);
            }

        }while(choice!=0);

       free(tab_coktails);
       mysql_close(&mysql);

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
    char id_cocktail[10];
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

       //printf("| List of the cocktail |\n");

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

       //printf("#%d#",count_row);
       do{
            count_row=0;
            choice=-1;
            x_mouse=0;
            y_mouse=0;
            y_position_increment=170;

            SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            font=TTF_OpenFont("poppins-Regular.ttf", 45);
            text=TTF_RenderText_Blended(font,"List Cocktail",font_color);
            position.x=0;
            position.y=0;
            texture= SDL_CreateTextureFromSurface(renderer,text);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=75;
            position.y=50;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            SDL_RenderPresent(renderer);

            font=TTF_OpenFont("poppins-Regular.ttf", 20);

            strcpy(query,"SELECT cocktails.id,name,user.pseudo FROM cocktails INNER JOIN user ON cocktails.id_user = user.id");
            mysql_query(&mysql,query);

            result = mysql_use_result(&mysql);
            while((row = mysql_fetch_row(result))){

                /*printf("[%d] |%s| created by %s",count_row+1,row[1],row[2]);
                printf("\n");*/
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
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
            position.x=20;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=20;
            position.y=610;
            SDL_RenderCopy(renderer, texture, NULL, &position);

            SDL_RenderPresent(renderer);

            /*for(i=0;i<count_row;i++){
                printf("%s\n",tab_coktails[i]);
            }*/

            //printf("Choose a cocktails (enter 0 to return to the coktails menu)\n");
            //scanf("%d",&choice);

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

                font=TTF_OpenFont("poppins-Regular.ttf", 45);
                text=TTF_RenderText_Blended(font,"List Cocktail",font_color);
                position.x=0;
                position.y=0;
                texture= SDL_CreateTextureFromSurface(renderer,text);
                SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
                position.x=75;
                position.y=50;
                SDL_RenderCopy(renderer, texture, NULL, &position);

                font=TTF_OpenFont("poppins-Regular.ttf", 20);

                strcpy(id_cocktail,tab_coktails[loop-choice]);
                printf("Recipe\n");
                //printf("%s\n",id_cocktail);
                strcpy(query,"SELECT ingredient.name,quantity FROM recipe INNER JOIN cocktails ON recipe.id_cocktail = '");
                strcat(query,id_cocktail);
                strcat(query,"' and cocktails.id='");
                strcat(query,id_cocktail);
                strcat(query,"' INNER JOIN ingredient ON recipe.id_ingredient = ingredient.id");
                 mysql_query(&mysql,query);

                result = mysql_use_result(&mysql);
                while((row = mysql_fetch_row(result))){

                    //printf("|%s| quantity: %s",row[0],row[1]);
                    //printf("\n");
                    strcpy(gui_txt,"|");
                    strcat(gui_txt,row[0]);
                    strcat(gui_txt,"| quantity: ");
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

                text=TTF_RenderText_Blended(font,"Return",font_color);
                surface=NULL;
                surface = SDL_CreateRGBSurface(0, 120, 70, 32, 0, 0, 0, 0);
                SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
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
                    //printf("Return to cocktails list ?(enter 0)");
                    //scanf("%d",&choice2);
                }while(choice2!=0);
            }

            if(choice==-2){
                if(loop>6){
                    loop-=1;
                }

                printf("%d\n",loop);
            }

            if(choice==-3){

                if(loop+1<=count_row){
                    loop+=1;
                }

                printf("%d\n",loop);
            }

        }while(choice!=0);

       free(tab_coktails);
       mysql_close(&mysql);

    }else{

        printf("ERROR: An error occurred while connecting to the DB!");

    }

}



int main(int argc, char **argv){

    int choice;
    int id;

    SDL_Rect rect={78,50,250,120};
    SDL_bool quit=SDL_FALSE;
    int x_mouse;
    int y_mouse;

    if (SDL_Init(SDL_INIT_VIDEO)!=0){
        fprintf(stderr, "SDL Error : Init failed\n");
        return 0;
    }

    window=SDL_CreateWindow("Picomencer",600,100,400,700,0);
    if(window==NULL){
        printf("SDL ERROR");
    }

    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);

    if(renderer==NULL){
        printf("SDL renderer ERROR");
    }

    TTF_Init();

    SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    /*SDL_SetRenderDrawColor(renderer,items.r,items.g,items.b,items.a);
    SDL_RenderFillRect(renderer,&rect);
    rect.y+=170;
    SDL_RenderFillRect(renderer,&rect);
    SDL_RenderPresent(renderer);*/
    font=TTF_OpenFont("poppins-Regular.ttf", 30);
    text=TTF_RenderText_Blended(font,"Sign In",font_color);
    surface = SDL_CreateRGBSurface(0, 170, 70, 32, 0, 0, 0, 0);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
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
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
    position.x=35;
    position.y=15;
    SDL_BlitSurface(text,NULL,surface,&position);
    texture= SDL_CreateTextureFromSurface(renderer,surface);
    SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
    position.x=110;
    position.y=230;
    SDL_RenderCopy(renderer, texture, NULL, &position);

    SDL_RenderPresent(renderer);


    /*while(!quit){


        if(event.button.button==SDL_BUTTON_LEFT){
            SDL_GetMouseState(&x_mouse,&y_mouse);

        }

        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT){
            quit=SDL_TRUE;
        }


    }




    do{
        printf("1: Sign in\n2: Sign up\n");
        scanf("%d",&choice);
    }while(choice!=1 && choice!=2);*/


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
        id=SignIn();
    }else if(choice==2){
        id=SignUp();
    }

    //printf("#%d\n#",id);


    /*do{
        printf("--MENU--\n1: Cocktails\n4:EXIT\n");
        scanf("%d",&choice);

        if(choice==1){
            cocktails(id);
        }

    }while(choice!=4);*/

    do{

        //printf("GUI CHoice\n");
        x_mouse=0;
        y_mouse=0;
        choice=0;

        SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        font=TTF_OpenFont("poppins-Regular.ttf", 50);
        text=TTF_RenderText_Blended(font,"MENU",font_color);
        position.x=0;
        position.y=0;
        texture= SDL_CreateTextureFromSurface(renderer,text);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=125;
        position.y=50;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        font=TTF_OpenFont("poppins-Regular.ttf", 30);
        text=TTF_RenderText_Blended(font,"Cocktails",font_color);
        surface=NULL;
        surface = SDL_CreateRGBSurface(0, 170, 70, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
        position.x=20;
        position.y=15;
        SDL_BlitSurface(text,NULL,surface,&position);
        texture= SDL_CreateTextureFromSurface(renderer,surface);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        position.x=110;
        position.y=170;
        SDL_RenderCopy(renderer, texture, NULL, &position);

        text=TTF_RenderText_Blended(font,"Exit",font_color);
        surface=NULL;
        surface = SDL_CreateRGBSurface(0, 90, 70, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
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
            choice=0;
            x_mouse=0;
            y_mouse=0;
            //printf("choice: %d\n",choice);
            SDL_WaitEvent(&event);

            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT){
                SDL_GetMouseState(&x_mouse,&y_mouse);
                //printf("coord: %d-%d \n",x_mouse,y_mouse);
            }

            if(x_mouse>=110 && x_mouse<=280 && y_mouse>=170 && y_mouse<=240){
                choice=1;
                printf("coord: %d-%d \n",x_mouse,y_mouse);
                x_mouse=0;
                y_mouse=0;
            }

            if(x_mouse>=20 && x_mouse<=90 && y_mouse>=610 && y_mouse<=680){
                choice=4;
                x_mouse=0;
                y_mouse=0;
            }


        }while(choice!=1 && choice!=4);

        if(choice==1){
            cocktails(id);
        }



    }while(choice!=4);

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();



    return 0;

}



