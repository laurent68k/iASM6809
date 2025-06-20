#include <stdio.h>

#include "func.h"
#include "as.h"
#include "directives.h"
#include "refvars.h"

/*
 *      do_pseudo --- do pseudo op processing
 */
void do_pseudo(int op)
//int op; /* which op */
{
        char    fccdelim;
        int     j;
        int     fill;
        char    *skip_white();

        if( op != EQU && *Label )
                install(Label,Pc);

        P_force++;
        switch(op){
                case RMB:                       /* reserve memory bytes */
                        if( eval() ){
                                Pc +=  Result;
                                f_record();     /* flush out bytes */
                                }
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case ZMB:                       /* zero memory bytes */
                        if( eval() )
                                while( Result-- )
                                        emit(0);
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case FILL:                      /* fill memory with constant */
                        eval();
                        fill = Result;
                        if( *Optr++ != ',' )
                                error("Bad fill");
                        else{
                                Optr = skip_white(Optr);
                                eval();
                                while( Result-- )
                                        emit(fill);
                                }
                        break;
                case FCB:                       /* form constant byte(s) */
                        do{
                                Optr = skip_white(Optr);
                                eval();
                                if( Result > 0xFF ){
                                        if(!Force_byte)
                                                warn("Value truncated");
                                        Result = lobyte(Result);
                                        }
                                emit(Result);
                        }while( *Optr++ == ',' );
                        Optr -= 1;
                        break;
                case FDB:                       /* form double byte(s) */
                        do{
                                Optr = skip_white(Optr);
                                eval();
                                eword(Result);
                        }while( *Optr++ == ',' );
                        Optr -= 1;
                        break;
                case FCC:                       /* form constant characters */
                        if(*Operand==EOS)
                                break;
                        fccdelim = *Optr++;
						while( *Optr != EOS && *Optr != fccdelim) {

							//	special characters to emit
							if( *Optr == '\\' ) {
								Optr++;
								//	octal representation
								if( *Optr == '0' ) {
									Optr++;
									{										
										int value = 0;
										while(any(*Optr,"01234567") ) {

											value += (value * 7 ) + (*Optr) - '0';
											Optr++;
										}
										emit(value);
									}
								}
								else if( *Optr == '$' ) {

									Optr++;
									{
										int value = 0;
										while(any(*Optr,"0123456789ABCDEF")) {

											int tmp = 0;
											if( (*Optr) >= '0' && (*Optr) <='9' ) 
												tmp = (*Optr) - '0';
											if( (*Optr) >= 'A' && (*Optr) <= 'F' ) 
												tmp = (*Optr) - 'A' + 10;
											value = (value << 4 ) + tmp;

											Optr++;
										}
										emit(value);
									}
								}
								else {
									switch (*Optr) {

										case 'n':	emit(0x0A);
													break;

										case 'r':	emit(0x0D);
													break;

										case 't':	emit(0x09);
													break;

										default:	emit(*Optr++);
													break;
									}
								}
							}
							else {
								emit(*Optr++);
							}
						}
                        if(*Optr == fccdelim)
                                Optr++;
                        else
                                error("Missing Delimiter");
                        break;
                case ORG:                       /* origin */
                        if( eval() ) {
							f_record();     /* flush out any bytes */
							;fprintf(stdout, "ORG $%04X\n", Result );
							/*if( Pass == 2 && Pc != 0x0000 && Result != 0x0000 ) {
								
								fprintf(stdout, "Fill with 0xFF between 0x%04X and 0x%04X\n", Pc, Result );
								{
									int index = 0;
									for( index = Result-1; index >= Pc; index-- ) {
										binout( 0xFF );
									}
								}
							}*/
                            Old_pc = Pc = Result;
                            //f_record();     /* flush out any bytes */
                        }
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case SPACETO:                       /* ROM Origin */
                        if( eval() ) {							
							f_record();     /* flush out any bytes */						
							if( Pass == 2 && Result > Pc ) {
								fprintf(stdout, "SPACETO $%04X\n", Result );
								fprintf(stdout, "Fill with 0xFF between last PC=0x%04X to here=0x%04X\n", Pc, Result );
								{
									int index = 0;
									for( index = Result-1; index >= Pc; index-- ) {
										binout( 0xFF );
										if( HXTflag) {
										  hxtout(0xFF);
										}
									}
								}
							}
                            //Pc_ROMORG = Result;
                            //f_record();     /* flush out any bytes */
                        }
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case EQU:                       /* equate */
                        if(*Label==EOS){
                                error("EQU requires label");
                                break;
                                }
                        if( eval() ){
                                install(Label,Result);
                                Old_pc = Result;        /* override normal */
                                }
                        else
                                error("Undefined Operand during Pass One");
                        break;
                case OPT:                       /* assembler option */
                        P_force=0;
                        if( head(Operand,"l") )
                                Lflag=1;
                        else if( head(Operand,"nol") )
                                Lflag=0;
                        else if( head(Operand,"c") ){
                                Cflag=1;
                                Ctotal=0;
                                }
                        else if( head(Operand,"noc") )
                                Cflag=0;
                        else if( head(Operand,"contc") )
                                Cflag=1;
                        else if ( head(Operand,"s") )
                                Sflag = 1;
                        else if ( head(Operand,"cre") )
                                Rflag = 1;
                        else if ( head(Operand,"bin") )
                                Bflag = 1;
                        else if ( head(Operand,"s19") )
                                Oflag = 1;
                        else
                                errors("Unrecognized OPT", Operand);
                        break;
                case PAGE:                      /* go to a new page */
                        P_force=0;
                        N_page = 1;
                        if (Pass == 2 )
                         if (Lflag)
                          {
                           fprintf (Lstfil,"\f");
                           fprintf (Lstfil,"%-10s",Argv[Cfn]);
                           fprintf (Lstfil,"                                   ");
                           fprintf (Lstfil,"page %3d\n",Page_num++);
                          }
                        break;
                case NULL_OP:                   /* ignored psuedo ops */
                        P_force=0;
                        break;
                default:
                        fatal("Pseudo error");
                }
}
