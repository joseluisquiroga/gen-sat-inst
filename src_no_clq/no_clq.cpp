
/*************************************************************

gen_noclingra.cpp -- 
(C) QUIROGA, Jose L. Bogota Colombia. South America

funcs for no_clique sat instance generation.

--------------------------------------------------------------*/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

#define CARRIAGE_RETURN		((char)13)

#define DBG_CK(prm) 	assert(prm)

// NOCLIQUE PRT_FUNCS

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

inline
long	prop_var_id(long base, long xx_rang, long xx, long yy){
	return (base + ((yy - 1) * xx_rang) + xx);
}

inline
long	prop_diag_id(long base, long xx, long yy){
	DBG_CK(xx < yy);
	long id = base + ((yy * (yy - 1))/2) - (yy - 1 - xx);
	return id;
}

inline
long	diag_sum(long max_xx){
	long max_1 = max_xx - 1;
	return (((max_1 * max_1) + max_1) / 2);
}

inline
long
get_num_neu(long num_col){
	long num_nod = num_col + 2;
	long clique_sz = num_col + 1;

	long num_neu_1 = clique_sz;
	long num_neu_2 = (num_nod * diag_sum(clique_sz));
	long num_neu_3 = (((clique_sz * clique_sz) - clique_sz) * diag_sum(num_nod));
	long num_neu_4 = num_nod;
	long num_neu_5 = (num_col * diag_sum(num_nod));

	/*os << "c ";
	os << "neu1 = " << num_neu_1 << " ";
	os << "neu2 = " << num_neu_2 << " ";
	os << "neu3 = " << num_neu_3 << " ";
	os << "neu4 = " << num_neu_4 << " ";
	os << "neu5 = " << num_neu_5 << " ";
	os << std::endl;*/

	long num_neu = num_neu_1 + num_neu_2 + num_neu_3 +
					num_neu_4 + num_neu_5;

	return num_neu;
}

std::ostream& 	
print_noclique(std::ostream& os, long num_col, bool with_rot, long pos_skip = 0, 
			bool with_txt = false)
{
	long ii, mm, cc, jj, mm2, ll;

	long num_nod = num_col + 2;
	long clique_sz = num_col + 1;

	long QQ = 0;
	long num_QQ = (clique_sz * num_nod);
	long RR = QQ + num_QQ;
	long num_RR = (num_nod * num_col);
	long PP = RR + num_RR;
	long num_PP = ((num_nod * (num_nod - 1)) / 2);

	long num_neu = get_num_neu(num_col);
	long num_var = num_QQ + num_RR + num_PP;

	long num_neu_prt = num_neu;
	pos_skip--;
	if(pos_skip >= 0){
		num_neu_prt--;
	}

	os << "c by QUIROGA, Jose. Bogota Colombia. South America."<< std::endl;
	os << "c noclique instance ";
	if(pos_skip < 0){
		os << "(unsat)";
	} else {
		os << "(sat";
		os << " skip = " << (pos_skip + 1);
		os << ") ";
	}
	os << "nn = " << num_nod << " ";
	os << "kk = " << clique_sz << " ";
	os << std::endl;

	os << "c this cnf states the fact that a (kk-1) colorable graph of nn nodes";
	os << std::endl;
	os << "c cannot contain a clique of size kk"<< std::endl;
	os << "p cnf " << num_var << " " << num_neu_prt << std::endl;
	os << std::endl << std::endl;

	long verif_num_neu = 0;

	if(with_txt){
		os << "sum(QQ_mm_ii, ii) >= 1";
		os << "\t\t for 1 <= mm <= kk" << std::endl;
		os << "______________________________________________" << std::endl;
		os << std::endl;
	}
	for(mm = 1; mm <= clique_sz; mm++){
		if(pos_skip != verif_num_neu){
			for(ii = 1; ii <= num_nod; ii++){
				long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
				if(with_rot){
					QQ_mm_ii = prop_var_id(QQ, num_nod, ii, mm);
				}
				if(with_txt){
					os << "QQ." << mm << "." << ii << " = ";
				}
				os << QQ_mm_ii << " ";
			}
			os << " 0" << std::endl;
		}
		verif_num_neu++;
	}
	os << std::endl << std::endl;
	if(with_txt){
		os << "sum(QQ_mm_ii, mm) <= 1";
		os << "\t\t for 1 <= ii <= nn" << std::endl;
		os << "______________________________________________" << std::endl;
		os << std::endl;
	}
	for(ii = 1; ii <= num_nod; ii++){
		for(mm = 1; mm <= clique_sz; mm++){
			long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
			if(with_rot){
				QQ_mm_ii = prop_var_id(QQ, num_nod, ii, mm);
			}
			for(cc = mm + 1; cc <= clique_sz; cc++){
				long QQ_cc_ii = prop_var_id(QQ, clique_sz, cc, ii);
				if(with_rot){
					QQ_cc_ii = prop_var_id(QQ, num_nod, ii, cc);
				}
				if(with_txt){
					os << "-QQ." << mm << "." << ii << " ";
					os << "-QQ." << cc << "." << ii << " ";
					os << " = " << std::endl;
				}
				if(pos_skip != verif_num_neu){
					os  << -QQ_mm_ii << " " 
						<< -QQ_cc_ii << " 0" 
						<< std::endl;
				}
				verif_num_neu++;
			}
		}
		os << std::endl;
	}
	os << std::endl << std::endl;
	
	if(with_txt){
		os << "PP_ii_jj - QQ_mm_ii - QQ_mm2_jj >= -1" << std::endl;
		os << "\t\t for 1 <= ii < jj <= nn,  " << std::endl;
		os << "\t\t for 1 <= mm, mm2 <= kk  ";
		os << "\t\t and mm != mm2" << std::endl;
		os << "______________________________________________" << std::endl;
		os << std::endl;
	}
	for(mm = 1; mm <= clique_sz; mm++){
		for(mm2 = 1; mm2 <= clique_sz; mm2++){
			if(mm != mm2){
				for(ii = 1; ii <= num_nod; ii++){
					long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
					if(with_rot){
						QQ_mm_ii = prop_var_id(QQ, num_nod, ii, mm);
					}
					for(jj = ii + 1; jj <= num_nod; jj++){
						long QQ_mm2_jj = 
							prop_var_id(QQ, clique_sz, mm2, jj);
						if(with_rot){
							QQ_mm2_jj = 
							prop_var_id(QQ, num_nod, jj, mm2);
						}
						long PP_ii_jj = prop_diag_id(PP, ii, jj);
						if(with_txt){
							os << "PP." << ii << "." << jj << " ";
							os << "-QQ." << mm << "." << ii << " ";
							os << "-QQ." << mm2 << "." << jj << " ";
							os << " = " << std::endl;
						}
						if(pos_skip != verif_num_neu){
							os	<< PP_ii_jj << " " 
								<< -QQ_mm_ii << " " 
								<< -QQ_mm2_jj << " 0" 
								<< std::endl;
						}
						verif_num_neu++;
					}
				}
				os << std::endl;
			}
		}
	}
	os << std::endl << std::endl;

	if(with_txt){
		os << "sum(RR_ii_ll, ll) >= 1";
		os << "\t\t for 1 <= ii <= nn" << std::endl;
		os << "______________________________________________" << std::endl;
		os << std::endl;
	}
	for(ii = 1; ii <= num_nod; ii++){
		if(pos_skip != verif_num_neu){
			for(ll = 1; ll <= num_col; ll++){
				long RR_ii_ll = prop_var_id(RR, num_nod, ii, ll);
				if(with_txt){
					os << "RR." << ii << "." << ll << " = ";
				}
				os << RR_ii_ll << " ";
			}
			os << " 0" << std::endl;
		}
		verif_num_neu++;
	}
	os << std::endl << std::endl;

	if(with_txt){
		os << "PP_ii_jj + RR_ii_ll + RR_jj_ll <= 2" << std::endl;
		os << "\t\t for 1 <= ii < jj <= nn,  " << std::endl;
		os << "\t\t for 1 <= ll <= (kk -1) " << std::endl;
		os << "______________________________________________" << std::endl;
		os << std::endl;
	}
	for(ll = 1; ll <= num_col; ll++){
		for(ii = 1; ii <= num_nod; ii++){
			long RR_ii_ll = prop_var_id(RR, num_nod, ii, ll);
			for(jj = ii + 1; jj <= num_nod; jj++){
				long RR_jj_ll = prop_var_id(RR, num_nod, jj, ll);
				long PP_ii_jj = prop_diag_id(PP, ii, jj);
				if(with_txt){
					os << "-PP." << ii << "." << jj << " ";
					os << "-RR." << ii << "." << ll << " ";
					os << "-RR." << jj << "." << ll << " ";
					os << " = " << std::endl;
				}
				if(pos_skip != verif_num_neu){
					os	<< -PP_ii_jj << " " 
						<< -RR_ii_ll << " " 
						<< -RR_jj_ll << " 0" 
						<< std::endl;
				}
				verif_num_neu++;
			}
		}
		os << std::endl;
	}
	os << std::endl << std::endl;
	DBG_CK(verif_num_neu == num_neu);

	return os;
}

void	get_name(std::string& inst_nam, long num_vert, long digs_nm, bool with_rot, long ii, bool with_txt){
	std::string f_nam = "no_clique_v";
	if(with_rot){
		f_nam = "no_clique_rot_v";
	}
	if(with_txt){
		f_nam = "no_clique_txt_v";
	}

	std::ostringstream out_nam;
	out_nam << f_nam;
	out_nam.width(digs_nm);
	out_nam.fill('0');
	out_nam << num_vert;
	if(ii > 0){
		out_nam << "_sat_" << ii;
	}
	out_nam << ".yos";
	inst_nam = out_nam.str();
}

void
generate_noclique(long num_vert, long digs_nm, long ii, bool with_rot, bool with_txt){
	std::string inst_nam;

	long first = 0;
	long last = ii + 1;
	if(ii != 0){
		first = ii;
	} else {
		first = 1;
		last = get_num_neu(num_vert);
	}

	DBG_CK((first >= 0) || ((first + 1) == last));
	for(long kk = first; kk < last; kk++){	
		get_name(inst_nam, num_vert, digs_nm, with_rot, kk, with_txt);

		std::ofstream ff;
		ff.open(inst_nam.c_str());
		if(ff.good() && ff.is_open()){
			print_noclique(ff, num_vert, with_rot, kk, with_txt);
			ff.close();
		}
	}
}

int
main(int argc, char** argv){
	std::ostream& os = std::cout;

	bool with_txt = false;
	bool with_rot = false;
	long ii = -5;
	long first_num = 2;
	long last_num = 3;

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
		os << argv[0] << " <num_vertex> [ii]" << std::endl << std::endl;

		os << "no ii -> unsat with num_vertex." << std::endl;
		os << "ii = -1 -> all unsat with num_vertex or less." << std::endl;
		os << "ii = 0 -> all sat with num_vertex." << std::endl;
		os << "ii > 0 -> sat with num_vertex skipping ii." << std::endl;
		os << "ii = -2 -> print num clauses with num_vertex." << std::endl;
		os << "ii = -3 -> unsat with_txt." << std::endl;
		os << "ii = -4 -> unsat with_rot." << std::endl;

		return 0;
	}

	if(last_num < 2){
		first_num = 2;
		last_num = 3;
	}

	if(ii == -2){
		long num_cla = get_num_neu(first_num);
		os << "num clauses = " << num_cla << std::endl; 
		return 0;
	}
	if(ii == -3){
		with_txt = true;
	}
	if(ii == -4){
		with_rot = true;
	}

	long digs_nm = num_digits(last_num);

	for(long num_vert = first_num; num_vert < last_num; num_vert++){
		generate_noclique(num_vert, digs_nm, ii, with_rot, with_txt);
		os << CARRIAGE_RETURN;
		os << "Generating instance " << num_vert << ".";
		os.flush();
	}
	os << std::endl;
	return 0;
}

