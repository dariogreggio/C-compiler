#include <ctype.h>

ciclo(int t)
{
   int i,j=20500;

   while(t--) 
      for(i=20000; i<=j; i++);
   }

ciclo1(int t)
{
   register i,j=20500;

   while(t--) 
      for(i=20000; i<=j; i++);
   }

 ciclo2(int t)
{
   char i,j=253;

   while(t--) {
      for(i=3; i<=j; i++);
      for(i=3; i<=j; i++);
      }
   }

ciclo3(int t)
{
   long i,j=200500L;

   while(t--) 
      for(i=200000; i<=j; i++);
   }

ciclo4(int t)
{
   short i,j=20500;

   while(t--) 
      for(i=20000; i<=j; i++);
   }

main(int argc, char **argv)
{
   int t,t1,t2;

   if((argc==1) || !isdigit(*argv[1])) {
      puts("Syntax: TEST <number> <loops>");
      puts("Number is");
      puts("1. loop with int");
      puts("2. loop with register");
      puts("3. loop with char");
      puts("4. loop with long");
      puts("5. loop with short");
      exit(99);
      }

   t=atol(argv[2]);
   printf("Ticks iniziali %d\n",time(&t1));
   switch(*argv[1]) {
      case '1': 
         ciclo(t);
         break;
      case '2':
         ciclo1(t);
         break;
      case '3':
         ciclo2(t);
         break;
      case '4':
         ciclo3(t);
         break;
      case '5':
         ciclo4(t);
         break;
      }

   printf("Ticks finali %d\n",time(&t2));
   printf("Ticks impiegati %d\n",t2-t1);
   }

/* Con la versione 0.84 del compilatore (13/6/90)
   questi sono i risultati per 1000 loops:
   1 = 305  (int)
   2 = 153  (register)
   3 = 306  (char)
   4 = 325  (long)
   5 = 559  (short)
   */

/* Con la versione 0.87 del compilatore (30/6/90)
   questi sono i risultati per 1000 loops:
   1 = 202  (int)
   2 = 102  (register)
   3 = ?  (char)
   4 = 202  (long)
   5 = 370  (short)
   */

/* Con la versione 0.90 del compilatore (19/12/90)
   questi sono i risultati per 1000 loops:
   1 = 247  (int)
   2 = 86  (register)
   3 = 263  (char)
   4 = 247  (long)
   5 = 504  (short)
   */

/* N.B. in 512 righe si e' avuta la max velocita';
        a 256 c'e' un lieve rallentamento;
        a 480 si va 3 volte + lenti! */