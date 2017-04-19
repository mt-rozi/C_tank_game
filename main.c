/*	Wersja: v0.1
	Stan na: 26 styczen 2016 01:22
	Napisane przez: Mateusz Rozmus
	Program ten, jest programem zaliczeniowym z przedmiotu Jezyk C, 
	napisany po pierwszym semestrze nauki tego jezyka.

	uwagi:
		-zrobic porzadek ze zmiennymi, dac im normalniejsze nazwy
	do napisania zostalo:
			-pocisk niszczy mur
			-jakies menu
	dodatkowe pomysly:
			-paliwo ktore po x ruchach sie konczy ale co zebraniu przedmioty uzupelnia sie
			-co x czas pojawia sie dodatkowy czolg wroga
			-czolg przejezdzajacy przez wode zwalnia
			-czolg po wyslaniu posisku zatrzymuje sie, czyli brak mozliwosci strzalow w ruchu
			-dodanie karabinu
			-statystyki obok czolgu
	uwagi, do poprawki:
			-odbieranie pancerza nie dziala tak jak powinno, z niewiadomych przyczyn odbiera na poczatku gry graczowi pewna ilosc
kompilacja:

gcc main.c -o wy $(allegro-config --libs)

biblioteka:
sudo apt-get install liballegro4-dev

poruszanie:
strzalki kierowanie czolgiem, spacja strzelanie, ESC wyjscie z gry

*/
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define COLOR_DEPTH	32
#define HEIGHT_PIX	480
#define WITDH_PIX	640
#define SPEED_OBJECT	2

volatile long speed = 0; /* zmienna obslugujaca szybkosc wykonywania petli */

void increment_speed()
{
    speed++;
}

/* obsluga timera */
LOCK_VARIABLE(speed)
LOCK_FUNCTION(increment_speed)

void rysuj_mape(int tab_map_g[416][416], int tab_map_p[416][416])
{
	/* 1-woda, 2-cegla 3-zelbet 4-krzaki */
	int i,j,x,y;
	tab_map_g[96][0]=0;tab_map_g[128][0]=4;tab_map_g[160][0]=4;tab_map_g[192][0]=1;tab_map_g[224][0]=4;tab_map_g[256][0]=4;
	tab_map_g[96][32]=2;tab_map_g[128][32]=4;tab_map_g[160][32]=1;tab_map_g[192][32]=1;tab_map_g[224][32]=1;tab_map_g[256][32]=4;
	tab_map_g[96][64]=2;tab_map_g[160][64]=1;tab_map_g[192][64]=1;tab_map_g[224][64]=1;tab_map_g[256][64]=1;
	tab_map_g[64][96]=2;tab_map_g[160][96]=1;tab_map_g[192][96]=1;tab_map_g[224][96]=1;tab_map_g[256][96]=1;
	tab_map_g[64][128]=2;tab_map_g[96][128]=1;tab_map_g[128][128]=1;tab_map_g[160][128]=1;tab_map_g[192][128]=1;tab_map_g[224][128]=1;tab_map_g[256][128]=1;tab_map_g[352][128]=2;
	tab_map_g[64][160]=2;tab_map_g[96][160]=4;tab_map_g[128][160]=1;tab_map_g[160][160]=1;tab_map_g[192][160]=1;tab_map_g[224][160]=1;
	tab_map_g[96][192]=4;tab_map_g[160][192]=1;tab_map_g[192][192]=1;tab_map_g[224][192]=1;tab_map_g[320][192]=4;
	tab_map_g[96][224]=4;tab_map_g[160][224]=1;tab_map_g[192][224]=1;tab_map_g[224][224]=1;tab_map_g[256][224]=1;tab_map_g[288][224]=4;tab_map_g[320][224]=2;
	tab_map_g[0][256]=2;tab_map_g[32][256]=2;tab_map_g[128][256]=1;tab_map_g[160][256]=1;tab_map_g[192][256]=1;tab_map_g[224][256]=1;tab_map_g[256][256]=1;tab_map_g[288][256]=4;tab_map_g[320][256]=2;
	tab_map_g[128][288]=1;tab_map_g[160][288]=1;tab_map_g[192][288]=1;tab_map_g[224][288]=1;tab_map_g[320][288]=2;
	tab_map_g[96][320]=3;tab_map_g[128][320]=1;tab_map_g[160][320]=1;tab_map_g[192][320]=1;tab_map_g[224][320]=1;tab_map_g[288][320]=2;
	tab_map_g[128][352]=4;tab_map_g[160][352]=1;tab_map_g[192][352]=1;tab_map_g[224][352]=1;tab_map_g[256][352]=4;tab_map_g[288][352]=2;
	tab_map_g[128][384]=4;tab_map_g[160][384]=4;tab_map_g[192][384]=1;tab_map_g[224][384]=4;tab_map_g[256][384]=4;tab_map_g[288][384]=4;
	
	
	/* wartosc 0 w tablicy oznacza, ze nie wolno tam wjechac */
	for(y=0;y<417;y+=32)
		for(x=0;x<417;x+=32)
		{
			if(tab_map_g[y][x]==2 || tab_map_g[y][x]==3)
			{
				for(j=0;j<32;j++)
					for(i=0;i<32;i++) tab_map_p[y+j][x+i]=10;
			}
		}
}

void rysuj_ruchy(int tab_map_c[416][416],int x, int y, int z)
{
	int i,j;
	for(i=x;i<x+32;i++)
		for(j=y;j<y+32;j++) tab_map_c[i][j]=z;
}

int main()
{
	int i,x,y;
	int predkosc_wroga=SPEED_OBJECT;
	int predkosc_gracza=SPEED_OBJECT;
	int zycia=100;
	int boot_num,boot_krok=0;				/* zmienne do boota */
	int czolg_x = 0, czolg_y = 0,key_mem=1;
	int czolg_wrog_x = 384, czolg_wrog_y = 384, czolg_wrog_ilosc=100;
	int tab_map_g[416][416],tab_map_p[416][416], tab_map_c[416][416];		/* tab_map_g - tablica ktora wyswietla grafike tab_map_p tablica ktora rysuje blokowanie dostepu do pola */
	int pocisk_gracz_x=0,pocisk_gracz_y=0, pocisk_gracz_czas=100, pocisk_gracz_mem,pocisk_gracz_odleglosc=50;
	int pocisk_wrog_x=384,pocisk_wrog_y=384, pocisk_wrog_czas=100,pocisk_wrog_mem,pocisk_wrog_odleglosc=50,boot_mem,boot_strzal;
	int dzwiek_ak=0,dzwiek_mg=0;


	BITMAP *bufor = NULL;
	BITMAP *bufor_2 = NULL;
	BITMAP *czolg_down = NULL;
	BITMAP *czolg_up = NULL;
	BITMAP *czolg_left = NULL;
	BITMAP *czolg_right = NULL;
	BITMAP *czolg_wrog_down = NULL;
	BITMAP *czolg_wrog_up = NULL;
	BITMAP *czolg_wrog_left = NULL;
	BITMAP *czolg_wrog_right = NULL;
	BITMAP *krzaki = NULL;
	BITMAP *woda = NULL;
	BITMAP *zelbet = NULL;
	BITMAP *cegla = NULL;
	BITMAP *pocisk = NULL;
	BITMAP *map = NULL;
	BITMAP *pocisk_gracz = NULL;
	BITMAP *pocisk_wrog = NULL;

	
	SAMPLE * dzwiek_tla = NULL;
	SAMPLE * dzwiek_tla_2 = NULL;
	SAMPLE * dzwiek_bomba = NULL;
	SAMPLE * dzwiek_bomba_2 = NULL;
	SAMPLE * dzwiek_tla_3 = NULL;
	/* Losowanie ruchow boota */
	srand(time(NULL));

	allegro_init();/* uruchamiamy biblioteke allegro */
	install_keyboard();/* instaluje klawiature */
	set_color_depth(COLOR_DEPTH);/* ustawia glebie kolorow, mozliwe tryby: 8,15,16,24,32 uruchamiamy zawsze przed trybem graficznym */
	set_gfx_mode( GFX_AUTODETECT_WINDOWED, WITDH_PIX, HEIGHT_PIX, 0, 0);/* wlaczenie trybu graficznego. GTX_AUTODETECT odpalanie na pelnym ekranie GTX_AUTODETECT_WINDOWED w oknie, wymiary okna, polozenie okna */
	set_palette(default_palette);/* ustawia palete kolorow, default -16 kolorow, black -czarno biala, desktop */
	
	/* dzwiek */
	install_sound( DIGI_AUTODETECT, MIDI_AUTODETECT, "" );
	set_volume( 255, 255 );
	
	/* ustawianie czestotliwosci wykonywania polecen, po to aby na kazdym komputerze pentle dzialaly z taka sama szybkoscia */
	install_timer();
	install_int_ex( increment_speed, BPS_TO_TIMER( 40 ) );/* czym wieksza wartosc tym szybcie wykonuje sie pentla, przez co czolgi szybciej sie poruszaja*/	

	/*wczytywanie plików grafiki */
	czolg_down	 = load_pcx("./images/czolg/czolg_down.pcx",default_palette);
	czolg_up	 = load_pcx("./images/czolg/czolg_up.pcx",default_palette);
	czolg_left	 = load_pcx("./images/czolg/czolg_left.pcx",default_palette);
	czolg_right	 = load_pcx("./images/czolg/czolg_right.pcx",default_palette);
	czolg_wrog_down	 = load_pcx("./images/czolg_wrog/czolg_down.pcx",default_palette);
	czolg_wrog_up	 = load_pcx("./images/czolg_wrog/czolg_up.pcx",default_palette);
	czolg_wrog_left	 = load_pcx("./images/czolg_wrog/czolg_left.pcx",default_palette);
	czolg_wrog_right = load_pcx("./images/czolg_wrog/czolg_right.pcx",default_palette);
	krzaki	= load_pcx("./images/krzaki.pcx",default_palette);
	woda	= load_pcx("./images/woda.pcx",default_palette);
	zelbet	= load_pcx("./images/zelbet.pcx",default_palette);
	cegla 	= load_pcx("./images/cegla.pcx",default_palette);
	pocisk 	= load_pcx("./images/pocisk.pcx",default_palette);
	map 	= load_pcx("./images/map.pcx",default_palette);
	pocisk_gracz	= load_pcx("./images/pocisk_gracz.pcx",default_palette);
	pocisk_wrog	= load_pcx("./images/pocisk_wrog.pcx",default_palette);

	clear_to_color(screen,makecol(0,0,0));/* zamalowywuje strukture BITMAP */
	
	bufor = create_bitmap(416,416);
	bufor_2 = create_bitmap(256,416);
	if(!bufor || !bufor_2)
	{
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Nie mogę utworzyć bufora !");
		allegro_exit();
		return 0;
	}

	if(!czolg_down || !czolg_up || !czolg_left || !czolg_right || !krzaki || !woda || !zelbet || !cegla || !map || !pocisk_gracz || !pocisk_wrog)
	{
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Blad wczytywania pliku grafiki!");
		allegro_exit();
		return 0;
	}

	dzwiek_tla = load_sample( "./sound/1.wav" );
	dzwiek_tla_2 = load_sample( "./sound/2.wav" );
	dzwiek_bomba = load_sample( "./sound/3.wav" );
	dzwiek_bomba_2 = load_sample( "./sound/4.wav" );
	dzwiek_tla_3 = load_sample("./sound/5.wav");
	if( !dzwiek_tla || !dzwiek_tla_2 || !dzwiek_bomba || !dzwiek_bomba_2 || !dzwiek_tla_3)
	{
		set_gfx_mode( GFX_TEXT, 0, 0, 0, 0 );
		allegro_message( "nie mogę załadować dzwieku !" );
		allegro_exit();
		return 0;
	}

	/* rysowanie ramki */
	for(i=0;i<468;i+=32)
	{
		blit( map, screen, 0, 0, (HEIGHT_PIX-416)/2-32, i, map->w, map->h);
		blit( map, screen, 0, 0, (HEIGHT_PIX-416)/2+416, i, map->w, map->h);
		blit( map, screen, 0, 0, i, 0, map->w, map->h);
		blit( map, screen, 0, 0, i, 448, map->w, map->h);
	}

	rysuj_mape(tab_map_g,tab_map_p);
	play_sample( dzwiek_tla, 100, 127, 1000, 1 );
	/* glowna petla programu, zajmuje sie poruszaniem elementow i odbieranie od uzytkownika kierunkow ruchow */
while(!key[KEY_ESC])
{
	while(speed > 0) /* petla timera */
	{
		/* Losowanie ruchów boota */
		if(dzwiek_ak==0)
		{
			play_sample(dzwiek_tla_2,50,127,1500,0);
			dzwiek_ak=96;
		}
			dzwiek_ak--;
		if(dzwiek_mg==0)
		{
			play_sample(dzwiek_tla_3,50,127,1000,0);
			dzwiek_mg=160;
		}
			dzwiek_mg--;
		if(boot_krok==0)
		{
			boot_num=rand()%6;
			boot_krok=7*boot_num+10;
		}
		boot_strzal=rand()%20;
		clear_to_color( bufor, makecol( 20, 20, 20) );
		clear_to_color( bufor_2, makecol( 0, 0, 0) );
		/* petla rysujaca elementy na mapie ktore sa pod graczem */
		for(x=0;x<417;x+=32)
			for(y=0;y<417;y+=32)
			{
				if(tab_map_g[x][y]==1) masked_blit( woda, bufor, 0, 0, x, y, woda->w, woda->h);
			}
		/* czyszczenie mapy z gracza, potrzebne do ustalenia warunku, zeby w siebie nie wjezdzac */
		rysuj_ruchy(tab_map_c,czolg_x,czolg_y,0);
		/* warunki ruchu gracza [Klawisz][koniec planszy][element mapy][czolg przeciwnika]*/
		if(key[KEY_UP] && czolg_y>0 && tab_map_p[czolg_x][czolg_y]!=10 && tab_map_p[czolg_x+32][czolg_y]!=10 && tab_map_c[czolg_x][czolg_y]!=1 && tab_map_c[czolg_x+32][czolg_y]!=1)
		{
			czolg_y-=predkosc_gracza;
			masked_blit( czolg_up, bufor, 0, 0, czolg_x, czolg_y, czolg_up->w, czolg_up->h);
			key_mem=1;
		}
		else if(key[KEY_DOWN] && czolg_y<384 && tab_map_p[czolg_x][czolg_y+32]!=10 && tab_map_p[czolg_x+32][czolg_y+32]!=10  && tab_map_c[czolg_x][czolg_y+32]!=1 && tab_map_c[czolg_x+32][czolg_y+32]!=1)
		{
			czolg_y+=predkosc_gracza;
			masked_blit( czolg_down, bufor, 0, 0, czolg_x, czolg_y, czolg_down->w, czolg_down->h);
			key_mem=2;
		}
		else if(key[KEY_LEFT] && czolg_x>0 && tab_map_p[czolg_x][czolg_y]!=10 && tab_map_p[czolg_x][czolg_y+32]!=10  && tab_map_c[czolg_x][czolg_y]!=1 && tab_map_c[czolg_x][czolg_y+32]!=1)
		{
			czolg_x-=predkosc_gracza;
			masked_blit( czolg_left, bufor, 0, 0, czolg_x, czolg_y, czolg_left->w, czolg_left->h);
			key_mem=3;
		}
		else if(key[KEY_RIGHT] && czolg_x<384 && tab_map_p[czolg_x+32][czolg_y]!=10 && tab_map_p[czolg_x+32][czolg_y+32]!=10  && tab_map_c[czolg_x+32][czolg_y]!=1 && tab_map_c[czolg_x+32][czolg_y+32]!=1)
		{
			czolg_x+=predkosc_gracza;
			masked_blit( czolg_right, bufor, 0, 0, czolg_x, czolg_y, czolg_right->w, czolg_right->h);
			key_mem=4;
		}
		else
		{
			if(key_mem==1) masked_blit(czolg_up, bufor, 0, 0, czolg_x, czolg_y, czolg_up->w, czolg_up->h);
			else if(key_mem==2) masked_blit(czolg_down, bufor, 0, 0, czolg_x, czolg_y, czolg_up->w, czolg_up->h);
			else if(key_mem==3) masked_blit(czolg_left, bufor, 0, 0, czolg_x, czolg_y, czolg_up->w, czolg_up->h);
			else if(key_mem==4) masked_blit(czolg_right, bufor, 0, 0, czolg_x, czolg_y, czolg_up->w, czolg_up->h);
		}

		/* warunki wystrzelonego pocisku od gracza na podstawie zmiennej key_mem */
		if(key[KEY_SPACE] && pocisk_gracz_czas>=100)
		{
			if(key_mem==1)
			{
				play_sample(dzwiek_bomba,255,127,1000,0);
				pocisk_gracz_mem=1;
				pocisk_gracz_x=czolg_x+12;
				pocisk_gracz_y=czolg_y+1;
				pocisk_gracz_y-=4;
				masked_blit( pocisk_gracz, bufor, 0, 0, pocisk_gracz_x, pocisk_gracz_y, pocisk_gracz->w, pocisk_gracz->h);
			}
			if(key_mem==2)
			{
				play_sample(dzwiek_bomba_2,255,127,1100,0);
				pocisk_gracz_mem=2;
				pocisk_gracz_x=czolg_x+12;
				pocisk_gracz_y=czolg_y+1;
				pocisk_gracz_y+=4;
				masked_blit( pocisk_gracz, bufor, 0, 0, pocisk_gracz_x, pocisk_gracz_y, pocisk_gracz->w, pocisk_gracz->h);
			}
			if(key_mem==3)
			{
				play_sample(dzwiek_bomba,255,127,1200,0);
				pocisk_gracz_mem=3;
				pocisk_gracz_x=czolg_x+1;
				pocisk_gracz_y=czolg_y+12;
				pocisk_gracz_x-=4;
				masked_blit( pocisk_gracz, bufor, 0, 0, pocisk_gracz_x, pocisk_gracz_y, pocisk_gracz->w, pocisk_gracz->h);
			}
			if(key_mem==4)
			{
				play_sample(dzwiek_bomba_2,255,127,1000,0);
				pocisk_gracz_mem=4;
				pocisk_gracz_x=czolg_x+1;
				pocisk_gracz_y=czolg_y+12;
				pocisk_gracz_x+=4;
				masked_blit( pocisk_gracz, bufor, 0, 0, pocisk_gracz_x, pocisk_gracz_y, pocisk_gracz->w, pocisk_gracz->h);
			}
			pocisk_gracz_czas=0;
			pocisk_gracz_odleglosc=50;
		}
		/* dalsza czesc wyswietlania pocisku */
		if(pocisk_gracz_odleglosc!=0)
		{	
			if(pocisk_gracz_mem==1)
			{
				pocisk_gracz_y-=4;
				masked_blit( pocisk_gracz, bufor, 0, 0, pocisk_gracz_x, pocisk_gracz_y, pocisk_gracz->w, pocisk_gracz->h);
			}
			if(pocisk_gracz_mem==2)
			{
				pocisk_gracz_y+=4;
				masked_blit( pocisk_gracz, bufor, 0, 0, pocisk_gracz_x, pocisk_gracz_y, pocisk_gracz->w, pocisk_gracz->h);
			}
			if(pocisk_gracz_mem==3)
			{
				pocisk_gracz_x-=4;
				masked_blit( pocisk_gracz, bufor, 0, 0, pocisk_gracz_x, pocisk_gracz_y, pocisk_gracz->w, pocisk_gracz->h);
			}
			if(pocisk_gracz_mem==4)
			{
				pocisk_gracz_x+=4;
				masked_blit( pocisk_gracz, bufor, 0, 0, pocisk_gracz_x, pocisk_gracz_y, pocisk_gracz->w, pocisk_gracz->h);
			}
			pocisk_gracz_odleglosc--;
		}
		/* potrzebne do ustalenia warunku, zeby w siebie nie wjezdzac */
		rysuj_ruchy(tab_map_c,czolg_x,czolg_y,2);
		/* czyszczenie mapy po bocie */
		rysuj_ruchy(tab_map_c,czolg_wrog_x,czolg_wrog_y,0);

		/* warunki ruchu boota */
		if(boot_num==1 && czolg_wrog_x<384 && tab_map_p[czolg_wrog_x+32][czolg_wrog_y]!=10 && tab_map_p[czolg_wrog_x+32][czolg_wrog_y+32]!=10 && tab_map_c[czolg_wrog_x+32][czolg_wrog_y]!=2 && tab_map_c[czolg_wrog_x+32][czolg_wrog_y+32]!=2)
		{
			boot_krok--;
			czolg_wrog_x+=predkosc_wroga;
			masked_blit( czolg_wrog_right, bufor, 0, 0, czolg_wrog_x, czolg_wrog_y, czolg_wrog_right->w, czolg_wrog_right->h);
		}
		else if(boot_num==2 && czolg_wrog_x>0 && tab_map_p[czolg_wrog_x][czolg_wrog_y]!=10 && tab_map_p[czolg_wrog_x][czolg_wrog_y+32]!=10 && tab_map_c[czolg_wrog_x][czolg_wrog_y]!=2 && tab_map_c[czolg_wrog_x][czolg_wrog_y+32]!=2)
		{
			boot_krok--;
			czolg_wrog_x-=predkosc_wroga;
			masked_blit( czolg_wrog_left, bufor, 0, 0, czolg_wrog_x, czolg_wrog_y, czolg_wrog_left->w, czolg_wrog_left->h);
		}
		else if(boot_num==3 && czolg_wrog_y>0 && tab_map_p[czolg_wrog_x][czolg_wrog_y]!=10 && tab_map_p[czolg_wrog_x+32][czolg_wrog_y]!=10 && tab_map_c[czolg_wrog_x][czolg_wrog_y]!=2 && tab_map_c[czolg_wrog_x+32][czolg_wrog_y]!=2)
		{
			boot_krok--;
			czolg_wrog_y-=predkosc_wroga;
			masked_blit( czolg_wrog_up, bufor, 0, 0, czolg_wrog_x, czolg_wrog_y, czolg_wrog_up->w, czolg_wrog_up->h);
		}
		else if(boot_num==4 && czolg_wrog_y<384 && tab_map_p[czolg_wrog_x][czolg_wrog_y+32]!=10 && tab_map_p[czolg_wrog_x+32][czolg_wrog_y+32]!=10 && tab_map_c[czolg_wrog_x][czolg_wrog_y+32]!=2 && tab_map_c[czolg_wrog_x+32][czolg_wrog_y+32]!=2)
		{	
			boot_krok--;
			czolg_wrog_y+=predkosc_wroga;
			masked_blit( czolg_wrog_down, bufor, 0, 0, czolg_wrog_x, czolg_wrog_y, czolg_wrog_down->w, czolg_wrog_down->h);
		}
		else
		{	boot_krok=0;
			masked_blit( czolg_wrog_up, bufor, 0, 0, czolg_wrog_x, czolg_wrog_y, czolg_wrog_up->w, czolg_wrog_up->h);
		}
		/* strzelanie boota */
		boot_mem=boot_num;
		if(boot_strzal==5 && pocisk_wrog_czas>=100)
		{
			if(boot_mem==3)
			{
				play_sample(dzwiek_bomba_2,255,127,1100,0);
				pocisk_wrog_mem=1;
				pocisk_wrog_x=czolg_wrog_x+12;
				pocisk_wrog_y=czolg_wrog_y+1;
				pocisk_wrog_y-=4;
				masked_blit( pocisk_wrog, bufor, 0, 0, pocisk_wrog_x, pocisk_wrog_y, pocisk_wrog->w, pocisk_wrog->h);
			}
			if(boot_mem==4)
			{
				play_sample(dzwiek_bomba,255,127,1000,0);
				pocisk_wrog_mem=2;
				pocisk_wrog_x=czolg_wrog_x+12;
				pocisk_wrog_y=czolg_wrog_y+1;
				pocisk_wrog_y+=4;
				masked_blit( pocisk_wrog, bufor, 0, 0, pocisk_wrog_x, pocisk_wrog_y, pocisk_wrog->w, pocisk_wrog->h);
			}
			if(boot_mem==2)
			{
				play_sample(dzwiek_bomba_2,255,127,1200,0);
				pocisk_wrog_mem=3;
				pocisk_wrog_x=czolg_wrog_x+1;
				pocisk_wrog_y=czolg_wrog_y+12;
				pocisk_wrog_x-=4;
				masked_blit( pocisk_wrog, bufor, 0, 0, pocisk_wrog_x, pocisk_wrog_y, pocisk_wrog->w, pocisk_wrog->h);
			}
			if(boot_mem==1)
			{
				play_sample(dzwiek_bomba,255,127,1000,0);
				pocisk_wrog_mem=4;
				pocisk_wrog_x=czolg_wrog_x+1;
				pocisk_wrog_y=czolg_wrog_y+12;
				pocisk_wrog_x+=4;
				masked_blit( pocisk_wrog, bufor, 0, 0, pocisk_wrog_x, pocisk_wrog_y, pocisk_wrog->w, pocisk_wrog->h);
			}
			pocisk_wrog_czas=0;
			pocisk_wrog_odleglosc=50;
		}
		if(pocisk_wrog_odleglosc!=0)
		{	
			if(pocisk_wrog_mem==1)
			{
				pocisk_wrog_y-=4;
				masked_blit( pocisk_wrog, bufor, 0, 0, pocisk_wrog_x, pocisk_wrog_y, pocisk_wrog->w, pocisk_wrog->h);
			}
			if(pocisk_wrog_mem==2)
			{	
				pocisk_wrog_y+=4;
				masked_blit( pocisk_wrog, bufor, 0, 0, pocisk_wrog_x, pocisk_wrog_y, pocisk_wrog->w, pocisk_wrog->h);
			}
			if(pocisk_wrog_mem==3)
			{
				pocisk_wrog_x-=4;
				masked_blit( pocisk_wrog, bufor, 0, 0, pocisk_wrog_x, pocisk_wrog_y, pocisk_wrog->w, pocisk_wrog->h);
			}
			if(pocisk_wrog_mem==4)
			{
				pocisk_wrog_x+=4;
				masked_blit( pocisk_wrog, bufor, 0, 0, pocisk_wrog_x, pocisk_wrog_y, pocisk_wrog->w, pocisk_wrog->h);
			}
			pocisk_wrog_odleglosc--;
		}
		/* rysowanie boot na mapie */
		rysuj_ruchy(tab_map_c,czolg_wrog_x,czolg_wrog_y,1);
		/* warunki trafienia pociskow */
		if(tab_map_c[pocisk_gracz_x][pocisk_gracz_y]==1)
		{
			pocisk_gracz_x=' ';
			pocisk_gracz_y=' ';
			pocisk_gracz_odleglosc=0;
			czolg_wrog_ilosc-=10;
		}
		if(tab_map_c[pocisk_wrog_x][pocisk_wrog_y]==2)
		{
			pocisk_wrog_x=' ';
			pocisk_wrog_y=' ';
			pocisk_wrog_odleglosc=0;
			zycia-=10;
		}
		if(tab_map_p[pocisk_gracz_x][pocisk_gracz_y]==10)
		{			
			pocisk_gracz_odleglosc=0;
		}
		if(tab_map_p[pocisk_wrog_x][pocisk_wrog_y]==10)
		{			
			pocisk_wrog_odleglosc=0;
		}
		
		/* pentla wyswietlajaca elementy mapy */
		for(x=0;x<385;x+=32)
			for(y=0;y<385;y+=32)
			{
				if(tab_map_g[x][y]==2) masked_blit( cegla, bufor, 0, 0, x, y, cegla->w, cegla->h);
				if(tab_map_g[x][y]==3) masked_blit( zelbet, bufor, 0, 0, x, y, zelbet->w, zelbet->h);
				if(tab_map_g[x][y]==4) masked_blit( krzaki, bufor, 0, 0, x, y, krzaki->w, krzaki->h);
			}

		if(pocisk_gracz_x<0 || pocisk_gracz_y<0 || pocisk_gracz_x>384 || pocisk_gracz_y>384 ) pocisk_gracz_odleglosc=0;
		if(pocisk_wrog_x<0 || pocisk_wrog_y<0 || pocisk_wrog_x>384 || pocisk_wrog_y>384 ) pocisk_wrog_odleglosc=0;

		blit( bufor, screen, 0, 0, (HEIGHT_PIX-416)/2, 32, 416, 416);

		/* Wyswietlanie statystyk na buforze 2 */
		textout_ex( bufor_2, font,"Pancerz ", 20, 20, makecol( 200, 200, 200 ), - 1 );
			if(zycia>=20) textprintf( bufor_2, font, 20, 40, makecol( 150, 150, 255 ),"%i%%",zycia);
			else if(zycia<20) textprintf( bufor_2, font, 20, 40, makecol( 255, 0, 0 ),"%i%%",zycia);
		textout_ex( bufor_2, font,"Pancerz wroga: ", 20, 60, makecol( 200, 200, 200 ), - 1 );
			textprintf( bufor_2, font, 20, 75, makecol( 150, 150, 255 ),"%i%%",czolg_wrog_ilosc);
		textout_ex( bufor_2, font,"Pocisk: ", 10, 280, makecol( 200, 200, 200 ), - 1 );
		if(pocisk_gracz_czas<=99)
		{
			textout_ex( bufor_2, font,"Ładowanie", 10, 300, makecol( 200, 200, 200 ), - 1 );
			textprintf( bufor_2, font, 100, 300, makecol( 150, 150, 255 ),"%i",pocisk_gracz_czas);
		}
		else textout_ex( bufor_2, font,"Gotowy", 10, 300, makecol( 255, 255, 5 ), - 1 );
		blit( bufor_2, screen, 0, 0, 500, 0, 256, 416);
	
		speed--;
		pocisk_gracz_czas++;
		pocisk_wrog_czas++;
		if(key[KEY_ESC]||zycia==0||czolg_wrog_ilosc==0) break;
	}
	if(key[KEY_ESC]||zycia==0||czolg_wrog_ilosc==0) break;
}

	if(zycia<1) textout_ex( screen, font,"Wygral komputer", 200, 100, makecol( 0, 0, 0 ), - 1 );
	else if(czolg_wrog_ilosc<1) textout_ex( screen, font,"Wygrałeś", 200, 100, makecol(0, 0, 0), - 1 );
	textout_ex( screen, font,"Koniec Gry", 200, 200, makecol( 0, 0, 0 ), - 1 );
	sleep(3);
	readkey();
	stop_sample(dzwiek_tla);
	stop_sample(dzwiek_tla_2);
	stop_sample(dzwiek_tla_3);
	stop_sample(dzwiek_bomba);
	stop_sample(dzwiek_bomba_2);
	/* usuwanie bitmap z pamieci */
	destroy_bitmap(bufor);destroy_bitmap(bufor_2);
	destroy_bitmap(czolg_up);destroy_bitmap(czolg_down);destroy_bitmap(czolg_left);destroy_bitmap(czolg_right);
	destroy_bitmap(czolg_wrog_up);destroy_bitmap(czolg_wrog_down);destroy_bitmap(czolg_wrog_left);destroy_bitmap(czolg_wrog_right);
	destroy_bitmap(pocisk_gracz);
	destroy_bitmap(pocisk_wrog);
	destroy_bitmap(krzaki);
	destroy_bitmap(woda);
	destroy_bitmap(zelbet);
	destroy_bitmap(cegla);
	destroy_bitmap(pocisk);
	destroy_bitmap(map);
	destroy_sample( dzwiek_tla);
	destroy_sample( dzwiek_tla_2);
	destroy_sample( dzwiek_bomba);
	destroy_sample( dzwiek_bomba_2);
	destroy_sample( dzwiek_tla_3);
	
	allegro_exit();/* koniec biblioteki */
	return 0;
}
