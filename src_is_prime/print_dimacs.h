

#ifndef PRINT_DIMACS_H
#define PRINT_DIMACS_H

#ifdef __cplusplus
extern "C" {
#endif 

void dimacs_initialize(FILE *outfile);
void dimacs_finalize(FILE *outfile);
void dimacs_print_and(FILE *outf, line_t outline, int count, ...);
void dimacs_print_or(FILE *outf, line_t outline, int count, ...);
void dimacs_print_xor(FILE *outf, line_t outline, int count, ...);
void dimacs_set_line(FILE *outf, line_t line);
void dimacs_set_equal(FILE *outf, line_t line1, line_t line2);
void dimacs_set_choice(FILE *outf, line_t l);

#ifdef __cplusplus
}
#endif 

#endif // PRINT_DIMACS_H
