//
//  refvars.h
//  i6809asm
//
//  Created by Laurent Favard on 12/06/2025.
//  Copyright © 2025 Laurent. All rights reserved.
//

#ifndef     __REFS_VARIABLES__
#define     __REFS_VARIABLES__  __REFS_VARIABLES__

#include <stdio.h>

extern struct  nlist *root;
extern struct oper pseudo[];
extern struct oper table[];

extern unsigned long NMNE_SIZE;
extern unsigned long NPSE_SIZE;

extern int     E_pc;
extern char    Line[MAXBUF];
extern int     E_bytes[E_LIMIT];
extern int     E_total;
extern int     Line_num;
extern int     Err_count; 

extern int     Line_num;
extern int     Last_sym;

extern char    *Optr;
extern int     Result;
extern int     Pc;
extern char    Operand[MAXBUF];
extern int     Pass;
extern int     Cycles;
extern int     Force_word;
extern int     Force_byte;

extern char    Label[MAXLAB];
extern int     P_force;
extern int     Old_pc;
extern int     HXTflag;
extern int     Lflag;              
extern int     HXTflag;            
extern int     Bflag;              
extern int     Oflag;              
extern int     Cflag;              
extern int     Sflag;              
extern int     Rflag;              
extern int     Wflag;              
extern int     P_force;            
extern int     P_total;            
extern int     P_bytes[P_LIMIT];
extern int     Cycles;             
extern long    Ctotal;             
extern int     N_page;             
extern int     Page_num;           
extern FILE    *Objfil;           
extern FILE    *Binfil;           
extern FILE    *Lstfil;           
extern FILE    *Symfil;           
extern FILE    *Crffil;           
extern FILE    *HXTfil; 

extern int     Pass;                
extern int     N_files;             
extern FILE    *Fd;                 
extern int     Cfn;                 
extern int     Ffn;                 
extern int     F_ref;               
extern char    **Argv;              

#endif

