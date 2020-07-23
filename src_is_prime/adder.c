/*
 * (C) Tuomo Pyhälä
 */
#include <stdio.h>
#include <unistd.h>
#include "output-format.h"

void printpfa(FILE *f, char *prefix, char *in1, char *in2, int n) {
  line_t in1_l = {in1, 0};
  line_t in2_l = {in2, 0};
  char genstr[256], propstr[256], sumstr[256], carrystr[256];
  line_t propagate = {propstr, 0};
  line_t generate = {genstr, 0};
  line_t sum = {sumstr, 0};
  line_t carry = {carrystr, 0};
  sprintf(sumstr, "%s_sum%d", prefix,n);
  sprintf(genstr, "%s_generate%d_%d", prefix, n, n);
  sprintf(propstr, "%s_propagate%d_%d", prefix, n, n);
  sprintf(carrystr, "%s_carry%d", prefix, n);

  print_xor(f, propagate, 2, in1_l, in2_l);
  print_and(f, generate, 2, in1_l, in2_l);
  print_xor(f, sum, 2, propagate, carry);
  /*  fprintf(f, "// %s + %s\n", in1, in2); */
  /*  fprintf(f, "%s_propagate%d_%d := %s ^ %s;\n", prefix, n, n, in1, in2);
  fprintf(f, "%s_generate%d_%d := %s AND %s;\n", prefix, n, n, in1, in2);
  fprintf(f, "%s_sum%d := %s_propagate%d_%d ^ %s_carry%d;\n", prefix, n, 
  prefix, n, n, prefix, n); */
}

void printpropagatelogic(FILE *f, char *prefix, int size, int n) {
  /* carry n+size */
  char generate1_s[256], generate2_s[256], generate3_s[256], generate4_s[256];
  char propagate1_s[256], propagate2_s[256], propagate3_s[256], propagate4_s[256];
  char carry1_s[256], carry2_s[256], carry3_s[256], carry4_s[256];
  char generateup_s[256], propagateup_s[256], pgenerate12_s[256], pgenerate13_s[256], pgenerate23_s[256];
  char carry0_s[256];
  char propcarry1_s[256], propcarry2_s[256], propcarry3_s[256], pgenerate1up_s[256], pgenerate2up_s[256], pgenerate3up_s[256];
  
  

  line_t generate1 = {generate1_s, 0};
  line_t generate2 = {generate2_s, 0};
  line_t generate3 = {generate3_s, 0};
  line_t generate4 = {generate4_s, 0};
  
  line_t propagate1 = {propagate1_s, 0};
  line_t propagate2 = {propagate2_s, 0};
  line_t propagate3 = {propagate3_s, 0};
  line_t propagate4 = {propagate4_s, 0};
  
  line_t carry0 = {carry0_s, 0};
  line_t carry1 = {carry1_s, 0};
  line_t carry2 = {carry2_s, 0};
  line_t carry3 = {carry3_s, 0};
  
  line_t propagatecarry1 = {propcarry1_s, 0};
  line_t propagatecarry2 = {propcarry2_s, 0};
  line_t propagatecarry3 = {propcarry3_s, 0};
  
  line_t generateup = {generateup_s, 0};
  line_t propagateup = {propagateup_s, 0};
  
  line_t pgenerate12 = {pgenerate12_s, 0};
  line_t pgenerate13 = {pgenerate13_s, 0};
  line_t pgenerate23 = {pgenerate23_s, 0};

  line_t pgenerate1_up = {pgenerate1up_s, 0};
  line_t pgenerate2_up = {pgenerate2up_s, 0};
  line_t pgenerate3_up = {pgenerate3up_s, 0};
  
  sprintf(generate1_s, "%s_generate%d_%d", prefix, n, n+size-1);
  sprintf(generate2_s, "%s_generate%d_%d", prefix, n+size, n+size*2-1);
  sprintf(generate3_s, "%s_generate%d_%d", prefix, n+size*2, n+size*3-1);
  sprintf(generate4_s, "%s_generate%d_%d", prefix, n+size*3, n+size*4-1);

  sprintf(propagate1_s, "%s_propagate%d_%d", prefix, n, n+size-1);
  sprintf(propagate2_s, "%s_propagate%d_%d", prefix, n+size, n+size*2-1);
  sprintf(propagate3_s, "%s_propagate%d_%d", prefix, n+size*2, n+size*3-1);
  sprintf(propagate4_s, "%s_propagate%d_%d", prefix, n+size*3, n+size*4-1);
  
  sprintf(carry0_s, "%s_carry%d", prefix, n);
  sprintf(carry1_s, "%s_carry%d", prefix, n+size);
  sprintf(carry2_s, "%s_carry%d", prefix, n+size*2);
  sprintf(carry3_s, "%s_carry%d", prefix, n+size*3);

  sprintf(propcarry1_s, "%s_propagatecarry%dto%d", prefix, n, n+size); 
  sprintf(propcarry2_s, "%s_propagatecarry%dto%d", prefix, n, n+size*2);
  sprintf(propcarry3_s, "%s_propagatecarry%dto%d", prefix, n, n+size*3);

  sprintf(generateup_s, "%s_generate%d_%d", prefix, n, n+size*4-1);
  sprintf(propagateup_s, "%s_propagate%d_%d", prefix, n, n+size*4-1);

  sprintf(pgenerate12_s, "%s_propagate_generate%d_%d_to_%d", prefix, n, n+size-1, n+size*2);
  sprintf(pgenerate13_s, "%s_propagate_generate%d_%d_to_%d", prefix, n, n+size-1, n+size*3);
  sprintf(pgenerate23_s, "%s_propagate_generate%d_%d_to_%d", prefix, n+size, n+size*2-1, n+size*3);

  sprintf(pgenerate1up_s, "%s_propagategenerate%d_%d_to_%d_%d", prefix, n, n+size-1, n, n+size*4-1);
  sprintf(pgenerate2up_s, "%s_propagategenerate%d_%d_to_%d_%d", prefix, n+size*1, n+size*2-1, n, n+size*4-1);
  sprintf(pgenerate3up_s, "%s_propagategenerate%d_%d_to_%d_%d", prefix, n+size*2, n+size*3-1, n, n+size*4-1);

	  

  print_and(f, propagatecarry1, 2, propagate1, carry0);
  print_and(f, propagatecarry2, 3, propagate1, propagate2, carry0);
  print_and(f, propagatecarry3, 4, propagate1, propagate2, propagate3, carry0);

  /* carry n+size */

  print_or(f, carry1, 2, propagatecarry1, generate1);

  /* carry n+size*2 */

  print_and(f, pgenerate12, 2, propagate2, generate1);
  print_or(f, carry2, 3, pgenerate12, propagatecarry2, generate2);
  

  /* carry n+size*3 */
  
  print_and(f, pgenerate13, 3, propagate2, propagate3, generate1);
  print_and(f, pgenerate23, 2, propagate3, generate2);
  print_or(f, carry3, 4, generate3, pgenerate13, pgenerate23, propagatecarry3);
  
  /* higher level propagation signal */

  print_and(f, propagateup, 4, propagate1, propagate2, propagate3, propagate4);

  /* higher level generate signal */

  print_and(f, pgenerate1_up, 4, generate1, propagate2, propagate3, propagate4);
  print_and(f, pgenerate2_up, 3, generate2, propagate3, propagate4);
  print_and(f, pgenerate3_up, 2, generate3, propagate4);
  print_or(f, generateup, 4, pgenerate1_up, pgenerate2_up, pgenerate3_up, generate4);
}

void printdummypropagatelogic(FILE *f, char *prefix, int size, int n) {
  /* carry n+size */
  char propagateup_s[256], generateup_s[256];
  
  line_t propagateup = {propagateup_s, 0};
  line_t generateup = {generateup_s, 0};
  line_t false_l = {"false", 0};

  //  fprintf(f, "%s_carry%d := F;\n", prefix, n+size, prefix, n, n+size, prefix, n, n+size-1);
  /* carry n+size*2 */
  //fprintf(f, "%s_carry%d := F;\n", prefix, n+size*2);
  /* carry n+size*3 */
  //  fprintf(f, "%s_carry%d := F;\n", prefix, n+size*3);
  /* higher level propagation signal */
  //  fprintf(f, "%s_propagate%d_%d := F; \n", prefix, n, n+size*4-1);
  /* higher level generate signal */
  // fprintf(f, "%s_generate%d_%d := F;\n",  prefix, n, n+size*4-1);  
  
  sprintf(propagateup_s, "%s_propagate%d_%d",  prefix, n, n+size*4-1);
  sprintf(generateup_s, "%s_generate%d_%d", prefix, n, n+size*4-1);
  set_equal(f, propagateup, false_l);
  set_equal(f, generateup, false_l);
}


int log4(int n) {
  int ret;
  for(ret = 0; n > 1; ret++) {
    n /= 4;
  }
  return ret;
}


void printadder(FILE *outfile, int width, char *inlines[], char *prefix) {
  int levelcount;
  int i,j;
  int size = 1;
  char *s1, *s2;
  int realwidth, dummywidth;
  int finalcarrylevel;
  char l1_s[256], l2_s[256];
  line_t l1 = {l1_s, 0};
  line_t l2 = {l2_s, 0};
  line_t false_l = {"false", 0};

  realwidth = width;

  for(i=4;i<width; i*=4)
    if(realwidth%i == 0)
      finalcarrylevel = i;
  
  width = i;
  
  j = realwidth + ((realwidth%4 == 0) ? 0 : 4 - (realwidth%4));

  for(i=0; i<j; i++) {
    if(i<realwidth) {
      s1 = inlines[i];
      s2 = inlines[i+realwidth];
    }
    else {
      s1 = "false";
      s2 = "false";
    }
      
    printpfa(outfile, prefix, s1, s2, i);
  }
  
  levelcount = log4(width);
  
  //  printf("// ADDER WIDTH %d (LEVELS %d)\n", width, levelcount); 
  
  for(i=1; i<=levelcount; i++) {
    for(j=0; j<realwidth;j+=size*4) {
      printpropagatelogic(outfile, prefix, size, j);
    }
    dummywidth = j + ((j % (size * 16) == 0) ? 0 : 16*size - (j % (size*16))); 
    for(;j<dummywidth;j+=size*4) {
      printdummypropagatelogic(outfile, prefix, size, j);
    }
    size *= 4;
  }

  sprintf(l1_s, "%s_carry0", prefix);
  set_equal(outfile, l1, false_l);
  
  // fprintf(outfile, "%s_propagate0out := %s_carry0 AND %s_propagate%d_%d;\n", prefix, prefix, prefix, 0, width-1);
  // fprintf(outfile, "%s_carryout := %s_generate%d_%d OR %s_propagate0out;\n", prefix, prefix, 0, width-1, prefix);
  
  if(width != realwidth) {
    sprintf(l1_s, "%s_carryout", prefix);
    sprintf(l2_s, "%s_carry%d", prefix, realwidth);
    set_equal(outfile, l1,l2);
  }
  else {
    sprintf(l1_s, "%s_carryout", prefix);
    sprintf(l2_s, "%s_generate%d_%d", prefix, 0, realwidth-1);
    set_equal(outfile, l1,l2);
  }
  /*    
	if(realwidth%4==0) {
	fprintf(outfile, "%s_carryout := %s_carry%d;\n", realwidth);
	//  fprintf(outfile, "%s_carryout := %s_generate%d_%d;\n", prefix, prefix, realwidth-finalcarrylevel, realwidth-1);
	}
	else {
	fprintf(outfile, "%s_carryout := %s_sum%d;\n", prefix, prefix, realwidth);
	}
  */
}















