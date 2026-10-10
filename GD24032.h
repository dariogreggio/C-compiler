/*-------------------------------------------------------------------------
  GD24032.h - header per funzioni specifiche del GD24032
 
	(C) G.Dar 2026

-------------------------------------------------------------------------*/

#ifndef __GD24032_H
#define __GD24032_H 1

void _builtin_mas(int,int,int);
void _builtin_mss(int,int,int);
void _builtin_vma(int*,int,int,int);
void _builtin_ei();
void _builtin_di();
void _builtin_nand(int,int);
void _builtin_nor(int,int);
int _builtin_getflags();
void _builtin_nop();

#endif


