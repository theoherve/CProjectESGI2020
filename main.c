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
#include <time.h>
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
int r_color=129;
int g_color=120;
int b_color=115;

int app_mod;
char txt_font[255];

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
    int choice;

    printf("--COCKTAILS--\n");

    do{
        printf("\n1:Create a coktails\n2:List of users Cocktails\n3:Return to menu\n");
        scanf("%d",&choice);
        if(choice==1){
            createCocktails(id);
        }

        if(choice==2){
            listCocktails(id);
        }
    }while(choice!=3);
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

        printf("Choose an ingredient(max 10)\n");
        do{
            count_row=0;
            printf("Choose a number (or 0 if the list is finish)\n");
            printf("(%d/10)\n)",max);
            strcpy(query,"SELECT * FROM ingredient");
            mysql_query(&mysql,query);

            result = mysql_use_result(&mysql);
            while((row = mysql_fetch_row(result))){

                printf("[%d] [%s]",count_row+1,row[1]);
                printf("\n");

                if(check_count_row==0){

                    strcpy(tab_choice_ingredient[count_row],row[0]);

                }

                count_row++;
            }

            check_count_row=1;

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

                if(count_row<loop && count_row>=loop-6){
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
                        text=TTF_RenderText_Blended(font,"Enter the quantity:",font_color);
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

            printf("Choose a cocktails (enter 0 to return to the coktails menu)\n");
            scanf("%d",&choice);

            if(choice>0 && choice<=count_row){
                strcpy(id_cocktail,tab_coktails[choice-1]);
                printf("Recipe\n");
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

            strcpy(query,"SELECT cocktails.id,name,user.pseudo FROM cocktails INNER JOIN user ON cocktails.id_user = user.id");
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

                strcpy(id_cocktail,tab_coktails[loop-choice]);
                printf("Recipe\n");

                strcpy(query,"SELECT ingredient.name,quantity FROM recipe INNER JOIN cocktails ON recipe.id_cocktail = '");
                strcat(query,id_cocktail);
                strcat(query,"' and cocktails.id='");
                strcat(query,id_cocktail);
                strcat(query,"' INNER JOIN ingredient ON recipe.id_ingredient = ingredient.id");
                 mysql_query(&mysql,query);

                result = mysql_use_result(&mysql);
                while((row = mysql_fetch_row(result))){

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

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    srand(time(NULL));

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){
        printf("--GAME--\n");

        do{
            printf("1: Start a game\n2: Return to the menu\n");
            scanf("%d",&choice);

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

                            do{
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


        fgets(conf_txt,255,fp);
        if((strncmp(conf_txt,"renderer:",9))==0 && check_verif_conf!=0){
            if((strncmp(conf_txt+9,"software",8))==0){
                return 1;
            }else if((strncmp(conf_txt+9,"accelerate",10))==0){
                return 2;
            }else{
                return 1;
            }
        }else{
            printf("#4#");
            check_verif_conf=0;
        }

        if(check_verif_conf==0){
            printf("ERROR: the conf file format is wrong");
            return 0;
        }


    }else{
        printf("ERROR: Conf file does not exist");
        return 0;
    }

}

void menu(){

    int choice;
    int id;

    SDL_DestroyWindow(window);

    do{
        printf("1: Sign in\n2: Sign up\n");
        scanf("%d",&choice);
    }while(choice!=1 && choice!=2);

    if(choice==1){
        id=SignIn();
    }else if(choice==2){
        id=SignUp();
    }

    do{
        printf("--MENU--\n1: Cocktails\n4:EXIT\n");
        scanf("%d",&choice);

        if(choice==1){
            cocktails(id);
        }

        if(choice==3){
            game();
        }

    }while(choice!=4);
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

                if(x_mouse>=20 && x_mouse<=90 && y_mouse>=610 && y_mouse<=680){
                    choice=4;
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

        }while(choice!=4);

}

int main(int argc, char **argv){

    int choice;
    int id;

    SDL_Rect rect={78,50,250,120};
    SDL_bool quit=SDL_FALSE;
    int x_mouse;
    int y_mouse;
    int check=0;
    int result;


    result=verifConfTxt();

    if(result!=0){

        if (SDL_Init(SDL_INIT_VIDEO)!=0){
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

        if(app_mod==1){
            menu_SDL();
        }else if(app_mod==2){
            menu();
        }

        /*SDL_SetRenderDrawColor(renderer,background.r,background.g,background.b,background.a);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

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

        SDL_RenderPresent(renderer);*/


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


        /*do{

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
        }*/

        /*do{
            printf("--MENU--\n1: Cocktails\n4:EXIT\n");
            scanf("%d",&choice);

            if(choice==1){
                cocktails(id);
            }

            if(choice==3){
                game();
            }

        }while(choice!=4);*/

       /* do{

            x_mouse=0;
            y_mouse=0;
            choice=-1;

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

            font=TTF_OpenFont("poppins-Regular.ttf", 30);
            text=TTF_RenderText_Blended(font,"Game",font_color);
            surface=NULL;
            surface = SDL_CreateRGBSurface(0, 170, 70, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 129, 120, 115));
            position.x=40;
            position.y=15;
            SDL_BlitSurface(text,NULL,surface,&position);
            texture= SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
            position.x=110;
            position.y=260;
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

                if(x_mouse>=20 && x_mouse<=90 && y_mouse>=610 && y_mouse<=680){
                    choice=4;
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

        }while(choice!=4);*/


        SDL_FreeSurface(surface);
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();

    }

    return 0;

}



