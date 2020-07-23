/*
 * (C) Tuomo Pyhala
 */
#include <stdarg.h>
#include "output-format.h"

/*
 *
 * Bcsat
 *
 */

void bcsat_initialize(FILE *outfile) {
  fprintf(outfile, "BC1.0\n");
  fprintf(outfile, "false := F;\n");
}

void bcsat_finalize(FILE *outfile) {
}

void print_any(FILE *outf, line_t outline, char term_delim, char *inf_delim, char *eol_delim, char *neg_str, int count, va_list vl) {
  int eka = 1;
  int i;
  line_t iline;

  fprintf(outf, "%s %s ", outline.name, inf_delim);
  while(count--) {
    iline = va_arg(vl, line_t);
    if(eka) {
      fprintf(outf, "%s%s", iline.is_not ? neg_str :"", iline.name);
      eka = 0;
    }
    else {
      fprintf(outf, " %c %s%s", term_delim, iline.is_not ? neg_str :"", iline.name);
    }
  }
  fprintf(outf, "%s", eol_delim);
}

void bcsat_print_any(FILE *outf, line_t outline, char delim, int count, va_list vl) {
  print_any(outf, outline, delim, ":=", ";\n", "~", count, vl);
}

void bcsat_print_and(FILE *outf, line_t outline, int count, ...) {
  va_list vl;
  va_start(vl, count);
  bcsat_print_any(outf, outline, '&', count, vl);
  va_end(vl);
}

void bcsat_print_or(FILE *outf, line_t outline, int count, ...) {
  va_list vl;
  va_start(vl, count);
  
  bcsat_print_any(outf, outline, '|', count, vl);
  va_end(vl);

}

void bcsat_print_xor(FILE *outf, line_t outline, int count, ...) {
  va_list vl;
  va_start(vl, count);
  bcsat_print_any(outf, outline, '^', count, vl);
  va_end(vl);

}

void bcsat_set_line(FILE *outf, line_t line) {
  fprintf(outf, "ASSIGN %s%s;\n", line.is_not ? "~" : "", line.name);
}

void bcsat_set_equal(FILE *outf, line_t line1, line_t line2) {
  fprintf(outf, "%s := %s%s;\n", line1.name, line2.is_not ? "~" : "",  line2.name);
}

void bcsat_set_choice(FILE *outf, line_t l) {
}

/*
 *
 * Smodels
 *
 */

void smodels_initialize(FILE *outf) {
  fprintf(outf, ":- false.\n");
}

void smodels_finalize(FILE *outf) {
}

void smodels_print_and(FILE *outf, line_t outline, int count, ...) {
  va_list vl;
  va_start(vl, count);
  print_any(outf, outline, ',', ":-", ".\n", "not ", count, vl);
  va_end(vl);
}

void smodels_print_or(FILE *outf, line_t outline, int count, ...) {
  va_list vl;
  line_t iline;
  va_start(vl, count);
  
  while(count--) {
    iline = va_arg(vl, line_t);
    fprintf(outf, "%s :- %s%s.\n", outline.name, iline.name, iline.is_not ? "not " : "");
  }
}

void smodels_print_xor(FILE *outf, line_t outline, int count, ...) {
  line_t inline0, inline1;

  va_list vl;
  va_start(vl, count);
  inline0 = va_arg(vl, line_t);
  inline1 = va_arg(vl, line_t);
  fprintf(outf, "%s :- not %s, %s.\n", outline.name, inline0.name, 
	  inline1.name);
  fprintf(outf, "%s :- %s, not %s.\n", outline.name, inline0.name, 
	  inline1.name);
  // fprintf(outf, "%s :- 1{%s, %s}1.\n", outline.name, inline0.name, inline1.name);
  //  smodels_print_or(outf, outline, 2, inline0, inline1);
  //  fprintf(outf, "u :- %s, %s%s, %s%s, not u.\n", outline.name, (inline0.is_not) ? "not " : "", inline0.name,
  //	  (inline1.is_not) ? "not " : "", inline1.name);
  //fprintf(outf, "u :- %s, %s%s, %s%s, not u.\n", outline.name, (!inline0.is_not) ? "not" : "", inline0.name,
  //  (!inline1.is_not) ? "not " : "", inline1.name);
}

void smodels_set_line(FILE *outf, line_t line) {
  fprintf(outf, ":- %s%s.\n", line.is_not ? "" : "not ", line.name);
}

void smodels_set_equal(FILE *outf, line_t line1, line_t line2) {
  fprintf(outf, "%s :- %s.\n", line1.name, line2.name);
  fprintf(outf, "%s :- %s.\n", line2.name, line1.name);
}

void smodels_set_choice(FILE *outf, line_t l) {
  fprintf(outf, "%s :- not not_%s.\n", l.name, l.name);
  fprintf(outf, "not_%s :- not %s.\n", l.name, l.name);
}


/*
 *
 *
 *
 */

void print_large_gate(FILE *outf, gate_print_f f, line_t target, int count, 
		 line_t lines[])
{
  static int no = 0;
  int i;
  int level = 0;
  char s0[256], s1[256], s2[256], s3[256]; 
  line_t l0 = {s0, 0};
  line_t l1 = {s1, 0};
  line_t l2 = {s2, 0};
  line_t l3 = {s3, 0};
  
  if(count == 1) {
    set_equal(outf, target, lines[0]);
  }

  for(i=0; i<count/2; i++) {
    if(count> 3) {
      sprintf(s0, "print_large_%d_%d_%d", no, level, i);
    } else {
      l0 = target;
    }
    if(i+1 == count/2 && count%2 == 1) {
      f(outf, l0, 3, lines[i*2], lines[i*2+1], lines[i*2+2]);
    } else {
      f(outf, l0, 2, lines[i*2], lines[i*2+1]);
    }
  }
  if(count > 3) {
    do {
      count = count /2;
      level++;
      for(i=0; i<count/2; i++) {
	sprintf(s0, "print_large_%d_%d_%d", no, level-1, i*2);
	sprintf(s1, "print_large_%d_%d_%d", no, level-1, i*2+1);
	sprintf(s2, "print_large_%d_%d_%d", no, level-1, i*2+2);
	if(count/2 > 1) {
	  sprintf(s3, "print_large_%d_%d_%d", no, level, i);
	} else {
	  l3 = target;
	}
	if(i+1 == count/2 && count%2 == 1) {
	  f(outf, l3, 3, l0, l1 ,l2);
	} else {
	  f(outf, l3, 2, l0, l1);
	}
      }
    } while(count>1);
  }
  no++;
}

void print_and_table(FILE *outf, line_t target, line_t lines[], int count)
{
  print_large_gate(outf, print_and, target, count, lines);
}

void print_xor_table(FILE *outf, line_t target, line_t lines[], int count)
{
  print_large_gate(outf, print_and, target, count, lines);
}

void print_or_table(FILE *outf, line_t target, line_t lines[], int count)
{
  print_large_gate(outf, print_and, target, count, lines);
}



/*
 * Set output format to use
 *
 */


output_format_t formats[] = {
  {"dimacs", dimacs_print_and, dimacs_print_or, dimacs_print_xor, dimacs_set_line, dimacs_set_equal, 
dimacs_initialize, dimacs_finalize, dimacs_set_choice},
  {"bcsat", bcsat_print_and, bcsat_print_or, bcsat_print_xor, bcsat_set_line, bcsat_set_equal, 
bcsat_initialize, bcsat_finalize, bcsat_set_choice},
  {"smodel", smodels_print_and, smodels_print_or, smodels_print_xor, smodels_set_line, smodels_set_equal, 
smodels_initialize, smodels_finalize, smodels_set_choice},
  {NULL, NULL, NULL, NULL, NULL, NULL}
};

gate_print_f print_or;
gate_print_f print_and;
gate_print_f print_xor;
set_line_t set_line;
set_equal_t set_equal;
set_choice_t set_choice;
finalize_t finalize_print;

/*
char *get_output_format_name(int n) {
  int i;
  for(i=0; i<n && formats[i].name!=NULL;i++);
  return formats[i].name;
}
*/

int set_output_format(FILE *outf, int n) {
  int i;
  for(i=0; i<n && formats[i].name!=NULL;i++);
  if(formats[i].name != NULL) {
	//fprintf(stderr, "outputformat == %s\n", formats[i].name);

    print_xor = formats[i].xor_pf;
    print_and = formats[i].and_pf;
    print_or = formats[i].or_pf;
    set_line = formats[i].sl_f;
    set_equal = formats[i].se_f;
    finalize_print = formats[i].end_f;
    set_choice = formats[i].choice_f;

    formats[i].init_f(outf);
    return 0;
  }
  return -1;
}


