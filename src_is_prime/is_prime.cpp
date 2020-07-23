

/*************************************************************

gsat_is_prime

is_prime.cpp  
(C 2010) QUIROGA BELTRAN, Jose Luis. Bogotá - Colombia.

Date of birth: December 28 of 1970.
Place of birth: Bogota - Colombia - Southamerica.
Id (cedula): 79523732 de Bogota.
email: JoseLuisQuiroga@yahoo.com

Main function to generate ans is_prime instance.

--------------------------------------------------------------*/


#include <unistd.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <gmpxx.h>

#include "braun-multiplier.h"
#include "output-format.h"

#define CARRIAGE_RETURN		((char)13)

class running_args {
public:
	bool		prt_help;
	bool		prt_version;

	std::string	help_str;
	std::string	version_str;
	
	FILE*		output_file_pt;
	std::string 	output_file_nm;
	int		output_format;
	int		multiplier_type;

	mpz_class 	input_number;
	std::string 	input_bin_num;
	int 		input_bin_width;

	long		next_n_primes;
	bool		generating_primes;

	running_args(){
		init_running_args();
	}

	~running_args(){
	}

	void 	init_running_args();

	int	get_format_idx(std::string fmt_nm);
	bool	get_args(int argc, char** argv);
};

void
running_args::init_running_args(){
	prt_help = false;
	prt_version = false;

	help_str =
		"<the_num> [-h | -v] "
		"[-n <num_next>] "
		"[-c] "
		"[-f ('bcsat' | 'smodel' | 'dimacs')] "
		"[-t ('braun' | 'tree') ]\n"
		"\n"
		"-h : print help.\n"
		"-v : print version.\n"
		"-n : generate for the next 'num_next' primes after 'the_num' (or composites if -c). "
		"Default is 0 meaning generate just for 'the_num'.\n"
		"-c : generate composites (prime + 1). Only used when -n is used.\n"
		"-f : set the output format. Default is dimacs.\n"
		"-t : set the type of multiplier. Default is braun.\n"
		"\n"
		;

	version_str =
		"v0.1\n"
		"(c) 2010. QUIROGA BELTRAN, Jose Luis. c.c. 79523732. Bogota - Colombia.\n"
		;

	output_file_pt = stdout;
	output_file_nm = "";
	output_format = 0;
	multiplier_type = BRAUNMULTIPLIER;
	input_number = 0;
	input_bin_num = "";
	input_bin_width = 0;
	next_n_primes = 0;
	generating_primes = true;
}

int
running_args::get_format_idx(std::string fmt_nm){
	for(int ii = 0; formats[ii].name != NULL; ii++){
		std::string fii_nm = formats[ii].name;
		std::cerr << "comparing " << fii_nm << " with " << fmt_nm << std::endl;
		if(fii_nm == fmt_nm){
			return ii;
		}
	}
	return 0;	// dimacs
}

bool
running_args::get_args(int argc, char** argv)
{
	std::ostream& os = std::cout;
	//MARK_USED(os);

	for(long ii = 1; ii < argc; ii++){
		std::string the_arg = argv[ii];
		if(the_arg == "-h"){
			prt_help = true;
		} else if(the_arg == "-v"){
			prt_version = true;
		} else if((the_arg == "-o") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			output_file_nm = argv[kk_idx];

		} else if((the_arg == "-n") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			next_n_primes = atol(argv[kk_idx]);
			if(next_n_primes < 0){
				next_n_primes = 0;
			}

		} else if(the_arg == "-c"){
			generating_primes = false;
		} else if((the_arg == "-f") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			std::string m_format = argv[ii];
			output_format = get_format_idx(m_format);

		} else if((the_arg == "-t") && ((ii + 1) < argc)){
			int kk_idx = ii + 1;
			ii++;

			std::string m_type = argv[ii];
			if(m_type == "braun"){
				multiplier_type = BRAUNMULTIPLIER;
			}
			else if(m_type == "tree"){
				multiplier_type = TREEADDERMULTIPLIER;
			}

		} else if(input_bin_num.size() == 0){
			input_number = the_arg;
			input_bin_num = input_number.get_str(2);

			int ww = input_bin_num.size();
			std::string pad;
			pad.resize(ww, '0');

			input_bin_num = pad + input_bin_num;
			input_bin_width = ww;
			
			std::cerr << "num=" << input_number << " as_bin=" << input_bin_num << std::endl;
			std::cerr << "width=" << input_bin_width << std::endl;
		}
	}

	if(prt_help){
		os << argv[0] << " " << help_str << std::endl;
		return false;
	}
	if(prt_version){
		os << argv[0] << " " << version_str << std::endl;
		return false;
	}
	if(input_bin_num.size() == 0){
		os << argv[0] << " " << help_str << std::endl;
		os << argv[0] << " " << version_str << std::endl;
		return false;
	}

	return true;
}

long	num_digits(long ii){
	double jj = ii;
	if(jj < 0){
		jj = -jj; 
	}
	long n_dig = 1;
	while((jj /= 10) > 1){
		n_dig++;
	}
	return n_dig;
}

std::string
get_name(mpz_class the_num, long digs_nm){
	std::string f_nam = "is_prime_";

	//std::cerr << "digs_nm=" << digs_nm << std::endl;

	std::ostringstream out_nam;
	out_nam << f_nam;
	out_nam.width(digs_nm);
	out_nam.fill('0');
	out_nam << the_num;
	out_nam << ".yos";

	return out_nam.str();
}

void 
print_is_prime(FILE *outfile, std::string in_num, int width, int format, int mult_typ){

	set_output_format(outfile, format);

	switch(mult_typ){
	case BRAUNMULTIPLIER:
		printbraunmultiplier(outfile, width);
		break;
	case TREEADDERMULTIPLIER:
		printaddertreemult(outfile, width);
		break;
	case REDUCEDBRAUN:
		printreducedbraunmultiplier(outfile, width);
		break;
	}

	char* prod = (char*)(in_num.c_str());
	printbitset(outfile, prod, (char*)("product"));
	printnotone(outfile, (char*)("a"), width);
	printnotone(outfile, (char*)("b"), width);
  
	finalize_print(outfile);
}

std::string 
get_bin_str(mpz_class the_num, int& the_width){
	the_width = 0;
	std::string bin_str = the_num.get_str(2);

	int ww = bin_str.size();
	std::string pad;
	pad.resize(ww, '0');

	bin_str = pad + bin_str;
	the_width = ww;
	
	std::ostream& os = std::cout;
	os << CARRIAGE_RETURN;
	os << "Generating instance num=" << the_num << "  as_bin=" << bin_str << "  width=" << the_width << ".";
	os.flush();
	return bin_str;
}

void
generate_next_is_prime(mpz_class the_num, long digs_nm, int format, int mult_typ){
	std::string inst_nam = get_name(the_num, digs_nm);

	FILE* fp = fopen(inst_nam.c_str(), "w+");

	if(fp != NULL){
		int the_width = 0;
		std::string bin_str = get_bin_str(the_num, the_width);
		print_is_prime(fp, bin_str, the_width, format, mult_typ);
		fclose(fp);
	}

	/*
	std::ofstream ff;
	ff.open(inst_nam.c_str());
	if(ff.good() && ff.is_open()){
		print_is_prime(ff, last_num, width, format, mult_typ);
		ff.close();
	}
	*/
}

void
generate_all_is_prime(running_args& aas){
	std::ostream& os = std::cout;

	long last = aas.next_n_primes;
	if(last == 0){
		last = 1;
	}
	mpz_class p1, p2, p3;
	p1 = aas.input_number;
	p2 = aas.input_number;

	p3 = p1 + aas.next_n_primes;
	std::string p3_str = p3.get_str();
	long digs_nm = p3_str.size();

	int width = aas.input_bin_width;
	int format = aas.output_format;
	int mult_typ = aas.multiplier_type;

	for(long ii = 0; ii < last; ii++){
		bool is_p = true;
		if(aas.next_n_primes > 0){
			mpz_nextprime(p1.get_mpz_t(), p2.get_mpz_t());
			p2 = p1;

			is_p = aas.generating_primes;
			if(! is_p){
				p1++;
			}
		}
		generate_next_is_prime(p1, digs_nm, format, mult_typ);
	}

	os << std::endl;
	os.flush();
}

int
mult_main(int argc, char *argv[]) 
{
	running_args aas;
	if(! aas.get_args(argc, argv)){
		return 1;
	}

	generate_all_is_prime(aas);
	/*
	FILE *outfile = aas.output_file_pt;
	int width = aas.input_bin_width;

	set_output_format(outfile, aas.output_format);

	switch(aas.multiplier_type) {
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

	char* prod = (char*)(aas.input_bin_num.c_str());
	printbitset(outfile, prod, (char*)("product"));
	printnotone(outfile, (char*)("a"), width);
	printnotone(outfile, (char*)("b"), width);
  
	finalize_print(outfile);
	*/
	return 0;
}

int 
main(int argc, char *argv[]) 
{
	return mult_main(argc, argv);
}



