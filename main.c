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

int SignIn(){

    printf("\nSign in ok\n");

    return 0;

}

int SignUp(){

    char pseudo[50];
    char mail[100];
    char city[100];
    char password[100];
    char confirm_password[100];
    int check_password=1;
    char query[255];

    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"option");

    if(mysql_real_connect(&mysql,"localhost","root","root","picomancer",0,NULL,0)){

        printf("\nSign in:\n");

        printf("Enter your pseudo:\n\n");
        fflush(stdin);
        fgets(pseudo,50,stdin);
        if(pseudo[strlen(pseudo)-1]=='\n'){
            pseudo[strlen(pseudo)-1]='\0';
        }

        printf("Enter your mail:\n\n");
        fflush(stdin);
        fgets(mail,100,stdin);
        if(mail[strlen(mail)-1]=='\n'){
            mail[strlen(mail)-1]='\0';
        }

        printf("Enter your city:\n\n");
        fflush(stdin);
        fgets(city,100,stdin);
        if(city[strlen(city)-1]=='\n'){
            city[strlen(city)-1]='\0';
        }

        do{
            check_password=1;
            printf("Enter your password:\n\n");
            fflush(stdin);
            fgets(password,100,stdin);
            if(password[strlen(password)-1]=='\n'){
                password[strlen(password)-1]='\0';
            }
            if(strlen(password)<8){
                printf("Password to short\n");
                check_password=0;
            }

            if(password[0]<65 || password[0]>90 && check_password!=0 ){
                printf("The first letter must be a capital letter\n");
                check_password=0;
            }

            if(strpbrk(password,"0123456789")==NULL && check_password!=0){
                printf("Your password must contain a number\n");
                check_password=0;
            }

        }while(check_password!=1);


        do{
            check_password=1;
           printf("Confirm your password:\n\n");
           fflush(stdin);
           fgets(confirm_password,100,stdin);
           if(confirm_password[strlen(confirm_password)-1]=='\n'){
              confirm_password[strlen(confirm_password)-1]='\0';
           }

           if(strstr(password,confirm_password)==NULL){
                printf("Both passwords must match\n");
                check_password=0;
           }

        }while(check_password!=1);

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


        printf("\n%s",query);

        //sprintf(query,"INSERT INTO USER (pseudo,mail,city,password) VALUES('%s','%s','%s','%s');",pseudo,mail,city,password);

        mysql_query(&mysql,query);

        //mysql_query(&mysql,"INSERT INTO USER (pseudo,mail,city,password) VALUES('Izonite','armanddfl@gmail.com','Chatou','Test1234')");
         mysql_close(&mysql);



    }else{
        printf("ERROR: An error occurred while connecting to the DB!");
    }

    return 0;

}


int main(int argc, char **argv){

    int choice;
    int id;

    do{
        printf("1: Sign in\n2: Sign up\n");
        scanf("%d",&choice);
    }while(choice!=1 && choice!=2);

    if(choice==1){
        id=SignIn();
    }else if(choice==2){
        id=SignUp();
    }else{

    }



    return 0;

}



