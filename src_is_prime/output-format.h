#ifndef OUTPUT_FORMAT_H
#include <stdio.h>
#define OUTPUT_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif 


typedef struct _line_t {
  char *name;
  int is_not;
} line_t;


typedef void (*gate_print_f)(FILE *outf, line_t, int count, ...);
typedef void (*set_line_t)(FILE *outf, line_t);
typedef void (*set_equal_t)(FILE *outf, line_t, line_t);
typedef void (*initialize_t)(FILE *outf);
typedef void (*finalize_t)(FILE *outf);
typedef void (*set_choice_t)(FILE *outf, line_t);

typedef struct _output_format_t {
  char *name;
  gate_print_f 	and_pf;
  gate_print_f 	or_pf;
  gate_print_f 	xor_pf;
  set_line_t 	sl_f;
  set_equal_t 	se_f;
  initialize_t 	init_f;
  finalize_t 	end_f;
  set_choice_t 	choice_f;
} output_format_t;

extern gate_print_f print_or;
extern gate_print_f print_and;
extern gate_print_f print_xor;
extern set_line_t set_line;
extern set_equal_t set_equal;
extern set_choice_t set_choice;
extern finalize_t finalize_print;

extern output_format_t formats[];


char *get_output_format_name(int n);
int set_output_format(FILE *, int n);
void print_or_table(FILE *, line_t, line_t[], int);
void print_and_table(FILE *, line_t, line_t[], int);
void print_xor_table(FILE *, line_t, line_t[], int);


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

#endif


