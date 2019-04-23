#include <ncurses.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/select.h>
int genereaza_litera_random(char ascuns[],int litere[])
{
//Aceasta functie genereaza un caracter printabil aleatoriu
//din cele neincercate inca. In vectorul ascii, ascii[i]
//are valoarea 1 daca i a fost utilizat si 0 daca nu a fost utilizat.
//Apoi, caracterele neincercate sunt puse intr-un alt vector,
//pentru a putea alege usor unul in mod arbitrat.
	int i,j;
	int ascii[150],neutilizate[150];
	for(i=0;i<strlen(ascuns);i++)
		{
//Aici "vizitez" caracterele deja stiute.
		j=ascuns[i];
		ascii[j]=1;
		}
	for(i=32;i<127;i++)
		if(litere[i])
//Aici "vizitez" caracterele deja incercate.
			ascii[i]=1;
	j=0;
	for(i=32;i<127;i++)
		if(ascii[i]==0)
			{
			neutilizate[j]=i;
			j++;
			}
	srand(time(NULL));
	i=rand()%j;
	return neutilizate[i];
}
void afiseaza_litere(WINDOW *spanzuratoare,int litere[])
{
//Aceasta functie afiseaza literele deja incercate de jucator.
	int i,j;
	char x;
	j=1;
	for(i=32;i<127;i++)
		if(litere[i])
//In vectorul litere[]^, pe pozitia i avem 1 daca 
//avem caracterul cu codul ASCII i deja incercat,
//si 0 daca nu a fost incercat.
			{
			x=i;
			mvwprintw(spanzuratoare,17,j,"%c",x);
			wrefresh(spanzuratoare);
			j+=2;
			}
	wrefresh(spanzuratoare);
}
int resume_game()
{
//Aceasta functie verifica daca avem la ce sa ne intoarcem, dand "resume game"
//verificand daca prima linie din resume.txt este ~~~
//In mod afirmativ returneaza 1, altfel 0.
	FILE *resume;
	resume=fopen("resume.txt","r");
	char s[100];
	fgets(s,100,resume);
	if( strcmp(s,"~~~\n") )
		return 1;
	return 0;
}
int cauta_litera(char s[],char ascuns[],char litera)
{
//Aceasta functie cauta caracterul trimis prin "char litera"
//Daca se gaseste deja in sirul ascuns, nu vom face nimic si returnam 2
//pentru a transmite asta. Daca nu, cauta litera in sirul initial si o
//"descopera" si in sirul ascuns. Returnam sem, care ne va spune daca
//s-a gasit sau nu litera.
	int sem=0,i;
	for(i=0;i<strlen(ascuns);i++)
		if(litera==ascuns[i])
			return 2;
	for(i=0;i<strlen(s);i++)
		if(s[i]==litera)
			{
			sem=1;
			ascuns[i]=litera;
			}
	return sem;
}
void ascunde_sir(char s[],char ascuns[],int *t)
{
//Functia ascunde_sir ne va genera sirul ascuns si totodata
//va simplifica sirul primit ca input, pentru lejeritatea verificarii,
//respectiv afisarii.
	char *p,rezerva[1000],rezervarezervei[1000];;
	int litere[150],i,k=0,x;
	strcpy(rezervarezervei,s);
	for(i=32;i<=140;i++)
		litere[i]=0;
	const char spatiu[2]=" ";
	p=strtok(s,spatiu);
	while(p!=NULL)
		{
		if(strlen(p)>2)
			{
			x=p[0];
			litere[x]=1;
			x=p[strlen(p)-1];
			litere[x]=1;
			}
		else
			if(strlen(p)==2)
				{
				x=p[0];
				litere[x]=1;
				}
		p=strtok(NULL,spatiu);
		}
//Obtinem caracterlee cerute(cele din capete pentru cuvinte cu lungimi
//mai mari de 2 caractere, respectiv primul caracter pentru cele de lungime 2.
	p=strtok(rezervarezervei,spatiu);
	while(p!=NULL)
		{
		for(i=0;i<strlen(p);i++)
			{
			rezerva[k]=p[i];			
			k++;
			x=p[i];
			if(litere[x]==1)
				{
				ascuns[*t]=p[i];
				*t+=1;
				}
			else
				{
				ascuns[*t]=95;
				*t+=1;
				}
			}
		ascuns[*t]=32;
		*t+=1;
		rezerva[k]=32;
		k++;
		p=strtok(NULL,spatiu);
		}
//Aici vom obtine sirul simplificat si cel ascuns. Parametrul t
//este trimis prin referinta pentru a ii modifica valoarea. Acesta reprezinta
//lungimea celor 2 siruri rezultate.
	ascuns[*t]=0;
	rezerva[k]=0;
	strcpy(s,rezerva);
//In "rezerva" avem sirul simplificat, si il vom scrie peste s
//Pentru a modifica acolo unde apelam aceasta functie.
}
void prelucreaza_sir(char s[],char sir[])
{
//Functia curenta nu "prelucreaza" literalmente sirul. Aceasta memoreaza
//fiecare secventa primita din vectorul de pointeri.
	int i,k=0,x,printabile=0;
	for(i=0;i<strlen(s);i++)
		if(isprint(s[i]))
			{
			printabile=1;
			break;
			}
//Verific aici ^ daca are caractere printabile. Daca nu are, nu are rost
//sa mergem mai departe. Doar ignoram secventa.
	if(printabile==1)
		{
		for(i=0;i<strlen(s);i++)
			if( isprint(s[i]) )
				{
				x=s[i];
				if(!(x>64 && x<91))
					{
					sir[k]=x;
					k++;
					sir[k]=0;
					}
					else
						{
						x+=32;
						sir[k]=x;
						k++;
						sir[k]=0;
						}
				}
		}
//Daca are caractere printabile, ducem literele de tipar in litere de mana
//iar celelalte caractere le lasam nemodificate.
	else
		sir[0]=0;
	//Acesta ^ ne va spune ca nu trebuie preluata o secventa anume.
	//(mai exact cele fara caractere printabile);
}
void deseneaza_piciorul_stang(WINDOW *spanzuratoare)
{
//Aici este desenul corespunzator sfarsitului de joc.
//Toate functiile care deseneaza asa ceva ce urmeaza,
//primesc ca si parametru fereastra in care deseneaza.
	wclear(spanzuratoare);
	mvwprintw(spanzuratoare,16,3,"/ | \\\\      /     \\");
	mvwprintw(spanzuratoare,15,3," /|\\\\        /   \\");
	mvwprintw(spanzuratoare,14,3,"  |\\\\         / \\");
	mvwprintw(spanzuratoare,13,3,"  |            |");
	mvwprintw(spanzuratoare,12,3,"  |            |");
	mvwprintw(spanzuratoare,11,3,"  |            |");
	mvwprintw(spanzuratoare,10,3,"  |       \\____|___/");
	mvwprintw(spanzuratoare,9,3,"  |      \\     |    /");
	mvwprintw(spanzuratoare,8,3,"  |          -----");
	mvwprintw(spanzuratoare,7,3,"  |          | x |");
	mvwprintw(spanzuratoare,6,3,"  |          -----");
	mvwprintw(spanzuratoare,5,3,"  |            |");
	mvwprintw(spanzuratoare,4,3,"  |            |");
	mvwprintw(spanzuratoare,3,3,"  |            |");
	mvwprintw(spanzuratoare,2,1,"________________________");
	box(spanzuratoare,0,0);
	wrefresh(spanzuratoare);
}
void deseneaza_piciorul_drept(WINDOW *spanzuratoare)
{
//Acest desen corespunde momentului cand mai avem o singura incercare.
	wclear(spanzuratoare);
	mvwprintw(spanzuratoare,16,3,"/ | \\\\      /");
	mvwprintw(spanzuratoare,15,3," /|\\\\        /");
	mvwprintw(spanzuratoare,14,3,"  |\\\\         /");
	mvwprintw(spanzuratoare,13,3,"  |            |");
	mvwprintw(spanzuratoare,12,3,"  |            |");
	mvwprintw(spanzuratoare,11,3,"  |            |");
	mvwprintw(spanzuratoare,10,3,"  |       \\____|___/");
	mvwprintw(spanzuratoare,9,3,"  |      \\     |    /");
	mvwprintw(spanzuratoare,8,3,"  |          -----");
	mvwprintw(spanzuratoare,7,3,"  |          |   |");
	mvwprintw(spanzuratoare,6,3,"  |          -----");
	mvwprintw(spanzuratoare,5,3,"  |            |");
	mvwprintw(spanzuratoare,4,3,"  |            |");
	mvwprintw(spanzuratoare,3,3,"  |            |");
	mvwprintw(spanzuratoare,2,1,"________________________");
	box(spanzuratoare,0,0);
	wrefresh(spanzuratoare);
}
void deseneaza_mana_stanga(WINDOW *spanzuratoare)
{
//Aceasta functie deseneaza omul, cu corpul si mainile lui.
//Corespunde momentului in care mai avem 2 incercari
	wclear(spanzuratoare);
	mvwprintw(spanzuratoare,16,3,"/ | \\\\");
	mvwprintw(spanzuratoare,15,3," /|\\\\");
	mvwprintw(spanzuratoare,14,3,"  |\\\\");
	mvwprintw(spanzuratoare,13,3,"  |            |");
	mvwprintw(spanzuratoare,12,3,"  |            |");
	mvwprintw(spanzuratoare,11,3,"  |            |");
	mvwprintw(spanzuratoare,10,3,"  |       \\____|___/");
	mvwprintw(spanzuratoare,9,3,"  |      \\     |    /");
	mvwprintw(spanzuratoare,8,3,"  |          -----");
	mvwprintw(spanzuratoare,7,3,"  |          |   |");
	mvwprintw(spanzuratoare,6,3,"  |          -----");
	mvwprintw(spanzuratoare,5,3,"  |            |");
	mvwprintw(spanzuratoare,4,3,"  |            |");
	mvwprintw(spanzuratoare,3,3,"  |            |");
	mvwprintw(spanzuratoare,2,1,"________________________");
	box(spanzuratoare,0,0);
	wrefresh(spanzuratoare);
}
void deseneaza_mana_dreapta(WINDOW *spanzuratoare)
{
//Deseneaza_mana_dreapta deseneaza mana dreapta, impreuna
//corpul si capul sau. In acest moment avem 3 incercari pe mai departe.
	wclear(spanzuratoare);
	mvwprintw(spanzuratoare,16,3,"/ | \\\\");
	mvwprintw(spanzuratoare,15,3," /|\\\\");
	mvwprintw(spanzuratoare,14,3,"  |\\\\");
	mvwprintw(spanzuratoare,13,3,"  |            |");
	mvwprintw(spanzuratoare,12,3,"  |            |");
	mvwprintw(spanzuratoare,11,3,"  |            |");
	mvwprintw(spanzuratoare,10,3,"  |       \\____|  ");
	mvwprintw(spanzuratoare,9,3,"  |      \\     |    ");
	mvwprintw(spanzuratoare,8,3,"  |          -----");
	mvwprintw(spanzuratoare,7,3,"  |          |   |");
	mvwprintw(spanzuratoare,6,3,"  |          -----");
	mvwprintw(spanzuratoare,5,3,"  |            |");
	mvwprintw(spanzuratoare,4,3,"  |            |");
	mvwprintw(spanzuratoare,3,3,"  |            |");
	mvwprintw(spanzuratoare,2,1,"________________________");
	box(spanzuratoare,0,0);
	wrefresh(spanzuratoare);
}
void deseneaza_corp(WINDOW *spanzuratoare)
{
//Desi putin previzibil, aceasta functie va desena corpul victimei.
//4 incercari pana la moarte at this point. :)
	wclear(spanzuratoare);
	mvwprintw(spanzuratoare,16,3,"/ | \\\\");
	mvwprintw(spanzuratoare,15,3," /|\\\\");
	mvwprintw(spanzuratoare,14,3,"  |\\\\");
	mvwprintw(spanzuratoare,13,3,"  |            |");
	mvwprintw(spanzuratoare,12,3,"  |            |");
	mvwprintw(spanzuratoare,11,3,"  |            |");
	mvwprintw(spanzuratoare,10,3,"  |            |");
	mvwprintw(spanzuratoare,9,3,"  |            |");
	mvwprintw(spanzuratoare,8,3,"  |          -----");
	mvwprintw(spanzuratoare,7,3,"  |          |   |");
	mvwprintw(spanzuratoare,6,3,"  |          -----");
	mvwprintw(spanzuratoare,5,3,"  |            |");
	mvwprintw(spanzuratoare,4,3,"  |            |");
	mvwprintw(spanzuratoare,3,3,"  |            |");
	mvwprintw(spanzuratoare,2,1,"________________________");
	box(spanzuratoare,0,0);
	wrefresh(spanzuratoare);
}
void deseneaza_cap(WINDOW *spanzuratoare)
{
//Functia aceasta deseneaza spanzuratoarea si capul omului.
//Tocmai a avut loc prima greseala, daca apare aceasta.
	wclear(spanzuratoare);
	mvwprintw(spanzuratoare,16,3,"/ | \\\\");
	mvwprintw(spanzuratoare,15,3," /|\\\\");
	mvwprintw(spanzuratoare,14,3,"  |\\\\");
	mvwprintw(spanzuratoare,13,3,"  |");
	mvwprintw(spanzuratoare,12,3,"  |");
	mvwprintw(spanzuratoare,11,3,"  |");
	mvwprintw(spanzuratoare,10,3,"  |");
	mvwprintw(spanzuratoare,9,3,"  |");
	mvwprintw(spanzuratoare,8,3,"  |          -----");
	mvwprintw(spanzuratoare,7,3,"  |          |   |");
	mvwprintw(spanzuratoare,6,3,"  |          -----");
	mvwprintw(spanzuratoare,5,3,"  |            |");
	mvwprintw(spanzuratoare,4,3,"  |            |");
	mvwprintw(spanzuratoare,3,3,"  |            |");
	mvwprintw(spanzuratoare,2,1,"________________________");
	box(spanzuratoare,0,0);
	wrefresh(spanzuratoare);
}
void deseneaza_spanzuratoare(WINDOW *spanzuratoare)
{
//Acesta este momentul de inceput al jocului.
//6 greseli pana la moarte.
	mvwprintw(spanzuratoare,16,3,"/ | \\\\");
	mvwprintw(spanzuratoare,15,3," /|\\\\");
	mvwprintw(spanzuratoare,14,3,"  |\\\\");
	mvwprintw(spanzuratoare,13,3,"  |");
	mvwprintw(spanzuratoare,12,3,"  |");
	mvwprintw(spanzuratoare,11,3,"  |");
	mvwprintw(spanzuratoare,10,3,"  |");
	mvwprintw(spanzuratoare,9,3,"  |");
	mvwprintw(spanzuratoare,8,3,"  |");
	mvwprintw(spanzuratoare,7,3,"  |");
	mvwprintw(spanzuratoare,6,3,"  |            |");
	mvwprintw(spanzuratoare,5,3,"  |            |");
	mvwprintw(spanzuratoare,4,3,"  |            |");
	mvwprintw(spanzuratoare,3,3,"  |            |");
	mvwprintw(spanzuratoare,2,1,"________________________");
	box(spanzuratoare,0,0);
	wrefresh(spanzuratoare);
}
void timp(char buffer[26])
{
//Functia timp modifica un vector de caractere primit ca parametru
//spre a contine data si ora curenta. Foloseste structurile predefinite
//in time.h
	time_t timer;
	struct tm* tm_info;
	time(&timer);
	tm_info=localtime(&timer);
	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
}
void new_game(int k,int car,char **sec,int incercari,int esteresume,int *maijucam)
{
//Aceasta este functia principala a programului si indeplineste mai multe
//functii pe care voi incerca sa le explic si disting pe parcursul functiei.
//Imi cer scuze ca este atat de lunga, insa am tot adus modificari ei.
	int i,lungime=0,ch=33,litere[150],underlinenumber=0,aleator;
	char buffer[26],x,s[100],ascuns[100],sirlitere[150];
//Greetings este un vector ce va contine replicile care vor aparea in timpul
//modului de joc "Infinite!". Cateva sunt menite sa fie putin batjocoritoare
//catre jucator, iar celelalte amuzante.	
	char *greetings[]=
			{
	"Someone around here does not enjoy working under pressure.",
	"Pssst, I'm pretty sure it's \"Too weak for normals!\"! Heheh..",
	"Considering your current state, you might be given 200 more chances!",
	"Has anyone seen my game of Backgammon? Pretty sure I can beat you there.",
	"This sure gets boring quickly, seeing as it takes you 1000 tries.",
	"What is Hangman without any risks?",
	"You probably thought it's knot funny, but there's a twist at the end.",
	"Thank god you chose Infinite!, my husband was dead worried!",
	"Why are there no Hangman competitions? There'd be a lot of ties!",
			};
	for(i=32;i<150;i++)
		litere[i]=0;
	srand(time(NULL));
	aleator=rand()%k;
	strcpy(s,sec[aleator]);
	ascunde_sir(s,ascuns,&lungime);
	WINDOW *spanzuratoare,*legenda;
	spanzuratoare=newwin(LINES,COLS/2,0,COLS/2);
	box(spanzuratoare,0,0);
	legenda=newwin(LINES,COLS/2,0,0);
	box(legenda,0,0);
	wrefresh(spanzuratoare);
	wrefresh(legenda);
//Pana aici au loc toate declararile si initializarile necesare unui joc.
//Legenda si spanzuratoare sunt 2 ferestre, fiecare reprezantand jumatate din
//terminalul curent, impartit pe verticala.
//Litere va fi vectorul care va memora literele introduse de jucator.
//Lungime este lungimea sirului ascuns, underlinenumber va reprezenta
//numarul de "_" din sirul ascuns. In aleator voi genera un numar random
//intre 0 si k pentru a alege o secventa.
//In ch intra fiecare caracter introdus de utilizator. i este folosit
//pentru aprcurgeri, buffer pentru ora, iar x pentru transmiterea unor valori
//intre char si int. s si ascuns sunt sirurile cu care voi lucra, iar sirlitere
//Este utilizat in cazul in care se da resume unui joc.
	mvwprintw(legenda,1,1,"Legenda:");
	timp(buffer);
	if(esteresume)
//Aceasta variabila^ verifica daca dam resume. 1=true, 0=fals.
		{
//Aici extrag din resume.txt datele pentru a da resume unui joc.
//Am considerat ca ar fi simplu doar sa rescriu unitatile
//pe care deja le foloseam. Astfel voi citi s(sirul original), cel ascuns,
//numarul de incercari si literele introduse in run-ul anterior.
		FILE *resume;
		resume=fopen("resume.txt","r");
		fgets(s,100,resume);
		s[strlen(s)-1]=0;
		fgets(ascuns,100,resume);
		ascuns[strlen(ascuns)-1]=0;
		char a[3];
		fgets(a,3,resume);
		incercari=a[0]-48;
		fgets(sirlitere,150,resume);
		sirlitere[strlen(sirlitere)-1]=0;
//Tot fac asta pentru a scapa de \n ^.
		for(i=0;i<strlen(sirlitere);i++)
			{
			aleator=sirlitere[i];
			litere[aleator]=1;
			}
		fclose(resume);
		}
	mvwprintw(legenda,2,1,"Data si ora: %s",buffer);
	mvwprintw(legenda,3,1,"R=actualizeaza ora&data");
	mvwprintw(legenda,4,1,"Q=intoarcere la meniul principal");
	mvwprintw(legenda,5,1,"Caractere citite: %i",car);
	mvwprintw(legenda,6,1,"Numar de expresii valide: %i",k);
//Aceste afisari sunt realizate in fereastra "legenda" care va cotine cateva
//informatii simple despre comenzi, data si ora si numarul de expresii citite
//si numarul total de caractere al lor.
	mvwprintw(spanzuratoare,1,1,"%s",ascuns);
	wrefresh(spanzuratoare);
	wrefresh(legenda);
	keypad(legenda,TRUE);
	keypad(spanzuratoare,TRUE);
	keypad(stdscr,TRUE);
	switch(incercari)
		{
//Acest switch va desena momentul de unde am ramas(sau cel initial), in
//functie de valoarea variabilei "incercari". In acest switch afisez si
//sirul ascuns, respectiv literele incercate.
		case 6:
			deseneaza_spanzuratoare(spanzuratoare);
			afiseaza_litere(spanzuratoare,litere);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			wrefresh(spanzuratoare);
			break;
		case 5:
			deseneaza_cap(spanzuratoare);
			afiseaza_litere(spanzuratoare,litere);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			wrefresh(spanzuratoare);
			break;
		case 4:
			deseneaza_corp(spanzuratoare);
			afiseaza_litere(spanzuratoare,litere);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			wrefresh(spanzuratoare);
			break;
		case 3:
			deseneaza_mana_dreapta(spanzuratoare);
			afiseaza_litere(spanzuratoare,litere);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			wrefresh(spanzuratoare);
			break;
		case 2:
			deseneaza_mana_stanga(spanzuratoare);
			afiseaza_litere(spanzuratoare,litere);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			wrefresh(spanzuratoare);
			break;
		case 1:
			deseneaza_piciorul_drept(spanzuratoare);
			afiseaza_litere(spanzuratoare,litere);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			wrefresh(spanzuratoare);		
			break;
		case 0:
			deseneaza_piciorul_stang(spanzuratoare);
			afiseaza_litere(spanzuratoare,litere);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			wrefresh(spanzuratoare);
			break;
		default:
			deseneaza_piciorul_stang(spanzuratoare);
			afiseaza_litere(spanzuratoare,litere);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			srand(time(NULL));
			aleator=rand()%9;
			mvwprintw(spanzuratoare,20,1,"%s",greetings[aleator]);
			//De fiecare data generez un numar aleator
			//Ce va corespunde unei replici din greetings[]
			//Apoi o afisez in fereastra spanzuratoare.
			wrefresh(spanzuratoare);
			break;
		}
	while(ch!=81)
//Daca se apasa Q ^, iesim din loop.
		{
		int nfds,sel,sem=0;
//Folosesc acest sem deoarece nu mai citeste daca nu ii dau break in functie
//de el. Probabil sa fi facut eu ceva gresit, dar cred ca are legatura cu
//faptul ca folosesc getch()
		fd_set read_descriptors;
		struct timeval timeout;
		nfds=1;
		FD_ZERO(&read_descriptors);
		FD_SET(0, &read_descriptors);
		timeout.tv_sec=7;
		timeout.tv_usec=0;
		while(1)
		{
//Aici am rezolvat cerinta 5, utilizand resursele postate pe forum.
//
		sel=select(nfds, &read_descriptors, NULL, NULL, &timeout);
			switch(sel)
				{
//Acest select ne va spune daca s-a apasat sau nu vreo tasta.
				case 1:
					ch=wgetch(spanzuratoare);
//In acest wgetch preiau caractere din fereastra spanzuratoare,
//in care ne vom si afla mai mereu pentru a facilita aceasta citire.
					sem=1;
//Acest sem este un "semafor" care ne va spune sa iesim din loop
//odata ce obtinem o valoare pentru ch.
					break;
				case 0:
					ch=genereaza_litera_random(ascuns,litere);
//Pentru generarea unui caracter printabil aleator si neintrodus, folosim
//functia scrisa mai sus.
					sem=1;
					break;
				}
		FD_SET(0, &read_descriptors);
		timeout.tv_sec=7;
		timeout.tv_usec=0;
		if(sem)
			break;
		}
	if(ch!=KEY_UP && ch!=KEY_LEFT && ch!=KEY_RIGHT && ch!=KEY_DOWN)
//Am decis sa ignor sagetile, pentru ca acestea nu sunt preluate corespunzator
//de getch. As fi dezactivat keypad pentru spanzuratoarea, insa nu a parut
//sa functioneze okay. Probabil am facut ceva gresit :(
		if( (isprint(ch)) && (ch>=32) && (ch<=127) )
//Ma asigur ca este printabil, altfel il ignor.
		switch(ch)
		{
		case 82:
			timp(buffer);
			mvwprintw(legenda,2,1,"Data si ora: %s",buffer);
			wrefresh(legenda);
			mvwprintw(spanzuratoare,1,1,"%s",ascuns);
			wrefresh(spanzuratoare);
//R pentru reactualizarea orei si a datei.
				break;
		case 81:
				break;
//In caz de Q, iesim din loop.
		default://In orice alt caz, verificam daca gasim caracterul.
			if(cauta_litera(s,ascuns,ch)==1)
				{
				mvwprintw(spanzuratoare,1,1,"%s",ascuns);
				wrefresh(spanzuratoare);
//Daca am gasit caracterul in sir, actualizam sirul ascuns.
				}
			else 
			if((cauta_litera(s,ascuns,ch)==0)&&(litere[ch]==0))
				{
//Daca nu, scadem numarul de incercari(pe care le mai avem) si
//Memoram caracterul introdus. In functie de valoarea lui "incercari"
//Programul va folosi desena etapa corespunzatoare a spanzuratorii.
				incercari--;
				litere[ch]=1;
				switch(incercari)
					{
//Cateva dintre apeluri sunt imaprtite pe cate 2 randuri aici deoarece
//depaseau limita de 80 de randuri. Sper sa fie okay, astfel. :)
					case 5:
						deseneaza_cap(spanzuratoare);
						afiseaza_litere(spanzuratoare,
								litere);
						mvwprintw(spanzuratoare,1,1,"%s",
								ascuns);
						wrefresh(spanzuratoare);
						break;
					case 4:
						deseneaza_corp(spanzuratoare);
						afiseaza_litere(spanzuratoare,
								litere);
						mvwprintw(spanzuratoare,1,1,"%s",
								ascuns);
						wrefresh(spanzuratoare);
						break;
					case 3:
						deseneaza_mana_dreapta(
								spanzuratoare);
						afiseaza_litere(spanzuratoare,
								litere);
						mvwprintw(spanzuratoare,1,1,"%s"
								,ascuns);
						wrefresh(spanzuratoare);
						break;
					case 2:
						deseneaza_mana_stanga(
							spanzuratoare);
						afiseaza_litere(spanzuratoare,
								litere);
						mvwprintw(spanzuratoare,1,1,"%s"
								,ascuns);
						wrefresh(spanzuratoare);
						break;
					case 1:
						deseneaza_piciorul_drept(spanzuratoare);
						afiseaza_litere(spanzuratoare,
								litere);
						mvwprintw(spanzuratoare,1,1,"%s"
								,ascuns);
						wrefresh(spanzuratoare);
						break;
					case 0:
						deseneaza_piciorul_stang(
								spanzuratoare);
						afiseaza_litere(spanzuratoare,
								litere);
						mvwprintw(spanzuratoare,1,1,"%s",
									ascuns);
						wrefresh(spanzuratoare);
						break;
					default:
						deseneaza_piciorul_stang(
							spanzuratoare);
						afiseaza_litere(spanzuratoare,
								litere);
						mvwprintw(spanzuratoare,1,1,"%s"
								,ascuns);
						srand(time(NULL));
						aleator=rand()%9;
						mvwprintw(spanzuratoare,
						20,1,"%s",greetings[aleator]);
						//Asemanator ca mai sus, afisez
						//O replica la intamplare
						//adresata jucatorului	
						wrefresh(spanzuratoare);
						break;
//In acest switch afisez si (manual) sirul ascuns deasupra spanzuratorii.
					}
				}
			}
		underlinenumber=0;
		for(i=0;i<strlen(ascuns);i++)
			if(ascuns[i]==95)
				underlinenumber++;
//Verific daca mai am "_" in sirul ascuns.
		if(!underlinenumber)
			{
			mvwprintw(spanzuratoare,18,1,"WIN");
			mvwprintw(spanzuratoare,19,1,"Apasati N pentru New ");
			wprintw(spanzuratoare,"Game sau Q pentru Quit.");
			wrefresh(spanzuratoare);
			ch=getch();
			break;
			}
//Daca nu mai am, inseamna ca s-a obtinut "WIN" si citim un nou caracter.
		else
			if(!incercari)
			{
			mvwprintw(spanzuratoare,18,1,"GAME OVER.");
			mvwprintw(spanzuratoare,19,1,"Apasati N pentru New ");
			wprintw(spanzuratoare,"Game sau Q pentru Quit.");
			wrefresh(spanzuratoare);
			ch=getch();
			break;
//Daca mai sunt "_" dar nu mai avem incercari, s-a atins "GAME OVER".
//Si aici citim un nou caracter. :)
			}
		}
	while( ch!=81 && ch!=78)
		ch=getch();
//Daca nu s-a citit N sau Q, recitim caractere.
	if(ch==81)
		{
		*maijucam=0;//Acesta ne va spune ca nu se mai apeleaza
//new_game, deoarece s-a ales optiunea quit game.
		if( ! (incercari==0 || underlinenumber==0) )
//Daca s-a iesit, dar mai erau incercari si "_", scriem sirul de cautat,
//pe cel ascuns, numarul de incercari si caracterele introduse in resume.txt
			{
			FILE *resume;
			resume=fopen("resume.txt","w");
			fputs(s,resume);
			fputs("\n",resume);
			fputs(ascuns,resume);
			fputs("\n",resume);
			x=incercari+48;
			fputc(x,resume);
			fputs("\n",resume);
			for(i=32;i<140;i++)
				{
				if(litere[i])
					{
					x=i;
					fputc(x,resume);
					}
				}
			fputs("\n",resume);
			fclose(resume);
			}
		else
			{FILE *resume;
			resume=fopen("resume.txt","w");
			fputs("~~~\n",resume);
			fclose(resume);
//Daca s-a iesit la final de joc, scriem "~~~" pentru a nu se putea da resume
//In cazul in care se cere, dar nu avem la ce sa dam.
			}
		delwin(spanzuratoare);
		delwin(legenda);
//Stergem ferestrele spanzuratoare si legenda.
		clear();
		}
	else
		if(ch==78)
			{
			*maijucam=1;
			FILE *resume;
			resume=fopen("resume.txt","w");
			fputs("~~~\n",resume);
			fclose(resume);
			delwin(spanzuratoare);
			delwin(legenda);
			clear();
//Daca s-a ales New Game, la sfarsitul unui joc incheiat, scriem "~~~" in resume.txt
//Si il facem pe "maijucam" 1 pentru a apela iarasi new_game.
			}
}	
void afiseaza_meniu(WINDOW *fereastra_meniu, int highlight)
{
//Functia afiseaza_meniu va afisa meniul principal de joc,
//format din New Game, Resume game si Quit
	char *optiuni[]={
			"New game",
			"Resume game",
			"Infinite!",
			"Quit",};
	int nr_optiuni=sizeof(optiuni)/sizeof(char*);
//Aici avem numarul total de optiuni.
	int x,y,i;
	x=2;
	y=2;
	box(fereastra_meniu,0,0);
//Desenam marginile ferestrei corespunzatoare meniului.
	for(i=0;i<nr_optiuni;i++)
	{
//Variabila highlight ne va spune pe cine trebuie sa highlight-uim
//in timpul navigarii prin meniu.
		if(highlight==i+1)
		{
			wattron(fereastra_meniu, A_REVERSE);
			//highlight ON
			mvwprintw(fereastra_meniu, y, x, "%s",optiuni[i]);
			wattroff(fereastra_meniu, A_REVERSE);
			//highlight OFF
		}
		else
			mvwprintw(fereastra_meniu, y, x, "%s",optiuni[i]);
		y++;
	}
	wrefresh(fereastra_meniu);
}


int meniu(int k, int caractere,char **secvente)
{
	char *optiuni[]={"New game","Resume game","Infinite!","Quit",};
	int maijucam,nr_optiuni=sizeof(optiuni)/sizeof(char*);
	WINDOW *fereastra_meniu;
//Optiuni reprezinta un vector cu optiunile din meniu,
//maijucam ne va spune daca s-a apelat new game sau quit game la sfarsitul
//unei partide. nr_optiuni este numarul de optiuni din meniu.
//Iar fereastra_meniu este fereastra corespunzatoarea meniului.
	int highlight=1,curent=0,c;
//Variabila curent este alegerea curenta iar c va prelua intrarile
//de la tastatura. Se misca sus/jos pentru sageata sus/jos corespunzator.
	initscr();
	start_color();
	init_pair(1,COLOR_RED,COLOR_BLACK);
	clear();
	noecho();
//Nu lasa sa fie afisate caracterele introduse de utilizator.
	fereastra_meniu=newwin(10,20,5,5);
	keypad(fereastra_meniu,TRUE);
	mvprintw(1,0,"Folositi sageata sus/jos pentru a naviga.");
	mvprintw(2,0,"Apasati enter pentru a select o optiune anume.");
	refresh();
	afiseaza_meniu(fereastra_meniu, highlight);
	while(1)
		{
		c=wgetch(fereastra_meniu);
		switch(c)
		{
		case KEY_UP:
		//sageata sus
			if(highlight==1)
				highlight = nr_optiuni;
			else
				highlight--;
			break;
		case KEY_DOWN:
		//sageata jos
			if(highlight==nr_optiuni)
				highlight=1;
			else
				highlight++;
			break;
		case 10:
		//enter
			curent=highlight;
			break;
		default:
			refresh();
			break;
		}
		afiseaza_meniu(fereastra_meniu,highlight);
		//desenam meniul pana se selecteaza ceva anume.
		if(curent!=0)
			break;
		}
	if(highlight==1)
//Highlight ne spune ce optiune s-a ales prin ENTER.
//1=New Game; 2=Resume Game; 3=Quit.
		{
		delwin(fereastra_meniu);
		clear();
		refresh();
		maijucam=1;
		while(maijucam)
			{
			refresh();
			new_game(k,caractere,secvente,6,0,&maijucam);
//k=numarul de secvente, caractere=numarul de caractere al secventelor
//secvente=vector de pointeri continand secventele, 6=numar de incercari
//Este 6 pentru ca apelam new_game. 0 ne va spune ca nu dam resume, iar
//maijucam ne va spune ulterior daca s-a ales iarasi new game sau quit game.
			refresh();
			if(!maijucam)
				break;
			}
		return 1;
		}
	else
		if(highlight==2)
			{
			if( resume_game() )
				{
				delwin(fereastra_meniu);
				clear();
				refresh();
				maijucam=1;
				while(maijucam)
					{
					refresh();
					new_game(k,caractere,secvente,6,1,&maijucam);
//Similar ca mai sus, insa avem 1 in loc de 0 la al 5-lea parametru deoarece
//aici dam resume game.
					refresh();
					if(!maijucam)
						break;
					}
				return 1;
				}
			}
		else
		if(highlight==3)	
			{
			delwin(fereastra_meniu);
			clear();
			refresh();
			maijucam=1;
			while(maijucam)
				{
				refresh();
				new_game(k,caractere,secvente,1000,0,&maijucam);
				refresh();
				if(!maijucam)
//Aici am inserat modul de joc "Infinite!". L-am implementat deoarece parea
//destul de simplu avand in vedere cum a fost realizata functia new_game.
//In principal, apelez new_game cu 1000 pentr incercari, iar restul
//functioneaza similar :)
					break;
				}
			return 1;
			}
//Daca nu s-a ales new game sau resume game, vom iesi din functie.
	refresh();
	clear();
	endwin();
	return 0;
}
int main(int argc,char** argv)
{
	char s[1000],sir[1000];
	char **secvente;
//Secvente este un vector de pointeri in care memorez liniile citite din fisiere.
//In s si in sir voi prelucra sirurile.
	int caractere=0,i,cap=10,k=0;
//Caractere=numar total de caractere al expresiilor valide,
//i este pentru parcurgere, cap este capacitatea corespunzatoare lui secvente.
//k=numarul total de expresii valide.
	secvente=(char**)malloc(cap*sizeof(char*));
	FILE *f;
//in f voi deschide fiecare fisier pentru a il citi linie cu linie.
	if(argc==1)
		{
		printf("[Eroare]: Nu s-au dat argumente de comanda.\n");
		return 1;
//Daca nu s-a dat niciun argument, iesim direct.
		}
	else
		for(i=1;i<argc;i++)
		{
			f=fopen(argv[i],"r");
			if(!f)
				{
				printf("[Eroare]: Fisirul %s nu",argv[i]);
				printf(" poate fi deschis.\n");
//Daca nu punem deschide vreun fisier, afisam eroare si iesim.
				return 1;
				}
			while(fgets(s,100,f)!=0)
			{
			s[strlen(s)-1]=0;
//Taiem \n.
			prelucreaza_sir(s,sir);
			if( strlen(s))
				{
				if(k==cap)
//Daca s-a atins capacitatea, vom dubla cap si folosim realloc pe **secvente.
					{
					cap*=2;
					secvente=(char**)realloc(secvente,cap*sizeof(char*));
					}
				if(strlen(sir)>0)
//Daca strlen(sir)>0 inseamna ca avem o secventa valiada, deci o memoram.
					{
					secvente[k]=(char*)malloc(1000*sizeof(char));
					strcpy(secvente[k],sir);
					caractere+=strlen(secvente[k]);
					k++;
					}
				}
			}
		}
	while(1)
		{
//Functia meniu ne va returna daca s-a ales "quit" sau nu.
//Daca s-a ales "quit", atunci iesim din loop.
		cap=meniu(k,caractere,secvente);
		if(!cap)
			break;
		}
	for(i=0;i<k;i++)
		free(secvente[i]);
	free(secvente);
	return 1;
}
