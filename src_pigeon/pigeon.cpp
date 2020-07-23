
/*************************************************************

gen_pigeon.cpp -- 
(C) QUIROGA, Jose L. Bogota Colombia. South America

funcs for pigeon hole sat instance generation 

--------------------------------------------------------------*/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

#define CARRIAGE_RETURN		((char)13)

#define DBG_CK(prm) 	assert(prm)

// PIGEON PRT_FUNCS

inline
long
get_num_neu(long holes, bool with_redun){
	long pigeons = (holes + 1);
	long num_neu = ((((holes * holes) + holes) / 2) * holes) + pigeons;
	if(with_redun){
		long r_num_neu = ((((holes * holes) - holes) / 2) * pigeons) + holes;
		num_neu += r_num_neu;
	}
	return num_neu;
}

std::ostream& 	
print_php_by_hole(std::ostream& os, long holes, long pos_skip, bool with_redun = false){
	long pigeons = (holes + 1);
	long num_neu = get_num_neu(holes, with_redun);
	long num_var = holes * pigeons;
	long ii, jj, kk, hh1, hh2, pp;

	long num_neu_prt = num_neu;
	pos_skip--;
	if(pos_skip >= 0){
		num_neu_prt--;
	}

	os << "c by QUIROGA, Jose. Bogota Colombia. South America."<< std::endl;
	os << "c pigeon hole instance ";
	if(pos_skip < 0){
		os << "(unsat)";
	} else {
		os << "(sat";
		os << " skip = " << (pos_skip + 1);
		os << ")";
	}
	os << "(" << holes << " holes)(var rot by hole)" << std::endl;
	os << "p cnf " << num_var << " " << num_neu_prt << std::endl;

	long verif_num_neu = 0;
	for(ii = 0; ii < holes; ii++){
		for(jj = 1; jj <= pigeons; jj++){
			long var1 = (ii * pigeons) + jj;
			for(kk = jj + 1; kk <= pigeons; kk++){
				long var2 = (ii * pigeons) + kk;
				if(pos_skip != verif_num_neu){
					os << -var1 << " " << -var2 << " 0" << std::endl;
				}
				verif_num_neu++;
			}
		}
		os << std::endl;
	}
	os << std::endl;
	for(jj = 1; jj <= pigeons; jj++){
		if(pos_skip != verif_num_neu){
			for(ii = 0; ii < holes; ii++){
				long var1 = (ii * pigeons) + jj;
				os << var1 << " ";
			}
			os << " 0" << std::endl;
		}
		verif_num_neu++;
	}
	if(with_redun){
		os << std::endl << std::endl << std::endl;
		for(pp = 1; pp <= pigeons; pp++){
			for(hh1 = 0; hh1 < holes; hh1++){
				long var1 = (hh1 * pigeons) + pp;
				for(hh2 = hh1 + 1; hh2 < holes; hh2++){
					long var2 = (hh2 * pigeons) + pp;
					os << -var1 << " " << -var2 << " 0" << std::endl;
					verif_num_neu++;
				}
			}
			os << std::endl;
		}
		os << std::endl;
		for(hh1 = 0; hh1 < holes; hh1++){
			for(pp = 1; pp <= pigeons; pp++){
				long var1 = (hh1 * pigeons) + pp;
				os << var1 << " ";
			}
			os << " 0" << std::endl;
			verif_num_neu++;
		}
	}
	DBG_CK(verif_num_neu == num_neu); 
	return os;
}

std::ostream& 	
print_php_by_pigeon(std::ostream& os, long holes, long pos_skip, bool with_redun = false){
	long pigeons = (holes + 1);
	long num_neu = get_num_neu(holes, with_redun);
	long num_var = holes * pigeons;
	long ii, jj, kk, hh1, hh2, pp;

	long num_neu_prt = num_neu;
	pos_skip--;
	if(pos_skip >= 0){
		num_neu_prt--;
	}

	os << "c by QUIROGA, Jose. Bogota Colombia. South America."<< std::endl;
	os << "c pigeon hole instance ";
	if(pos_skip < 0){
		os << "(unsat)";
	} else {
		os << "(sat";
		os << " skip = " << (pos_skip + 1);
		os << ")";
	}
	os << "(" << holes << " holes)(var rot by pigeon)" << std::endl;
	os << "p cnf " << num_var << " " << num_neu_prt << std::endl;

	long verif_num_neu = 0;
	for(ii = 1; ii <= holes; ii++){
		for(jj = 0; jj < pigeons; jj++){
			long var1 = ii + (jj * holes);
			for(kk = jj + 1; kk < pigeons; kk++){
				long var2 = ii + (kk * holes);
				if(pos_skip != verif_num_neu){
					os << -var1 << " " << -var2 << " 0" << std::endl;
				}
				verif_num_neu++;
			}
		}
		os << std::endl;
	}
	os << std::endl;
	for(jj = 0; jj < pigeons; jj++){
		if(pos_skip != verif_num_neu){
			for(ii = 1; ii <= holes; ii++){
				long var1 = ii + (jj * holes);
				os << var1 << " ";
			}
			os << " 0" << std::endl;
 		}
		verif_num_neu++;
	}

	if(with_redun){
		os << std::endl << std::endl << std::endl;
		for(pp = 0; pp < pigeons; pp++){
			for(hh1 = 1; hh1 <= holes; hh1++){
				long var1 = hh1 + (pp * holes);
				for(hh2 = hh1 + 1; hh2 <= holes; hh2++){
					long var2 = hh2 + (pp * holes);
					os << -var1 << " " << -var2 << " 0" << std::endl;
					verif_num_neu++;
				}
			}
			os << std::endl;
		}
		os << std::endl;
		for(hh1 = 1; hh1 <= holes; hh1++){
			for(pp = 0; pp < pigeons; pp++){
				long var1 = hh1 + (pp * holes);
				os << var1 << " ";
			}
			os << " 0" << std::endl;
			verif_num_neu++;
		}
	}
	DBG_CK(verif_num_neu == num_neu); 
	return os;
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

void	get_name(std::string& inst_nam, long holes, long digs_nm, bool order, long ii, long n_dig, bool with_redun){
	std::string f_nam = "h";	// by hole
	if(order){
		f_nam = "p";	// by pigeon
	}

	//std::cerr << "digs_nm=" << digs_nm << std::endl;

	std::ostringstream out_nam;
	out_nam << f_nam;
	if(with_redun){
		out_nam << "r";
	}
	out_nam.width(digs_nm);
	out_nam.fill('0');
	out_nam << holes;
	if(ii > 0){
		out_nam << "_";
		out_nam.width(n_dig);
		out_nam.fill('0');
		out_nam << ii;
	}
	out_nam << ".yos";
	inst_nam = out_nam.str();
}

void
generate_pigeon(long holes, long digs_nm, bool order, long ii, bool with_redun = false){
	std::string inst_nam;

	long first = 0;
	long last = ii + 1;
	long num_dig = num_digits(last);
	if(ii != 0){
		first = ii;
	} else {
		first = 1;
		last = get_num_neu(holes, with_redun);
		num_dig = num_digits(last);
	}

	DBG_CK((first >= 0) || ((first + 1) == last));
	for(long kk = first; kk < last; kk++){	
		get_name(inst_nam, holes, digs_nm, order, kk, num_dig, with_redun);

		std::ofstream ff;
		ff.open(inst_nam.c_str());
		if(ff.good() && ff.is_open()){
			if(order){
				print_php_by_pigeon(ff, holes, kk, with_redun);
			} else {
				print_php_by_hole(ff, holes, kk, with_redun);
			}
			ff.close();
		}
	}
}

int 
main(int argc, char** argv){
	std::ostream& os = std::cout;

	long ii = -3;
	long first_num = 2;
	long last_num = 3;
	bool with_redun = false;
	if(argc > 3){
		long rr = atol(argv[3]);
		if(rr != 0){
			with_redun = true;
		} 
	} 
	if(argc > 2){
		ii = atol(argv[2]);
		last_num = atol(argv[1]) + 1;
		if(ii != -1){
			first_num = last_num - 1; 
		}
	} else if(argc > 1){
		first_num = atol(argv[1]); 
		last_num = first_num + 1;
	} else {
		os << argv[0] << " <num_holes> [ii]" << std::endl << std::endl;

		os << "no ii -> unsat with num_holes." << std::endl;
		os << "ii = -1 -> all unsat with num_holes or less." << std::endl;
		os << "ii = 0 -> all sat with num_holes." << std::endl;
		os << "ii > 0 -> sat with num_holes skipping ii." << std::endl;
		os << "ii = -2 -> print num clauses with num_holes." << std::endl;
		os << "ii = -3 -> unsat with num_holes." << std::endl;

		return 0;
	}

	if(last_num < 2){
		first_num = 2;
		last_num = 3;
	}

	if(ii == -2){
		long num_cla = get_num_neu(first_num, false);
		os << "num clauses = " << num_cla << std::endl; 
		long r_num_cla = get_num_neu(first_num, true);
		os << "num clauses (with_redund) = " << r_num_cla << std::endl; 
		return 0;
	}

	long digs_nm = num_digits(last_num);

	//os << "fst=" << first_num << " lst=" << last_num << std::endl;
	for(long num_holes = first_num; num_holes < last_num; num_holes++){
		generate_pigeon(num_holes, digs_nm, false, ii, with_redun);
		generate_pigeon(num_holes, digs_nm, true, ii, with_redun);
		os << CARRIAGE_RETURN;
		os << "Generating instance " << num_holes << ".";
		os.flush();
	}
	os << std::endl;
	return 0;
}

