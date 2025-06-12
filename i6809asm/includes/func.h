//
//  func.h
//  i6809asm
//
//  Created by Laurent Favard on 04/06/2025.
//  Copyright © 2025 Laurent. All rights reserved.
//

#ifndef     __GLOABAL_FUNC__
#define     __GLOABAL_FUNC__    __GLOABAL_FUNC__

int main6809(int argc, char **argv);

void initialize();
void process();
int parse_line();
void make_pass();
void open_files();
void re_init();

void process();


void localinit();
void do_op(int opcode,int class);
void do_indexed(int op);
void abd_index(int pbyte);
void f_record();
void print_line();
void do_gen(int op,int mode);
void stable(struct nlist *ptr);
void cross(struct nlist *point);

/**
 * from util.c
 */

void fatal(char *str);
void error(char *str);
void errors(char *msg, char *str);
void warn(char *str);
int delim(char c);
char *skip_white(char *ptr);
void eword(int wd);
int emit(int byte);
void f_record();
void hexout(int byte);
void binout(int byte);
void hxtout(int byte);
void print_line();
int any(char c, char *str);
char mapdn(char c);
int lobyte(int i);
int hibyte(int i);
int head( char *str1, char *str2 );
int alpha(char c);
int alphan(char c);
int white(char c);
char *alloc(int nbytes);

/**
 *  From ffwd.c
 */


void fwdmark();
void fwdnext();
void fwdreinit();
void fwdinit();

/**
 *  From eval.c
 */

int eval();
int get_term();
int is_op(char c);

/**
 *  From do9.c
 */

void localinit();
void do_op(int opcode,int class);

void do_gen(int op, int mode);
void do_indexed(int op);

void abd_index(int pbyte);
int rtype(int r);
int set_mode();
int regnum();
int rtype(int r);


/**
 *  From symtab.c
 */

int install(char *str, int val);
struct nlist *lookup(char *name);
struct oper *mne_look(char *str);

/**
 From pseudo.c
 */
void do_pseudo(int op);

#endif

