


/*************************************************************

gsat_is_prime

print_dimacs.cpp  
(C 2010) QUIROGA BELTRAN, Jose Luis. Bogotá - Colombia.

Date of birth: December 28 of 1970.
Place of birth: Bogota - Colombia - Southamerica.
Id (cedula): 79523732 de Bogota.
email: JoseLuisQuiroga@yahoo.com

functions to print in dimacs format the constrained multiplier.

--------------------------------------------------------------*/

#include <stdarg.h>
#include "output-format.h"

#include <assert.h>
#include <cstring>

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <list>
#include <iterator>

std::map<std::string, long> dimacs_vars;
long dimacs_num_ccls;
long dimacs_p_cnf_pos;

long get_var(std::string nam){
	long the_v = dimacs_vars[nam];
	if(the_v == 0){
		the_v = dimacs_vars.size();
		dimacs_vars[nam] = the_v;
	}
	assert(the_v > 0);
	return the_v;
}

long get_lit(line_t iline){
	std::string nam = iline.name;
	long the_v = get_var(nam);
	if(iline.is_not){
		the_v = -the_v;
	}
	return the_v;
}

void args_to_vars(int a_sz, va_list vl, std::list<long>& vars){
	vars.clear();
	line_t iline;
	while(a_sz > 0){
		a_sz--;
		iline = va_arg(vl, line_t);
		long vvar = get_lit(iline);
		vars.push_front(vvar);
	}
}

void dimacs_initialize(FILE *outfile) {
	dimacs_vars.clear();
	dimacs_num_ccls = 0;
	dimacs_p_cnf_pos = 0;

	std::string ff_nam = "false";
	long v_false = get_var(ff_nam);
	fprintf(outfile, "c bit multiplier dimacs instance\n");

	dimacs_p_cnf_pos = ftell(outfile);
	// leave enough spaces to print numbers when 'dimacs_finalize'
	fprintf(outfile, "p cnf                                                              \n");

	fprintf(outfile, "%ld 0\n", -v_false);
	dimacs_num_ccls++;

	//fprintf(outfile, "false := F;\n");
}

void dimacs_finalize(FILE *outfile) {
	long num_var = dimacs_vars.size();
	long num_ccl = dimacs_num_ccls;
	//assert(dimacs_p_cnf_pos > 0);
	fseek(outfile, dimacs_p_cnf_pos, SEEK_SET);
	fprintf(outfile, "p cnf %ld %ld\n", num_var, num_ccl);
}

void dimacs_print_and_ccls(FILE *outf, long g1, std::list<long>& vars){
	// !g -> !c1 | ... | !cn 
	assert(g1 > 0);
	fprintf(outf, "%ld ", g1);
	for(std::list<long>::iterator aa = vars.begin(); aa != vars.end(); aa++){
		long vv = (*aa);
		fprintf(outf, "%ld ", -vv);
	}
	fprintf(outf, "0\n");
	dimacs_num_ccls++;

	// g -> ci 
	for(std::list<long>::iterator aa = vars.begin(); aa != vars.end(); aa++){
		long vv = (*aa);
		fprintf(outf, "%ld %ld 0\n", -g1, vv);
		dimacs_num_ccls++;
	}
}

void dimacs_print_and(FILE *outf, line_t outline, int count, ...) {
	long g_var = get_lit(outline);
	std::list<long> vars;

	va_list vl;
	va_start(vl, count);
	args_to_vars(count, vl, vars);
	dimacs_print_and_ccls(outf, g_var, vars);
	va_end(vl);
}

void dimacs_print_or_ccls(FILE *outf, long g1, std::list<long>& vars){
	// g -> c1 | ... | cn 
	assert(g1 > 0);
	fprintf(outf, "%ld ", -g1);
	for(std::list<long>::iterator aa = vars.begin(); aa != vars.end(); aa++){
		long vv = (*aa);
		fprintf(outf, "%ld ", vv);
	}
	fprintf(outf, "0\n");
	dimacs_num_ccls++;

	// !g -> !ci 
	for(std::list<long>::iterator aa = vars.begin(); aa != vars.end(); aa++){
		long vv = (*aa);
		fprintf(outf, "%ld %ld 0\n", g1, -vv);
		dimacs_num_ccls++;
	}
}

void dimacs_print_or(FILE *outf, line_t outline, int count, ...) {
	long g_var = get_lit(outline);
	std::list<long> vars;

	va_list vl;
	va_start(vl, count);
	args_to_vars(count, vl, vars);
	dimacs_print_or_ccls(outf, g_var, vars);
	va_end(vl);
}

void dimacs_print_xor_ccls(FILE *outf, long g1, std::list<long>& vars){
	assert(vars.size() == 2);
	long vv1 = vars.front();
	long vv2 = vars.back();

	// g := c1 ^ c2 
	// g -> (c1 -> ~c2) 
	fprintf(outf, "%ld %ld %ld 0\n", -g1, -vv1, -vv2);
	dimacs_num_ccls++;
	// g -> (~c1 -> c2) 
	fprintf(outf, "%ld %ld %ld 0\n", -g1, vv1, vv2);
	dimacs_num_ccls++;
	// ~g -> (c1 -> c2) 
	fprintf(outf, "%ld %ld %ld 0\n", g1, -vv1, vv2);
	dimacs_num_ccls++;
	// ~g -> (~c1 -> ~c2) 
	fprintf(outf, "%ld %ld %ld 0\n", g1, vv1, -vv2);
	dimacs_num_ccls++;
}

void dimacs_print_xor(FILE *outf, line_t outline, int count, ...) {
	long g_var = get_lit(outline);
	std::list<long> vars;

	va_list vl;
	va_start(vl, count);
	args_to_vars(count, vl, vars);
	dimacs_print_xor_ccls(outf, g_var, vars);
	va_end(vl);
}

void dimacs_set_line(FILE *outf, line_t line) {
	long g_var1 = get_lit(line);
	fprintf(outf, "%ld 0\n", g_var1);
	dimacs_num_ccls++;

	//  fprintf(outf, "ASSIGN %s%s;\n", line.not ? "~" : "", line.name);
}

void dimacs_set_equal(FILE *outf, line_t line1, line_t line2) {
	long g1 = get_lit(line1);
	long g2 = get_lit(line2);

	assert(g1 > 0);
	fprintf(outf, "%ld %ld 0\n", g1, -g2);
	dimacs_num_ccls++;
	fprintf(outf, "%ld %ld 0\n", -g1, g2);
	dimacs_num_ccls++;

	//  fprintf(outf, "%s := %s%s;\n", line1.name, line2.not ? "~" : "",  line2.name);
}

void dimacs_set_choice(FILE *outf, line_t l) {
}


