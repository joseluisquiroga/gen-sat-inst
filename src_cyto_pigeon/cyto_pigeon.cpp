
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
	
	//long bse_y = 80;
	long bse_y = 0;
	long sep_x = 50;
	long sep_y = 40;
	long hlf_x = 25;
	long h_width = (pigeons * sep_x);

	long num_neu_prt = num_neu;
	pos_skip--;
	if(pos_skip >= 0){
		num_neu_prt--;
	}

	long y_pos = 0;
	long x_pos = 0;
	
	bool in_fst = 1;
	
	os << "the_cnf_to_draw = {"<< std::endl;
	os << "\t nodes: ["<< std::endl;
	y_pos = bse_y;
	for(ii = 0; ii < num_var; ii++){
		x_pos = ii * sep_x;
		os << "\t\t ";
		if(ii > 0){ os << ","; }		
		os << "{ data: { id: 'd" << (ii + 1) << "'}, ";
		os << "position:{x:" << x_pos << ", y:" << y_pos << "}, classes:'dipole' }";
		os << std::endl;
	}
	os << std::endl;
	
	long ii_neu = 0; 
	long p_x_pos = 0;
	x_pos = 0;
	y_pos = 0;
	for(ii = 0; ii < holes; ii++){
		p_x_pos = ii * h_width;
		for(jj = 1; jj <= pigeons; jj++){
			long n_y = 0;
			y_pos = bse_y;
			x_pos = p_x_pos + ((jj - 1) * sep_x);
			for(kk = jj + 1; kk <= pigeons; kk++){
				x_pos += hlf_x;
				if((n_y % 2) == 0){
					y_pos -= sep_y;
				}
				if(pos_skip != ii_neu){
					os << "\t\t ,{ data: { id: 'n" << ii_neu << "'}, ";
					os << "position:{x:" << x_pos << ", y:" << y_pos << "}, ";
					os << "classes:'neuron' }";
					os << std::endl;
				}
				ii_neu++;
				n_y++;
			}
		}
		os << std::endl;
	}
	
	long full_width = h_width * holes;
	long x_p_sep = full_width / pigeons;
	x_pos = -(h_width / 2);
	y_pos = bse_y + (holes * sep_y);
	for(jj = 1; jj <= pigeons; jj++){
		x_pos += x_p_sep;
		if(pos_skip != ii_neu){
			os << "\t\t ,{ data: { id: 'n" << ii_neu << "'}, ";
			os << "position:{x:" << x_pos << ", y:" << y_pos << "}, ";
			os << "classes:'neuron' }";
			os << std::endl;
		}
		ii_neu++;
	}

	if(with_redun){
		os << std::endl << std::endl << std::endl;
		long y_bse_rdn = bse_y - (((holes / 2) + 1) * sep_y);
		for(pp = 1; pp <= pigeons; pp++){
			for(hh1 = 0; hh1 < holes; hh1++){
				long var1 = (hh1 * pigeons) + pp;
				for(hh2 = hh1 + 1; hh2 < holes; hh2++){
					long var2 = (hh2 * pigeons) + pp;
					long x_pos_v1 = (var1 - 1) * sep_x;
					long x_pos_v2 = (var2 - 1) * sep_x;
					x_pos = (x_pos_v1 + x_pos_v2) / 2;
					y_pos = y_bse_rdn - (((var2 - var1) / pigeons) * sep_y);

					os << "\t\t ,{ data: { id: 'n" << ii_neu << "'}, ";
					os << "position:{x:" << x_pos << ", y:" << y_pos << "}, ";
					os << "classes:'neuron' }";
					os << std::endl;
					
					ii_neu++;
				}
			}
			os << std::endl;
		}
		os << std::endl;
		
		x_pos = - (h_width / 2);
		y_pos = bse_y + ((holes + 2) * sep_y);
		for(hh1 = 0; hh1 < holes; hh1++){
			x_pos += h_width;
			os << "\t\t ,{ data: { id: 'n" << ii_neu << "'}, ";
			os << "position:{x:" << x_pos << ", y:" << y_pos << "}, ";
			os << "classes:'neuron' }";
			os << std::endl;
			ii_neu++;
		}
	}
	
	os << std::endl;
	os << "\t ],";
	os << std::endl;
		
	os << "\t edges: ["<< std::endl;

	ii_neu = 0;
	long ii_qua = 0; 
	long verif_num_neu = 0;
	for(ii = 0; ii < holes; ii++){
		for(jj = 1; jj <= pigeons; jj++){
			long var1 = (ii * pigeons) + jj;
			for(kk = jj + 1; kk <= pigeons; kk++){
				long var2 = (ii * pigeons) + kk;
				if(pos_skip != verif_num_neu){
					os << "\t\t ";
					if(ii_qua > 0){ os << ","; }
					os << "{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
					os << "', target: 'd" << var1 << "'}, classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;
					os << "\t\t ,{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
					os << "', target: 'd" << var2 << "'}, classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;
				}
				verif_num_neu++;
				ii_neu++;
			}
		}
		os << std::endl;
	}
	os << std::endl;
	os << std::endl;
	
	for(jj = 1; jj <= pigeons; jj++){
		if(pos_skip != verif_num_neu){
			for(ii = 0; ii < holes; ii++){
				long var1 = (ii * pigeons) + jj;
				os << "\t\t ,{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
				os << "', target: 'd" << var1 << "'}, classes: 'in_pos' }";
				os << std::endl;
				ii_qua++;
			}
		}
		verif_num_neu++;
		ii_neu++;
	}
	

	if(with_redun){
		os << std::endl << std::endl << std::endl;
		for(pp = 1; pp <= pigeons; pp++){
			for(hh1 = 0; hh1 < holes; hh1++){
				long var1 = (hh1 * pigeons) + pp;
				for(hh2 = hh1 + 1; hh2 < holes; hh2++){
					long var2 = (hh2 * pigeons) + pp;
					os << "\t\t ,{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
					os << "', target: 'd" << var1 << "'}, classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;
					os << "\t\t ,{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
					os << "', target: 'd" << var2 << "'}, classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;
					//os << -var1 << " " << -var2 << " 0" << std::endl;
					verif_num_neu++;
					ii_neu++;
				}
			}
			os << std::endl;
		}
		os << std::endl;
		
		for(hh1 = 0; hh1 < holes; hh1++){
			for(pp = 1; pp <= pigeons; pp++){
				long var1 = (hh1 * pigeons) + pp;
				os << "\t\t ,{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
				os << "', target: 'd" << var1 << "'}, classes: 'in_pos' }";
				os << std::endl;
				ii_qua++;
				//os << var1 << " ";
			}
			//os << " 0" << std::endl;
			verif_num_neu++;
			ii_neu++;
		}
	}
	DBG_CK(verif_num_neu == num_neu);
	
	os << "\t ]";
	os << std::endl;
	os << "};";
	os << std::endl;
	
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

void	get_name(std::string& inst_nam, long holes, long digs_nm, long ii, long n_dig, bool with_redun){
	std::string f_nam = "cy_h";	// by hole

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
generate_pigeon(long holes, long digs_nm, long ii, bool with_redun = false){
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
		get_name(inst_nam, holes, digs_nm, kk, num_dig, with_redun);

		std::ofstream ff;
		ff.open(inst_nam.c_str());
		if(ff.good() && ff.is_open()){
			print_php_by_hole(ff, holes, kk, with_redun);
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
		generate_pigeon(num_holes, digs_nm, ii, with_redun);
		os << CARRIAGE_RETURN;
		os << "Generating instance " << num_holes << ".";
		os.flush();
	}
	os << std::endl;
	return 0;
}

