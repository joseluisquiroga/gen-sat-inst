

#ifndef BRAUN_MULTIPIER_H
#define BRAUN_MULTIPIER_H

#define BRAUNMULTIPLIER 1
#define TREEADDERMULTIPLIER 2
#define REDUCEDBRAUN 3
#define VERSION "$Revision: 1.10 $ $Date: 2001/08/23 10:43:58 $"

#ifdef __cplusplus
extern "C" {
#endif 

void print_size_restriction(FILE *outf, int w);
void print_even_restriction(FILE *outf, int w);
void print_odd_restriction(FILE *outf);
void print_in_restriction(FILE *outf, int width);
void printnotone(FILE *outfile, char *prefix, int width);
void printbitset(FILE *outf, char *set, char *prefix);

void printbraunmultiplier(FILE *outfile, int width);
void printaddertreemult(FILE *outfile, int width );
void printreducedbraunmultiplier(FILE *outfile, int width);

void set_choiceset(FILE *outf, int w, char *prefix);


#ifdef __cplusplus
}
#endif 


#endif // BRAUN_MULTIPIER_H
