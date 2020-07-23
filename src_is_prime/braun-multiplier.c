/*
 *
 * Generates boolean circuits for multiplication
 * (C) Tuomo Pyhala
 *
 * $Log: braun-multiplier.c,v $
 * Revision 1.10  2001/08/23 10:43:58  tpyhala
 * Introduced -m 8 optimization and reduced braun multiplier circuit
 *
 * Revision 1.9  2001/08/17 08:02:00  tpyhala
 * this version does suck - it doesnt work
 *
 * Revision 1.8  2001/08/14 08:07:07  tpyhala
 * Added some optimizations.
 *
 * Revision 1.7  2001/07/12 11:58:34  tpyhala
 * Corrected some typos in printnotone.
 *
 * Revision 1.6  2001/07/10 13:53:31  tpyhala
 * Modified -1 switch to work correctly with new output-format code.
 *
 * Revision 1.5  2001/07/09 13:57:01  tpyhala
 * Implemented framework for different output formats. Smodels and bcsat
 * formats were also implemented using framework. Scripts include features
 * for parsing smodels output.
 *
 * Revision 1.4  2001/07/04 07:39:22  tpyhala
 * Fixed up the revision string (again).
 *
 * Revision 1.3  2001/07/04 07:37:42  tpyhala
 * Fixed up the revision tags.
 *
 * Revision 1.2  2001/07/04 07:35:48  tpyhala
 * Added revision tags.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "adder.h"
#include "braun-multiplier.h"
#include "output-format.h"

int calc_exp2(int i) {
  int ret = 1;

  for(;i>0; i--)
    ret *= 2;
  
  return ret;
}

void printfulladder(FILE *outfile, char *inline1, char *inline2, 
		   char *inline3, char *outline, char *carryout)
{
  static long int id = 0;
  line_t in1 = {inline1, 0};
  line_t in2 = {inline2, 0};
  line_t in3 = {inline3, 0};
  line_t out = {outline, 0};
  line_t carry = {carryout, 0};
  char xxorstring[256], andstring[256], andstring2[256];
  char str1[256], str2[256], str3[256];

  line_t xxor = {xxorstring, 0};
  line_t and = {andstring, 0};
  line_t and2 = {andstring2, 0};
  line_t l1 ={str1, 0};
  line_t l2 ={str2, 0};
  line_t l3 ={str3, 0};
  sprintf(xxorstring, "xxor_1_%ld", id);
  sprintf(andstring, "and_1_%ld", id);
  sprintf(andstring2, "and_2_%ld", id);
  
  //  fprintf(stderr, "%s + %s + %s = %s * 2 + %s\n", inline1, inline2, inline3, carryout, outline);
  
  //  array = {in1, in2, term};

  print_xor(outfile, xxor, 2, in1, in2);
  print_and(outfile, and, 2, in1, in2);
  print_xor(outfile, out, 2, xxor, in3);
  print_and(outfile, and2, 2, xxor, in3);
  print_or(outfile, carry, 2, and, and2);

#ifdef FULLADDER_BACKWARDS_OPTIMIZATION  
  /* Optimize for backwards propagation */
  
  sprintf(l1.name, "out_or_%d", id);
  sprintf(l2.name, "in_or_%d", id);
  sprintf(l3.name, "in_out_or_xxor_%d", id);
  print_or(outfile, l1, 2, carry, out);
  print_or(outfile, l2, 3, in1, in2, in3);
  print_xor(outfile, l3, 2, l1, l2); 
  l3.is_not = 1;
  set_line(outfile, l3);
  l3.is_not = 0;

  /* Optimize more for backward propagation */

  sprintf(l1.name, "out_and_%d", id);
  sprintf(l2.name, "in_and_%d", id);
  sprintf(l3.name, "in_out_and_xxor_%d", id);
  print_or(outfile, l1, 2, carry, out);
  print_or(outfile, l2, 3, in1, in2, in3);
  print_xor(outfile, l3, 2, l1, l2); 
  l3.is_not = 1;
  set_line(outfile, l3);
  l3.is_not = 0;
#endif
  /*fprintf(outfile, "xxor_1_%d := %s ^ %s;\n",id , inline1, inline2);
  fprintf(outfile, "and_1_%d := %s & %s;\n", id, inline1, inline2);
  fprintf(outfile, "%s := xxor_1_%d ^ %s;\n", outline, id, inline3);
  fprintf(outfile, "and_2_%d := xxor_1_%d & %s;\n", id, id, inline3);
  fprintf(outfile, "%s := and_1_%d | and_2_%d;\n", carryout, id, id); */
  id++;
}

void printbitset(FILE *outf, char *set, char *prefix) {
  //  int first = 1;
  int i;
  char str[256];
  long ssz = 0;
  char s_ii = 0;
  line_t line_to_set = {str, 0};
  
  if(set != NULL) {
    //    fprintf(outf, "SET{");
    for(i=0; set[i]; i++) {
	ssz = strlen(set);
	s_ii = set[ssz-i-1];
      /* switch(s_ii) { */
      switch(set[strlen(set)-i-1]) { 
      case '1':
	//      if(!first) 
	//  fprintf(outf, ",");
	//else
	//  first = 0;
	//fprintf(outf, "%s%d", prefix, i);
	sprintf(str, "%s%d", prefix, i);
	line_to_set.is_not = 0;
	set_line(outf, line_to_set);
        break;
      case '0':
	//          if(!first)
	///  fprintf(outf, ",");
	//else 
	//  first = 0;
	///fprintf(outf, "~%s%d", prefix, i);
	sprintf(str, "%s%d", prefix, i);
	line_to_set.is_not = 1;
	set_line(outf, line_to_set);
        break;
      case '?':
          break;
      default:
        fprintf(stderr, "Syntax error while processing bitset\n");
        break;
      }
    }
    //    fprintf(outf, "};\n");
  }
}

void printandgates(FILE *outfile, int awidth, int bwidth) {
  int i,j;
  char str1[256], str2[256], str3[256];
  line_t l1 = {str1, 0};
  line_t l2 = {str2, 0};
  line_t l3 = {str3, 0};
  for(i=0;i<awidth; i++) {
    for(j=0; j<bwidth; j++) {
      sprintf(str1, "a%d_and_b%d", i, j);
      sprintf(str2, "a%d", i);
      sprintf(str3, "b%d", j);
      print_and(outfile, l1, 2, l2, l3);
      //      fprintf(outfile, "a%d_and_b%d := a%d & b%d;\n", i,j,i,j);
    }
  }
}

void printbraunmultiplier(FILE *outfile, int width)
{
  char *tmpz1, *tmpz2, *tmpz3, *tmpz4, *tmpz5; 
  int i, j;
  int jlimit;
  char s[256], s2[256];
  line_t l = {s, 0};
  line_t l2 = {s2, 0};

  tmpz1 = malloc(2*width+30);
  tmpz2 = malloc(2*width+30);
  tmpz3 = malloc(2*width+30);
  tmpz4 = malloc(2*width+30);
  tmpz5 = malloc(2*width+30);

  printandgates(outfile, width, width);
  
  i=1;
  for(j=0;j<width-1; j++) {
      sprintf(tmpz1, "a%d_and_b%d", j+1, i-1);
      sprintf(tmpz2, "a%d_and_b%d", j, i);
      sprintf(tmpz3, "false");
      if(j==0) {
        sprintf(tmpz4, "product%d", i);
      }
      else {
        sprintf(tmpz4, "adder_%d_%d_out", j, i);
      }
      sprintf(tmpz5, "adder_%d_%d_carryout", j, i);
      printfulladder(outfile, tmpz1, tmpz2, tmpz3, tmpz4, tmpz5);
  }

  for(i=2;i<width; i++) {
    for(j=0;j<width-1; j++) {
      sprintf(tmpz1, "a%d_and_b%d", j, i);
      if(j<width-2) {
        sprintf(tmpz2, "adder_%d_%d_out", j+1, i-1);  
      }
      else {
        sprintf(tmpz2, "a%d_and_b%d", width-1, i-1);
      }
      sprintf(tmpz3, "adder_%d_%d_carryout", j, i-1);
      if(j==0) {
        sprintf(tmpz4, "product%d", i);
      }
      else {
        sprintf(tmpz4, "adder_%d_%d_out", j, i);
      }
      sprintf(tmpz5, "adder_%d_%d_carryout", j, i);

      printfulladder(outfile, tmpz1, tmpz2, tmpz3, tmpz4, tmpz5);
    }
  }
  
  //  fprintf(outfile, "product0 := a0_and_b0;\n");
  sprintf(l.name, "product0");
  sprintf(l2.name, "a0_and_b0");
  set_equal(outfile, l, l2);


  for(i=0; i<width-1; i++) {
    if(i<width-2) {
      sprintf(tmpz1, "adder_%d_%d_out", i+1, width-1);
    }
    else {
      sprintf(tmpz1, "a%d_and_b%d", width-1, width-1);
    }
    sprintf(tmpz2, "adder_%d_%d_carryout", i, width-1);
    if(i>0) {
      sprintf(tmpz3, "finaladder_%d_carryout", i-1);
    }
    else {
      sprintf(tmpz3, "false");
    }
    sprintf(tmpz4, "product%d", i+width);
    if(i<width-2) {
      sprintf(tmpz5, "finaladder_%d_carryout", i);
    }
    else {
      sprintf(tmpz5, "product%d", 2*width-1);
    }
    printfulladder(outfile, tmpz1, tmpz2, tmpz3, tmpz4, tmpz5);
  }
}


void printreducedbraunmultiplier(FILE *outfile, int width)
{
  char *tmpz1, *tmpz2, *tmpz3, *tmpz4, *tmpz5; 
  int i, j;
  int jlimit;
  char s[256], s2[256];
  line_t l = {s, 0};
  line_t l2 = {s2, 0};
  line_t in;

  tmpz1 = malloc(2*width+30);
  tmpz2 = malloc(2*width+30);
  tmpz3 = malloc(2*width+30);
  tmpz4 = malloc(2*width+30);
  tmpz5 = malloc(2*width+30);

  printandgates(outfile, width/2 + width%2, width);
  
  i=1;
  jlimit = width/2 + width%2 -1;
  for(j=0;j<jlimit; j++) {
      sprintf(tmpz1, "a%d_and_b%d", j+1, i-1);
      sprintf(tmpz2, "a%d_and_b%d", j, i);
      sprintf(tmpz3, "false");
      if(j==0) {
	sprintf(tmpz4, "product%d", i);
      }
      else {
	sprintf(tmpz4, "adder_%d_%d_out", j, i);
      }
      sprintf(tmpz5, "adder_%d_%d_carryout", j, i);
      printfulladder(outfile, tmpz1, tmpz2, tmpz3, tmpz4, tmpz5);
  }

  for(i=2;i<width; i++) {
    jlimit = width/2 + width%2 -1;
    for(j=0;j<jlimit; j++) {
      sprintf(tmpz1, "a%d_and_b%d", j, i);
      if(j<jlimit-1) {
	sprintf(tmpz2, "adder_%d_%d_out", j+1, i-1);  
      }
      else {
	sprintf(tmpz2, "a%d_and_b%d", j+1, i-1);
      }
      sprintf(tmpz3, "adder_%d_%d_carryout", j, i-1);
      if(j==0) {
	sprintf(tmpz4, "product%d", i);
      }
      else {
	sprintf(tmpz4, "adder_%d_%d_out", j, i);
      }
      sprintf(tmpz5, "adder_%d_%d_carryout", j, i);
      if(i+j<width)
	printfulladder(outfile, tmpz1, tmpz2, tmpz3, tmpz4, tmpz5);
      else {
	in.is_not = 1;
	in.name = tmpz2;
	if(j>=jlimit-1) 
	  set_line(outfile, in);
	in.name = tmpz3;
	set_line(outfile, in);
	in.name = tmpz1;
	set_line(outfile, in);
	for(j++;j<jlimit;j++) {
	  sprintf(tmpz1, "a%d_and_b%d", j, i);
	  set_line(outfile, in);
	}	
	sprintf(tmpz1, "a%d_and_b%d", j, i-1);
	set_line(outfile, in);
	  
	break;
      }
    }
  }
  
  //  fprintf(outfile, "product0 := a0_and_b0;\n");
  sprintf(l.name, "product0");
  sprintf(l2.name, "a0_and_b0");
  set_equal(outfile, l, l2);
  /*jlimit = width/2 + width%2;
  for(i=0; i<jlimit-1; i++) {
    if(i<jlimit-2) {
      sprintf(tmpz1, "adder_%d_%d_out", i+1, width-1);
    }
    else {
      sprintf(tmpz1, "a%d_and_b%d", jlimit-1, width-1);
    }
    sprintf(tmpz2, "adder_%d_%d_carryout", i, width-1);
    if(i>0) {
      sprintf(tmpz3, "finaladder_%d_carryout", i-1);
    }
    else {
      sprintf(tmpz3, "false");
    }
    sprintf(tmpz4, "product%d", i+width);
    if(i<jlimit-2) {
      sprintf(tmpz5, "finaladder_%d_carryout", i);
    }
    else {
      sprintf(tmpz5, "product%d", i+width+1);
    }
    printfulladder(outfile, tmpz1, tmpz2, tmpz3, tmpz4, tmpz5);
    }*/
}

void printaddertreemult(FILE *outfile, int width ) {
  int i, j;
  int adderwidth;
  int oldadderwidth;
  int level = 0;
  char **inlines;
  char prefix[128];
  int curwidth;
  int oddadderlevel, oddadderindex, oddadderwidth;
  int printextraadder = 0;
  char str1[256], str2[256];
  line_t l1 = {str1, 0};
  line_t l2 = {str2, 0};

  oddadderlevel = -1;
  
  curwidth = width /2;
  adderwidth = width+1;

  printandgates(outfile, width, width);
  
  if(width>1) {
    inlines = malloc(sizeof(char*)*8*(width+1));
    for(j=0; j<8*(width+1); j++) 
      inlines[j] = malloc(sizeof(char)*(100+width));
    for(i=0;i<curwidth;i++) {
      sprintf(prefix, "adder_0_%d", i);
      for(j=0; j<width; j++) {
	sprintf(inlines[j], "a%d_and_b%d", j, i*2);
	/* Empty input in the end of first input and in the beginning of 
	 * the second
	 */
	sprintf(inlines[j+width+2], "a%d_and_b%d", j, i*2+1);
      }
      sprintf(inlines[width], "false");
      sprintf(inlines[width+1], "false");

      printadder(outfile, width+1, inlines, prefix);
    }
    if(width%2 == 1) {
      for(j=0; j<width;j++) {
	sprintf(str1, "adder_0_%d_sum%d", i, j);
	sprintf(str2, "a%d_and_b%d", j, i*2);
	l1.is_not = l2.is_not = 0;
	set_equal(outfile, l1,l2);
      }
      /*      fprintf(outfile, "adder_0_%d_sum%d := a%d_and_b%d;\n", i, j, j, i*2); */
      sprintf(str1, "adder_0_%d_sum%d", i, j);
      sprintf(str2, "false");
      set_equal(outfile, l1,l2);
      sprintf(str1, "adder_0_%d_carryout", i);
      set_equal(outfile, l1, l2);
      curwidth++;
      /* oddadderlevel = 0;
	 oddadderindex = i;
	 oddadderwidth = adderwidth; */
    }


    while(curwidth>1 || oddadderlevel != -1) {
    //    do{ 
      if((curwidth % 2) != 0) {
	if(oddadderlevel == -1) {
	  oddadderlevel = level;
	  oddadderindex = curwidth -1;
	  oddadderwidth = adderwidth;
	}
	else {
	  printextraadder = 1;
	}
      }
      level++;
      curwidth /= 2;

      oldadderwidth = adderwidth;
      adderwidth++;
      adderwidth += calc_exp2(level);
      for(i=0; i<curwidth; i++) {
	sprintf(prefix, "adder_%d_%d", level, i);
	for(j=0; j<oldadderwidth; j++) {
	  sprintf(inlines[j], "adder_%d_%d_sum%d", level-1, i*2, j);
	  sprintf(inlines[2*adderwidth-oldadderwidth-1+j], "adder_%d_%d_sum%d", level-1, i*2+1, j);
	}
	sprintf(inlines[j], "adder_%d_%d_carryout", level-1, i*2);
	sprintf(inlines[2*adderwidth-1], "adder_%d_%d_carryout", level-1, i*2+1);
	for(j++;j<adderwidth; j++) {
	  sprintf(inlines[j], "false");
	  sprintf(inlines[adderwidth+j-oldadderwidth-1], "false");
	}
	printadder(outfile, adderwidth, inlines, prefix);
      }
      /* When program has encountered two levels with odd amount of adders,
       * it creates adder to add these together.
       *
       * 
       *
       */
      if(printextraadder) {
	curwidth++;
	sprintf(prefix, "adder_%d_%d", level, i);
	for(j=0; j<oldadderwidth; j++) {
	  sprintf(inlines[j], "adder_%d_%d_sum%d", level-1, i*2, j);
	}
	for(; j<adderwidth*2; j++) {
	  if(j<(2*adderwidth-oldadderwidth+oddadderwidth-1) && j>=(2*adderwidth-oldadderwidth-1)) {	     
		sprintf(inlines[j], "adder_%d_%d_sum%d", oddadderlevel, oddadderindex, j-2*adderwidth+oldadderwidth+1);
	  }
	  else {
	    if(j==2*adderwidth-oldadderwidth+oddadderwidth-1) {
	      sprintf(inlines[j], "adder_%d_%d_carryout", oddadderlevel, oddadderindex);
	    }
	    else {
	      sprintf(inlines[j], "false");
	    }
	  }
	  printextraadder=0;
	}
	printadder(outfile, adderwidth, inlines, prefix);
	oddadderlevel = -1;
      }
    } 


    for(i=0; i<2*width;i++) {
      sprintf(str1, "product%d", i);
      sprintf(str2, "adder_%d_%d_sum%d", level, 0, i);
      set_equal(outfile, l1, l2);
      //      fprintf(outfile, "product%d := adder_%d_%d_sum%d;\n", i, level, 0, i);
    }
    //fprintf(outfile, "SET{");
    for(i=2*width; i<adderwidth; i++) {
      sprintf(str1, "adder_%d_%d_sum%d", level, 0, i);
      l1.is_not = 1;
      set_line(outfile, l1);
      //      fprintf(outfile, "~adder_%d_%d_sum%d,\n", level, 0, i);
    }
    
    sprintf(str1, "adder_%d_%d_carryout", level, 0);
    l1.is_not = 1;
    set_line(outfile, l1);
  }	     
}

void printwallacetree(FILE *outfile, int width) {
  
}

void printnotone(FILE *outfile, char *prefix, int width) 
{
  int i;
  int level = 0;
  int oddindex = -1;
  int oddlevel = -1;
  int printextra = 0;
  char s1[256], s2[256], s3[256];
  line_t l1 = {s1, 0};
  line_t l2 = {s2, 0};
  line_t l3 = {s3, 0};
  line_t false_line = {"false", 0};

  while(width != 1 || oddindex != -1) {
    if(width%2) {
      if(oddindex == -1) {
	oddlevel = level;
	oddindex = width-1;
      } else {
	printextra = 1;
      }
    }
    width /= 2;
    
    for(i=0; i<width; i++) {
      if(level != 0) {
	sprintf(s1, "notone_%s_and_%d_%d", prefix, level-1, 2*i);
	sprintf(s2, "notone_%s_and_%d_%d", prefix, level-1, 2*i+1);
	l1.is_not = 0;
	l2.is_not = 0;
      } else {
	sprintf(s1, "%s%d", prefix, 2*i);
	sprintf(s2, "%s%d", prefix, 2*i+1);
	if(level == 0 && i == 0) {
	  l1.is_not = 0;
	}
	else {
	  l1.is_not = 1;
	}
	l2.is_not = 1;
      }
      sprintf(s3, "notone_%s_and_%d_%d", prefix, level, i);
      print_and(outfile, l3, 2, l1, l2);
    }
    if(printextra) {
      if(oddlevel ==0) {
	sprintf(s1, "%s%d", prefix, oddindex);
	l1.is_not = 1;
      }
      else {
	sprintf(s1, "notone_%s_and_%d_%d", prefix, oddlevel-1, oddindex); 
	l1.is_not = 0;
      }
      sprintf(s2, "notone_%s_and_%d_%d", prefix, level-1, 2*i);
      l2.is_not = 0;
      sprintf(s3, "notone_%s_and_%d_%d", prefix, level, i);
      print_and(outfile, l3, 2, l1, l2);
      width++;
      printextra = 0;
      oddindex = oddlevel = -1;
    }
    level++;
  }
  l3.is_not = 1;
  set_line(outfile, l3);
  /*  sprintf(s3, "notone_%s", prefix);
  sprintf(s
  fprintf(outfile, "notone_%s := %s0", prefix, prefix);
  for(i=1; i<width; i++) {
    fprintf(outfile, " & ~%s%d", prefix, i);
  }
  fprintf(outfile, ";\nSET{~notone_%s};\n", prefix); */
}

void set_choiceset(FILE *outf, int w, char *prefix) {
  char s[256];
  line_t l = {s, 0};
  for(w--;!(w<0);w--) {
    sprintf(s, "%s%d", prefix, w);
    set_choice(outf, l);
  }
} 

/*
 * Set rules for odd products
 *
 * a0 :- product0.
 * b0 :- product0.
 */

void print_odd_restriction(FILE *outf) {
  line_t odd_r_la = {"odd_restrictiona", 0};
  line_t odd_r_lb = {"odd_restrictionb", 0};
  line_t a0 = {"a0", 0};
  line_t b0 = {"b0", 0};
  line_t not_p0 = {"product0", 1};
  print_or(outf, odd_r_la, 2, not_p0, a0);
  print_or(outf, odd_r_lb, 2, not_p0, b0);
  set_line(outf, odd_r_la);
  set_line(outf, odd_r_lb);
}

/*
 * Set rule for even product
 *
 * ~product0 -> a==2
 *
 */

void print_even_restriction(FILE *outf, int w) {
  line_t a[w];
  line_t even_restriction = {"even_restriction", 0};
  line_t a_equals_2 = {"a_equals_2", 0};
  line_t product0 = {"product0", 0};
  int i;

  for(i=0; i<w; i++) {
    a[i].name = malloc(sizeof(char)*256);
    sprintf(a[i].name, "a%d", i);
    if(i == 1) {
      a[i].is_not = 0;
    } else {
      a[i].is_not = 1;
    }
  }
  print_and_table(outf, a_equals_2, a, w);
  print_or(outf, even_restriction, 2, product0, a_equals_2); 
  set_line(outf, even_restriction);
  for(i=0; i<w; i++) {
    free(a[i].name);
  }
  
}

void print_size_terms(FILE *outf, int w) {
  int i;
  line_t a[w];
  line_t b[w];
  char tempstr[256];
  line_t temp = {tempstr, 0};
  static int done = 0;
  if(!done) {
    for(i=0; i<w; i++) {
      a[i].name = malloc(sizeof(char)*256);
      b[i].name = malloc(sizeof(char)*256);
      a[i].is_not = 1;
      b[i].is_not = 1;
      sprintf(a[i].name, "a%d", i);
      sprintf(b[i].name, "b%d", i);
    }
    
    for(i=0; i<(w/2)+(w%2); i++) {
      //    printf("%d %d %d\n", i,(w/2)+(w%2)-i,w/2); 
      sprintf(tempstr, "a_zero_from_%d", (w/2)+(w%2)-i);
      print_and_table(outf, temp, &a[(w/2)+(w%2)-i], i);
      sprintf(tempstr, "b_zero_from_%d", w-i);
      print_and_table(outf, temp, &b[w-i], i);
    }
    
    for(i=0; i<w; i++) {
      free(a[i].name);
      free(b[i].name);
    }
    done = 1;
  }
}

/*
 * Print size restriction
 * 
 * If some bit in upper half of b is set to true, then corresponding bits in 
 * a must be zero.
 *
 * If some bit in a is set to true, then corresponding bits in b must be zero.
 *
 */

void print_size_restriction(FILE *outf, int w) {
  int i;
  line_t a[w];
  line_t b[w];
  char tempstr[256];
  line_t temp = {tempstr, 0};
  char tempstr2[256];
  line_t temp2 = {tempstr2, 0};
  char tempstr3[256];
  line_t temp3 = {tempstr3, 0};

  for(i=0; i<w; i++) {
      a[i].name = malloc(sizeof(char)*256);
      b[i].name = malloc(sizeof(char)*256);
      a[i].is_not = 1;
      b[i].is_not = 1;
      sprintf(a[i].name, "a%d", i);
      sprintf(b[i].name, "b%d", i);
  }
  
  for(i=0; i<w; i++) {
    a[i].is_not = 0;
    b[i].is_not = 0;
  }

  temp2.is_not = 1;
  for(i=1; i<(w/2)-(w%2); i++) {
    temp.is_not = 0;
    sprintf(tempstr2, "b_zero_from_%d", w-i);
    sprintf(tempstr, "size_restriction_%d", i);
    print_and(outf, temp, 2, temp2, a[i]); 
    temp.is_not = 1;
    set_line(outf, temp);
    temp.is_not = 0;
    sprintf(tempstr2, "a_zero_from_%d", (w/2)+(w%2)-i);
    sprintf(tempstr, "a_size_restriction_%d", i);
    print_and(outf, temp, 2, temp2, b[(w/2)+(w%2)+i]);
    temp.is_not = 1;
    set_line(outf, temp); 
  }

  for(i=0; i<w; i++) {
    free(a[i].name);
    free(b[i].name);
  }
}

void print_product_size_terms(FILE *outf, int w) {
  char tmpz[256];
  line_t tmp = {tmpz, 0};
  line_t product[w];
  int i;

  for(i=0; i<w; i++) {
    product[i].name = malloc(sizeof(char)*256);
    sprintf(product[i].name, "product%d", w-i-1);
    //    product[i].is_not = 1;
  }

  for(i=0; i<w; i++) {
    product[i].is_not = 0;
    sprintf(tmpz, "product_first_1_is_%d", w-i-1);
    print_and_table(outf, tmp, product, i+1);
    product[i].is_not = 1;
  }
  
  for(i=0; i<w; i++) {
    free(product[i].name);
  }
}

void print_in_terms(FILE *outf, int width) {
  char tempstr[256];
  line_t temp = {tempstr, 0};
  char tempstr1[225];
  line_t temp1 = {tempstr1, 0};
  char tempstr2[256];
  line_t temp2 = {tempstr2, 0};
  int i, j;

  for(i=1; i<width; i++) {
    for(j=1; j<i;j++) {
      sprintf(tempstr, "combined_in_length_%d_%d", i, j);
      sprintf(tempstr1, "a_zero_from_%d", j);
      sprintf(tempstr2, "b_zero_from_%d", i-j);
      print_and(outf, temp, 2, temp1, temp2);
    }
  }
}

void print_in_restriction(FILE *outf, int width) {
  char tempstr[256];
  line_t temp = {tempstr, 0};
  char tempstr1[256];
  line_t temp1 = {tempstr1, 0};
  char tempstr2[256];
  line_t temp2 = {tempstr2, 0};
  int i,j;

  print_product_size_terms(outf, width);
  print_size_terms(outf, width);
  print_in_terms(outf, width);

  for(i=4; i<width; i++) {
    sprintf(tempstr1, "product_first_1_is_%d", i);
    for(j=0; j<i-1; j++) {
      sprintf(tempstr, "p_restriction%d_%d", i, j);
      sprintf(tempstr2, "combined_in_length_%d_%d", i-1, j);
      print_and(outf, temp, 2, temp1, temp2);
      temp.is_not = 1;
      set_line(outf, temp);
      temp.is_not = 0;
    }
  }
}

/*
int main(int argc, char *argv[]) {
  char *optstring = "a:b:p:w:f:t:1vo:m:";
  char *product = NULL;
  char *multiplicand = NULL;
  char *multiplier = NULL;
  int width = 0;
  int i, j;
  FILE *outfile = stdout;
  char c;
  int multiplier_type;
  int factorconstraint = 0;
  int outputformat = 0;
  int optimizations;

  while((c = getopt(argc, argv, optstring))!= EOF) {
    switch(c) {
    case 'a':
      multiplicand = optarg;
      break;
    case 'b':
      multiplier = optarg;
      break;
    case 'p':
      product = optarg;
      break;
    case 'w':
      if((width = atoi(optarg))==0) {
	fprintf(stderr, "Width == 0\n");
	return 2;
      }
      break;
    case 'f':
      if((outfile = fopen(optarg, "w"))==NULL) {
	fprintf(stderr, "Error while opening %s\n", optarg);
	return 1;
      }
      break;
    case 't':
      multiplier_type = atoi(optarg);
      break;
    case '1':
      factorconstraint = 1;
      break;
    case 'v':
      fprintf(stderr, "CVS Says: %s\n", VERSION);
      break;
    case 'o':
      outputformat =atoi(optarg);
      break;
    case 'm':
      optimizations = atoi(optarg);
      break;
    }
  }
  set_output_format(outfile, outputformat);
  // fprintf(stderr, "Set output format: %s\n", get_output_format_name(outputformat));
  switch(multiplier_type) {
  case BRAUNMULTIPLIER:
    printbraunmultiplier(outfile, width);
    break;
  case TREEADDERMULTIPLIER:
    printaddertreemult(outfile,width);
    break;
  case REDUCEDBRAUN:
    printreducedbraunmultiplier(outfile, width);
    break;
  }
  if(multiplicand != NULL) {
    printbitset(outfile, multiplicand, "a");
  }
  set_choiceset(outfile, width, "a");
  if(multiplier != NULL) {
    printbitset(outfile, multiplier, "b");
  }
  set_choiceset(outfile, width, "b");
  if(product != NULL) {
    printbitset(outfile, product, "product");
  }
  
  if(factorconstraint) {
    printnotone(outfile, "a", width);
    printnotone(outfile, "b", width);
  }
  
  if(optimizations & 0x1) {
    print_size_restriction(outfile, width);
  }
  if(optimizations & 0x2) {
    print_even_restriction(outfile, width);
  }
  if(optimizations & 0x4) {
    print_odd_restriction(outfile);
  }
  if(optimizations & 0x8) {
    print_in_restriction(outfile, width);
  }

  finalize_print(outfile);
  return 0;
}

*/
