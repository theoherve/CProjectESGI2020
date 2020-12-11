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
}*/


/*#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);

  if (con == NULL){
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL){
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

  if (mysql_query(con, "CREATE DATABASE testdb")){
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

  mysql_close(con);
  exit(0);
}*/

//start real code
#include <stdio.h>
#include <stdlib.h>
#include <curl.h>

int main(int argc, char **argv){

    CURL *curl;
    FILE *fp;
    int result;
    char errbuf[CURL_ERROR_SIZE];

    //fp = fopen(argv[2], "wb");
    fp = fopen("testCurlApiExport/script1.json", "wb");

    curl = curl_easy_init(); //initialize CURL fonction
    if(curl){

        //curl_easy_setopt(curl, CURLOPT_URL, argv[1]);

        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_func); //marche pas

        curl_easy_setopt(curl, CURLOPT_URL, "https://opendata.paris.fr/explore/dataset/etalages-et-terrasses/download/?format=json&timezone=Europe/Berlin&lang=fr"); //CURLOPT_ULR allow us to enter the url of the file we want to dl
        //curl_easy_setopt(curl, CURLOPT_URL, "https://upload.wikimedia.org/wikipedia/commons/thumb/3/31/Nusfjord_road%2C_2010_09.jpg/1280px-Nusfjord_road%2C2010_09.jpg");

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //alow to write on the file we dl

        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = 0;

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

        result = curl_easy_perform(curl); //return if the dl was successful (might take few seconds)

        /*if(result != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result));
        }else{
            printf("Download successful !\n");
        }*/

        if(result != CURLE_OK) {
            size_t len = strlen(errbuf);
            fprintf(stderr, "\nlibcurl: (%d) ", result);
            if(len)
                fprintf(stderr, "%s%s", errbuf, ((errbuf[len - 1] != '\n') ? "\n" : ""));
            else
                fprintf(stderr, "%s\n", curl_easy_strerror(result));
            }

        fclose(fp);
        curl_easy_cleanup(curl);
    }

    return 0;

}
