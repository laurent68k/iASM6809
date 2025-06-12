#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "as.h"
#include "func.h"
#include "mc6809.h"
#include "directives.h"

#define strncasecmp strncmp

/**
 * global variables
 */

int     Line_num =0;            /* current line number          */
int     Err_count =0;           /* total number of errors       */
char    Line[MAXBUF] = {0};     /* input line buffer            */
char    Label[MAXLAB] = {0};    /* label on current line        */
char    Op[MAXOP] = {0};        /* opcode mnemonic on current line      */
char    Operand[MAXBUF] = {0};  /* remainder of line after op           */
char    *Optr =0;               /* pointer into current Operand field   */
int     Result =0;              /* result of expression evaluation      */
int     Force_word =0;          /* Result should be a word when set     */
int     Force_byte =0;          /* Result should be a byte when set     */
int     Pc =0;                  /* Program Counter              */
int     Old_pc =0;              /* Program Counter at beginning */

int     Pc_ROMORG = 0;

int     Last_sym =0;            /* result of last lookup        */

int     Pass =0;                /* Current pass #               */
int     N_files =0;             /* Number of files to assemble  */
FILE    *Fd =0;                 /* Current input file structure */
int     Cfn =0;                 /* Current file number 1...n    */
int     Ffn =0;                 /* forward ref file #           */
int     F_ref =0;               /* next line with forward ref   */
char    **Argv =0;              /* pointer to file names        */

int     E_total =0;             /* total # bytes for one line   */
int     E_bytes[E_LIMIT] = {0}; /* Emitted held bytes           */
int     E_pc =0;                /* Pc at beginning of collection*/

int     Lflag = 0;              /* listing flag 0=nolist,   1=list   */
int     HXTflag = 0;            /* hexa text flag 0=no,   1=HexaText   */
int     Bflag = 0;              /* binary  flag 0=nobinary, 1=binary */
int     Oflag = 0;              /* s19     flag 0=noS19,    1=S19    */
int     Cflag = 0;              /* cycles  flag 0=nocycles, 1=cycles */
int     Sflag = 0;              /* symbol  flag 0=nosymbol, 1=symbol */
int     Rflag = 0;              /* crf     flag 0=nocrf,    1=crf    */
/* Warn about comment not starting with comment sign ';' '*'         */
int     Wflag = 1;              /* warning flag 0=none,     1=warning*/

int     P_force = 0;            /* force listing line to include Old_pc */
int     P_total =0;             /* current number of bytes collected    */
int     P_bytes[P_LIMIT] = {0}; /* Bytes collected for listing  */

int     Cycles = 0;             /* # of cycles per instruction  */
long    Ctotal = 0;             /* # of cycles seen so far */
int     N_page = 0;             /* new page flag */
int     Page_num = 2;           /* page number */

struct  nlist *root;            /* root node of the tree */

FILE    *Objfil =0;             /* S19    file's file descriptor*/
FILE    *Binfil =0;             /* Binary file's file descriptor*/
FILE    *Lstfil =0;             /* List   file's file descriptor*/
FILE    *Symfil =0;             /* Symbol file's file descriptor*/
FILE    *Crffil =0;             /* Crf    file's file descriptor*/
FILE    *HXTfil = 0;            /* HexTxt file's file descriptor*/

char    Obj_name[MAXPATH]   = "             ";
char    Bin_name[MAXPATH]   = "             ";
char    Lst_name[MAXPATH]   = "             ";
char    Sym_name[MAXPATH]   = "             ";
char    Crf_name[MAXPATH]   = "             ";
char    Hexa_name[MAXPATH]  = "             ";

struct oper table[] = {
/*
    MNE     CLASS   BASE    CYCLES */
    "abx",  INH,    58,     3,
    "adca", GEN,    137,    2,
    "adcb", GEN,    201,    2,
    "adda", GEN,    139,    2,
    "addb", GEN,    203,    2,
    "addd", LONGIMM,195,    4,
    "anda", GEN,    132,    2,
    "andb", GEN,    196,    2,
    "andcc",IMM,    28,     3,
    "asl",  GRP2,   8,    4,
    "asla", INH,    72,    2,
    "aslb", INH,    88,    2,
    "asr",  GRP2,   7,    4,
    "asra", INH,    71,    2,
    "asrb", INH,    87,    2,
    "bcc",  REL,    36,    3,
    "bcs",  REL,    37,    3,
    "beq",  REL,    39,    3,
    "bge",  REL,    44,    3,
    "bgt",  REL,    46,    3,
    "bhi",  REL,    34,    3,
    "bhs",  REL,    36,    3,
    "bita", GEN,    133,    2,
    "bitb", GEN,    197,    2,
    "ble",  REL,    47,    3,
    "blo",  REL,    37,    3,
    "bls",  REL,    35,    3,
    "blt",  REL,    45,    3,
    "bmi",  REL,    43,    3,
    "bne",  REL,    38,    3,
    "bpl",  REL,    42,    3,
    "bra",  REL,    32,    3,
    "brn",  REL,    33,    3,
    "bsr",  REL,    141,    7,
    "bvc",  REL,    40,    3,
    "bvs",  REL,    41,    3,
    "clr",  GRP2,   15,    4,
    "clra", INH,    79,    2,
    "clrb", INH,    95,    2,
    "cmpa", GEN,    129,    2,
    "cmpb", GEN,    193,    2,
    "cmpd", P2GEN,  131,    5,
    "cmps", P3GEN,  140,    5,
    "cmpu", P3GEN,  131,    5,
    "cmpx", LONGIMM,140,    4,
    "cmpy", P2GEN,  140,    5,
    "com",  GRP2,   3,    4,
    "coma", INH,    67,    2,
    "comb", INH,    83,    2,
    "cpx",  LONGIMM,140,    4,      /* for compatibility with old code */
    "cwai", IMM,    60,     20,
    "daa",  INH,    25,    2,
    "dec",  GRP2,   10,    4,
    "deca", INH,    74,    2,
    "decb", INH,    90,    2,
    "eora", GEN,    136,    2,
    "eorb", GEN,    200,    2,
    "exg",  RTOR,   30,     8,
    "inc",  GRP2,   12,    4,
    "inca", INH,    76,    2,
    "incb", INH,    92,    2,
    "jmp",  GRP2,   14,     1,
    "jsr",  NOIMM,  141,    5,
    "lbcc", P2REL,  36,    6,
    "lbcs", P2REL,  37,    6,
    "lbeq", P2REL,  39,    6,
    "lbge", P2REL,  44,    6,
    "lbgt", P2REL,  46,    6,
    "lbhi", P2REL,  34,    6,
    "lbhs", P2REL,  36,    6,
    "lble", P2REL,  47,    6,
    "lblo", P2REL,  37,    6,
    "lbls", P2REL,  35,    6,
    "lblt", P2REL,  45,    6,
    "lbmi", P2REL,  43,    6,
    "lbne", P2REL,  38,    6,
    "lbpl", P2REL,  42,    6,
    "lbra", P1REL,  22,     5,
    "lbrn", P2REL,  33,     5,
    "lbsr", P1REL,  23,     9,
    "lbvc", P2REL,  40,    6,
    "lbvs", P2REL,  41,    6,
    "lda",  GEN,    134,    2,
    "ldb",  GEN,    198,    2,
    "ldd",  LONGIMM,204,    3,
    "lds",  P2GEN,  206,    4,
    "ldu",  LONGIMM,206,    3,
    "ldx",  LONGIMM,142,    3,
    "ldy",  P2GEN,  142,    4,
    "leas", INDEXED,50,     2,
    "leau", INDEXED,51,     2,
    "leax", INDEXED,48,     2,
    "leay", INDEXED,49,     2,
    "lsl",  GRP2,   8,    4,
    "lsla", INH,    72,    2,
    "lslb", INH,    88,    2,
    "lsr",  GRP2,   4,    4,
    "lsra", INH,    68,    2,
    "lsrb", INH,    84,    2,
    "mul",  INH,    61,     11,
    "neg",  GRP2,   0,    4,
    "nega", INH,    64,    2,
    "negb", INH,    80,    2,
    "nop",  INH,    18,    2,
    "ora",  GEN,    138,    2,
    "orb",  GEN,    202,    2,
    "orcc", IMM,    26,     3,
    "pshs", RLIST,  52,     5,
    "pshu", RLIST,  54,     5,
    "puls", RLIST,  53,     5,
    "pulu", RLIST,  55,     5,
    "rol",  GRP2,   9,    4,
    "rola", INH,    73,    2,
    "rolb", INH,    89,    2,
    "ror",  GRP2,   6,    4,
    "rora", INH,    70,    2,
    "rorb", INH,    86,    2,
    "rti",  INH,    59,     15,
    "rts",  INH,    57,     5,
    "sbca", GEN,    130,    2,
    "sbcb", GEN,    194,    2,
    "sex",  INH,    29,    2,
    "sta",  NOIMM,  135,    2,
    "stb",  NOIMM,  199,    2,
    "std",  NOIMM,  205,    3,
    "sts",  P2NOIMM,207,    4,
    "stu",  NOIMM,  207,    3,
    "stx",  NOIMM,  143,    3,
    "sty",  P2NOIMM,143,    4,
    "suba", GEN,    128,    2,
    "subb", GEN,    192,    2,
    "subd", LONGIMM,131,    4,
    "swi",  INH,    63,     19,
    "swi2", P2INH,  63,     20,
    "swi3", P3INH,  63,     20,
    "sync", INH,    19,     4,
    "sys",  SYS,    0,      19,
    "tfr",  RTOR,   31,     6,
    "tst",  GRP2,   13,    4,
    "tsta", INH,    77,    2,
    "tstb", INH,    93,     2
};

struct oper pseudo[] = {
    "bsz",  PSEUDO, ZMB,    0,
    "end",  PSEUDO, NULL_OP,0,
    "equ",  PSEUDO, EQU,    0,
    "fcb",  PSEUDO, FCB,    0,
    "fcc",  PSEUDO, FCC,    0,
    "fdb",  PSEUDO, FDB,    0,
    "fill", PSEUDO, FILL,   0,
    "nam",  PSEUDO, NULL_OP,0,
    "name", PSEUDO, NULL_OP,0,
    "opt",  PSEUDO, OPT,    0,
    "org",  PSEUDO, ORG,    0,
    "pag",  PSEUDO, PAGE,   0,
    "page", PSEUDO, PAGE,   0,
    "rmb",  PSEUDO, RMB,    0,
    "spaceto",  PSEUDO, SPACETO, 0,
    "spc",  PSEUDO, NULL_OP,0,
    "ttl",  PSEUDO, NULL_OP,0,
    "zmb",  PSEUDO, ZMB,    0
};

unsigned long NMNE_SIZE = (sizeof(table)/ sizeof(struct oper));
unsigned long NPSE_SIZE = (sizeof(pseudo)/ sizeof(struct oper));

/*
 *      as ---  cross assembler main program
 *      This requries a very peculiar set of arguments:
 *      First come *all* the file names.
 *      Then comes the first option, with a dash
 *      Then come the, possibly multichar, options without dash.
 */
int main6809(int argc, char **argv)
{
        char    **np;
        char    *pc;
        FILE    *fopen();
        int     j = 0;

        fprintf(stdout, "i6809asm an 6809 Assembler. Updated by Favard Laurent for macOS (Xcode 16.2)\n");
        fprintf(stdout, "2012-2025\n");

        if(argc < 2) {
                printf("Usage: [files] -opt1 -opt2 ...\n"/*,argv[j]*/);
                printf("\n");
                printf("Where -opt can be:\n");
                printf("-l    listing output\n");
                printf("-bin  binary output\n");
                printf("-hxt  Hexa.text output\n");
            exit(1);
        }
        
        Argv = argv;
        initialize();

        while ( (j<argc) && (*argv[j] != '-') )
           j++;

        N_files = j-1;
        if (j < argc ) {
          argv[j]++;
          while (j<argc) {
             for (pc = argv[j]; *pc != 0; pc++)
               *pc = tolower( (unsigned char)*pc );

            printf( "%s\n", pc);

             if (strcmp(argv[j],"l")==0 || strcmp(argv[j],"-l")==0) {
                fprintf(stdout, "with listing output\n");
               Lflag = 1;
             }
             else if (strcmp(argv[j],"h")==0 || strcmp(argv[j],"-hxt")==0) {
                fprintf(stdout, "with hexadecimal text output\n");
               HXTflag = 1;
             }
             else if (strcmp(argv[j],"nol")==0 || strcmp(argv[j],"-nol")==0 ) {
               Lflag = 0;
             }
             else if (strcmp(argv[j],"c")==0)
               Cflag = 1;
             else if (strcmp(argv[j],"noc")==0)
               Cflag = 0;
             else if (strcmp(argv[j],"s")==0)
               Sflag = 1;
             else if (strcmp(argv[j],"cre")==0)
               Rflag = 1;
             else if (strcmp(argv[j],"bin")==0 || strcmp(argv[j],"-bin")==0) {
               Bflag = 1;
               fprintf(stdout, "with bin output");
            }
             else if (strcmp(argv[j],"s19")==0 || strcmp(argv[j],"-s19")==0) {
               Oflag = 1;
               fprintf(stdout, "with S19 output");
            }
             else if (strcmp(argv[j],"w")==0)
               Wflag = 1;
             else if (strcmp(argv[j],"now")==0)
               Wflag = 0;
             else {
                 printf( "%s", argv[j]);
               fatal("Unknown option");

             }
             j++;
             }
          }
        root = NULL;

        //    force these files
        //Lflag = 1;
        //Bflag = 1;

        Cfn = 0;
        np = argv;
        Line_num = 0; /* reset line number */
        while( ++Cfn <= N_files ) {

            ++np;
            printf("processing: %s\n", *np);

                if((Fd = fopen( *np,"r")) == NULL)
                        printf("Can't open %s\n",*np);
                else{
                        make_pass();
                        fclose(Fd);
                }
        }

        if( Err_count == 0 ) {
            Pass++;
            re_init();
            Cfn = 0;
            np = argv;
            Line_num = 0;
            open_files();               /* open all necessary files */
            while( ++Cfn <= N_files)
                    if((Fd = fopen( *++np,"r")) != NULL) {
                            make_pass();
                            fclose(Fd);
                    }
                    if (Sflag == 1) {
                       stable (root);
                    }
                    if (Rflag == 1) {
                       cross (root);
                    }

            if ( Oflag )
                fprintf(Objfil,"S9030000FC\n"); /* at least give a decent ending */
        }
        fprintf(stdout, "End, %d errors\n", Err_count);
        exit(Err_count);
}

void initialize()
{
        FILE    *fopen();
        int     i = 0;

#ifdef DEBUG
        printf("Initializing\n");
#endif
        Err_count = 0;
        Pc        = 0;
        Pc_ROMORG = 0;
        Pass      = 1;
        Lflag     = 0;
        Cflag     = 0;
        Ctotal    = 0;
        Sflag     = 0;
        Rflag     = 0;
        Bflag     = 0;
        Oflag     = 0;
        N_page    = 0;

        strcpy(Obj_name,Argv[1]); /* copy object file name into array */
        strcpy(Bin_name,Argv[1]); /* copy binary file name into array */
        strcpy(Lst_name,Argv[1]); /* copy list   file name into array */
        strcpy(Sym_name,Argv[1]); /* copy symbol file name into array */
        strcpy(Crf_name,Argv[1]); /* copy crf    file name into array */
        strcpy(Hexa_name,Argv[1]);

        do {
            if (Obj_name[i]=='.')
               Obj_name[i]=0;
            if (Bin_name[i]=='.')
               Bin_name[i]=0;
            if (Lst_name[i]=='.')
               Lst_name[i]=0;
            if (Sym_name[i]=='.')
               Sym_name[i]=0;
            if (Crf_name[i]=='.')
               Crf_name[i]=0;
            if (Hexa_name[i]=='.')
               Hexa_name[i]=0;
        }
        while (Crf_name[i++] != 0);
        fwdinit();      /* forward ref init */
        localinit();    /* target machine specific init. */
}

void re_init()
{
#ifdef DEBUG
        printf("Reinitializing\n");
#endif
        Pc      = 0;
        Pc_ROMORG = 0;
        E_total = 0;
        P_total = 0;
        Ctotal  = 0;
        N_page  = 0;
        fwdreinit();
}

void open_files()
{
        if(Oflag){
          strcat(Obj_name,".s19");  /* append .s19 to object file name. */
            if( (Objfil = fopen(Obj_name,"wt")) == NULL)
                fatal("Can't create object file");
        }

        if(Bflag){
          strcat(Bin_name,".bin");  /* append .bin to binary file name. */
            if( (Binfil = fopen(Bin_name,"wb")) == NULL)
                fatal("Can't create binary file");
        }

        if(HXTflag) {
          strcat(Hexa_name,".txt");  /* append .txt to binary file name. */
            if( (HXTfil = fopen(Hexa_name,"wb")) == NULL)
                fatal("Can't create hexa. text file");
        }

        if(Lflag){
          strcat(Lst_name,".lst");  /* append .lst to list file name. */
            if( (Lstfil = fopen(Lst_name,"wt")) == NULL)
                fatal("Can't create list file");
        }

        if(Sflag){
          strcat(Sym_name,".sym");  /* append .sym to symbol file name. */
            if( (Symfil = fopen(Sym_name,"wt")) == NULL)
                fatal("Can't create symbol file");
        }

        if(Rflag){
          strcat(Crf_name,".crf");  /* append .crf to cross reference file name. */
            if( (Crffil = fopen(Crf_name,"wt")) == NULL)
                fatal("Can't create cross reference file");
        }
}

void make_pass()
{
        char *pc;

        fprintf(stdout, "Pass %d\n",Pass);

        while( fgets(Line,MAXBUF,Fd) ){
                pc = Line+strlen(Line);  /* Past character. */
                if ( '\r'==pc[-2] ) { pc[-2]='\n';pc[-1]=0;}
                Line_num++;
                P_force = 0;    /* No force unless bytes emitted */
                N_page = 0;
                if(parse_line())
                        process();
                if(Pass == 2 && Lflag && !N_page)
                        print_line();
                P_total = 0;    /* reset byte count */
                Cycles = 0;     /* and per instruction cycle count */
                }
        f_record();
}


/*
 *      parse_line --- split input line into label, op and operand
 */
int parse_line()
{
        register char *pcfrm = Line;
        register char *pcto;
        char    *skip_white();

        if( *pcfrm == '*' || *pcfrm == '\n' ||
            *pcfrm == ';' )
                return(0);      /* a comment line */
        /***************************/
        pcto = Label;
        while( delim(*pcfrm)== NO )   *pcto++ = *pcfrm++;
        /* allow trailing : */
        if(*pcfrm == ':')   pcfrm++;
        *pcto = EOS;

        pcfrm = skip_white(pcfrm);

        /***************************/
        pcto = Op;
        while( delim(*pcfrm) == NO)   *pcto++ = mapdn(*pcfrm++);
        *pcto = EOS;

        pcfrm = skip_white(pcfrm);

        /***************************/
        pcto = Operand;  /* Can contain comment */
        while( *pcfrm != NEWLINE && *pcfrm != EOS )
        {
                *pcto++ = *pcfrm++;
        }
        *pcto = EOS;

#ifdef DEBUG
        printf("Label-%s-\n",Label);
        printf("Op----%s-\n",Op);
        printf("Operand-%s-\n",Operand);
#endif
        return(1);
}

/*
 *      process --- determine mnemonic class and act on it
 */

void process()
{
        register struct oper *i;
        struct oper *mne_look();
        char    *skip_white();

        Old_pc = Pc;            /* setup `old' program counter */
        Optr = Operand;         /* point to beginning of operand field */

    if ( Line_num == 232 ) {
        Line_num = 232;
    }

        if(*Op==EOS){           /* no mnemonic */
                if(*Label != EOS)
                        install(Label,Pc);
                }
        else if( (i = mne_look(Op))== NULL)
                error("Unrecognized Mnemonic");
        else if( i->class == PSEUDO )
                do_pseudo(i->opcode);
        else{
                if( *Label )install(Label,Pc);
                if(Cflag)Cycles = i->cycles;
                do_op(i->opcode,i->class);
                if(Cflag)Ctotal += Cycles;
                }

        /*if ( Wflag && 2 == Pass )
            if ( EOS!=*Optr ) {
                // Yes I'm ashamed f this hack, (AH)
                if ( ']' == *Optr ) Optr += 1;
                Optr = skip_white(Optr);
                if ( EOS!=*Optr && ';'!=*Optr ){
                   printf("<<<%s>>>\n", Optr);
                   warn("Comment doesnot start with comment symbol");
                   }
            }*/
}

