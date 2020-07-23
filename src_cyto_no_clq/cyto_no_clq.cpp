
/*************************************************************

gen_noclingra.cpp -- 
(C) QUIROGA, Jose L. Bogota Colombia. South America

funcs for no_clique sat instance generation.

--------------------------------------------------------------*/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

#define CARRIAGE_RETURN		((char)13)

#define DBG_CK(prm) 	assert(prm)

// NOCLIQUE PRT_FUNCS

#define PI_VAL 3.14159265
#define AS_RADIANS(angle) ((angle * PI_VAL) / 180)
#define AS_DEGREES(radian) ((radian * 180) / PI_VAL)

long MIN_R_CIR = 100;

class xy_vec;
class ra_vec;

class xy_vec {
public:
	
	double x_coor;
	double y_coor;

	xy_vec(){
		init_xy_vec(0, 0);
	}
	
	void init_xy_vec(double x_val, double y_val){
		x_coor = x_val;
		y_coor = y_val;
	}
	
	ra_vec as_ra_vec();
	
	void plus(xy_vec v2){
		x_coor += v2.x_coor;
		y_coor += v2.y_coor;
	}

	void minus(xy_vec v2){
		x_coor -= v2.x_coor;
		y_coor -= v2.y_coor;
	}
	
	void neg(){
		x_coor = -x_coor;
		y_coor = -y_coor;
	}

	void divided(double val){
		x_coor /= val;
		y_coor /= val;
	}	

	void multiplied(double val){
		x_coor *= val;
		y_coor *= val;
	}		
};

class ra_vec {
public:
	
	double r_coor;
	double a_coor;

	ra_vec(){
		init_ra_vec(1, 0);
	}
	
	void init_ra_vec(double r_val, double a_val){
		r_coor = r_val;
		a_coor = a_val;
	}
	
	xy_vec as_xy_vec();
};

ra_vec
xy_vec::as_ra_vec(){
	ra_vec pvec;
	pvec.r_coor = sqrt((x_coor * x_coor) + (y_coor * y_coor));
	pvec.a_coor = AS_DEGREES(atan2(y_coor, x_coor));
	return pvec;
}

xy_vec
ra_vec::as_xy_vec(){
	xy_vec cvec;
	cvec.x_coor = r_coor * cos(AS_RADIANS(a_coor));
	cvec.y_coor = r_coor * sin(AS_RADIANS(a_coor));
	return cvec;
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
print_cy_circle(std::ostream& os)
{
	os << "the_cnf_to_draw = {"<< std::endl;
	os << "\t nodes: ["<< std::endl;
	
	xy_vec tv1;
	xy_vec tv2;
	
	long rr = 200;
	for(long aa = 0; aa < 360; aa += 10){
		ra_vec v1;
		v1.r_coor = rr;
		v1.a_coor = aa;
		xy_vec v2 = v1.as_xy_vec();
		
		if(aa == 40){ tv1 = v2; }
		if(aa == 50){ tv2 = v2; }
		
		os << "\t\t //aa=" << aa << std::endl;
		os << "\t\t ";
		if(aa > 0){ os << ","; }		
		os << "{ data: { id: 'd" << aa << "'}, ";
		os << "position:{x:" << v2.x_coor << ", y:" << v2.y_coor << "}, classes:'dipole' }";
		os << std::endl;
	}
	
	xy_vec tv3 = tv2;
	tv3.minus(tv1);
	tv3.divided(2);
	ra_vec tv4 = tv3.as_ra_vec();
	tv4.a_coor -= 90;
	xy_vec tv5 = tv4.as_xy_vec();
	tv5.multiplied(3);
	tv1.plus(tv3);
	tv1.plus(tv5);
	
	os << ",{ data: { id: 'd" << 500 << "'}, ";
	os << "position:{x:" << tv1.x_coor << ", y:" << tv1.y_coor << "}, classes:'dipole' }";
	os << std::endl;
	
	os << "\t ]";
	os << std::endl;
	os << "};";
	os << std::endl;
}

double get_r_circle(long num_col){
	return (num_col * MIN_R_CIR);
}

xy_vec 
get_grad_vec(xy_vec tv1, xy_vec tv2, double mul){
	xy_vec dlt_vc = tv2;
	dlt_vc.minus(tv1);
	dlt_vc.divided(2);
	ra_vec pol_vc = dlt_vc.as_ra_vec();
	pol_vc.a_coor += 90;
	xy_vec grd_vc = pol_vc.as_xy_vec();
	grd_vc.multiplied(mul);
	tv1.plus(dlt_vc);
	tv1.plus(grd_vc);
	return tv1;
}

xy_vec 
get_mid_vec(xy_vec tv1, xy_vec tv2){
	xy_vec dlt_vc = tv2;
	dlt_vc.minus(tv1);
	dlt_vc.divided(2);
	
	tv1.plus(dlt_vc);
	return tv1;
}

xy_vec 
get_delta_vec(xy_vec tv1, xy_vec tv2, long tot, long ii){
	xy_vec dlt_vc = tv2;
	dlt_vc.minus(tv1);
	dlt_vc.divided(tot);
	dlt_vc.multiplied(ii);
	
	tv1.plus(dlt_vc);
	return tv1;
}

xy_vec 
get_qq_pos(long num_col, long mm, long ii){
	long clique_sz = num_col + 1;
	long num_nod = num_col + 2;
	long num_QQ = (clique_sz * num_nod);
	long QQ_mm_ii = prop_var_id(0, clique_sz, mm, ii);
	
	double del_grp = ((double)360) / ((double)num_nod);
	double a_grp = del_grp * ii;
	double del_qqs = (del_grp / (2 * (clique_sz + 1)));
	
	ra_vec pos1;
	pos1.r_coor = get_r_circle(num_col);
	pos1.a_coor = a_grp + ((double)mm * del_qqs);
	return pos1.as_xy_vec();
}

xy_vec 
get_rr_pos(long num_col, long ii, long ll){
	long clique_sz = num_col + 1;
	long num_nod = num_col + 2;
	long num_RR = (num_nod * num_col);
	long RR_ii_ll = prop_var_id(0, num_nod, ii, ll);
	
	double r_cir = get_r_circle(num_col);
	double rr_cir = 1.2 * r_cir; 
	double sep_pp = ((rr_cir * 2) / num_RR);
	double pp_x_pos = - rr_cir;
	double pp_y_pos = 2.0 * r_cir;
	
	xy_vec pos1;
	pos1.x_coor = pp_x_pos + (RR_ii_ll * sep_pp);
	pos1.y_coor = pp_y_pos;
	return pos1;
}

std::ostream&
print_noclique_nodes(std::ostream& os, long num_col, long pos_skip = 0)
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

	os << "// by QUIROGA, Jose. Bogota Colombia. South America."<< std::endl;
	os << "// noclique instance ";
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

	os << "// this cnf states the fact that a (kk-1) colorable graph of nn nodes";
	os << std::endl;
	os << "// cannot contain a clique of size kk"<< std::endl;
	os << "// cnf num_var=" << num_var << " num_neu=" << num_neu_prt << std::endl;
	os << std::endl << std::endl;

	os << "the_cnf_to_draw = ["<< std::endl;
	
	long qua_ii = 0;
	for(mm = 1; mm <= clique_sz; mm++){
		for(ii = 1; ii <= num_nod; ii++){
			long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
			xy_vec qq_pos = get_qq_pos(num_col, mm, ii);
			
			os << "// QQ mm=" << mm << " ii=" << ii << std::endl;
			os << "";
			if(qua_ii > 0){ os << ","; }		
			os << "{ data: { id: 'd" << QQ_mm_ii << "'}, group:'nodes', ";
			os << "position:{x:" << qq_pos.x_coor << ", y:" << qq_pos.y_coor << "}";
			os << ", classes:'dipole' }";
			os << std::endl;
			qua_ii++;
		}
	}
	os << std::endl << std::endl;
	
	long pp_id = PP;
	double r_cir = get_r_circle(num_col);;
	double pp_cir = 1.2 * r_cir; 
	double sep_pp = ((pp_cir * 2) / num_PP); 
	double pp_x_pos = - pp_cir;
	double pp_y_pos = 2 * r_cir;
	for(long pp = 1; pp <= num_PP; pp++){
		pp_x_pos += sep_pp;
		os << ",{ data: { id: 'd" << PP + pp << "'}, group:'nodes', ";
		os << "position:{x:" << pp_x_pos << ", y:" << pp_y_pos << "}";
		os << ", classes:'dipole' }";
		os << std::endl;
	}
	os << std::endl << std::endl;
	os << std::endl << std::endl;
	
	for(ii = 1; ii <= num_nod; ii++){
		for(ll = 1; ll <= num_col; ll++){
			long RR_ii_ll = prop_var_id(RR, num_nod, ii, ll);
			xy_vec rr_pos = get_rr_pos(num_col, ii, ll);
			
			os << "// RR ii=" << ii << " ll=" << ll << std::endl;
			os << ",{ data: { id: 'd" << RR_ii_ll << "'}, group:'nodes', ";
			os << "position:{x:" << rr_pos.x_coor << ", y:" << rr_pos.y_coor << "}";
			os << ", classes:'dipole' }";
			os << std::endl;
		}
	}
	os << std::endl << std::endl;
	
	double nn_r_sep = 1.8 * r_cir; 
	double nn_a_sep = ((double)360) / ((double)clique_sz);
	double nn_a_coor = nn_a_sep / 2;
	long ii_qua = 0;
	long ii_neu = 0;
	xy_vec neu_pos;
	for(mm = 1; mm <= clique_sz; mm++){
		ra_vec nn_pos;
		nn_pos.a_coor = nn_a_coor;
		nn_pos.r_coor = nn_r_sep;
		nn_a_coor += nn_a_sep;
		neu_pos = nn_pos.as_xy_vec();
		
		os << ",{ data: { id: 'n" << ii_neu << "'}, group:'nodes', ";
		os << "position:{x:" << neu_pos.x_coor << ", y:" << neu_pos.y_coor << "}";
		os << ", classes:'neuron' }";
		os << std::endl;
		
		if(pos_skip != ii_neu){
			for(ii = 1; ii <= num_nod; ii++){
				long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
				
				os << ",{ data: { id: 'q" << ii_qua << "', source: 'n" << ii_neu;
				os << "', target: 'd" << QQ_mm_ii << "'}, group:'edges', classes: 'in_pos' }";
				os << std::endl;
				ii_qua++;
				
				//os << QQ_mm_ii << " ";
			}
		}
		ii_neu++;
	}
	os << std::endl << std::endl;
	
	for(ii = 1; ii <= num_nod; ii++){
		for(mm = 1; mm <= clique_sz; mm++){
			long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
			xy_vec qq_mm_ii_pos = get_qq_pos(num_col, mm, ii);
			for(cc = mm + 1; cc <= clique_sz; cc++){
				long QQ_cc_ii = prop_var_id(QQ, clique_sz, cc, ii);
				xy_vec qq_cc_ii_pos = get_qq_pos(num_col, cc, ii);
				if(pos_skip != ii_neu){
					neu_pos = get_grad_vec(qq_cc_ii_pos, qq_mm_ii_pos, 1);
					
					os << ",{ data: { id: 'n" << ii_neu << "'}, group:'nodes', ";
					os << "position:{x:" << neu_pos.x_coor << ", y:" << neu_pos.y_coor << "}";
					os << ", classes:'neuron' }";
					os << std::endl;
					
					os << ",{ data: { id: 'q" << ii_qua << "', ";
					os << "source: 'n" << ii_neu << "', ";
					os << "target: 'd" << QQ_mm_ii << "'}, ";
					os << "group:'edges', classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;
					
					os << ",{ data: { id: 'q" << ii_qua << "', ";
					os << "source: 'n" << ii_neu << "', ";
					os << "target: 'd" << QQ_cc_ii << "'}, ";
					os << "group:'edges', classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;

					//os  << -QQ_mm_ii << " " 
					//	<< -QQ_cc_ii << " 0" 
					//	<< std::endl;
				}
				ii_neu++;
			}
		}
		os << std::endl;
	}
	os << std::endl << std::endl;
	
	for(mm = 1; mm <= clique_sz; mm++){
		for(mm2 = 1; mm2 <= clique_sz; mm2++){
			if(mm != mm2){
				for(ii = 1; ii <= num_nod; ii++){
					long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
					xy_vec qq_mm_ii_pos = get_qq_pos(num_col, mm, ii);
					for(jj = ii + 1; jj <= num_nod; jj++){
						long QQ_mm2_jj = 
							prop_var_id(QQ, clique_sz, mm2, jj);
						xy_vec qq_mm2_jj_pos = get_qq_pos(num_col, mm2, jj);
						long PP_ii_jj = prop_diag_id(PP, ii, jj);
						if(pos_skip != ii_neu){
							xy_vec neu_pos = get_mid_vec(qq_mm_ii_pos, qq_mm2_jj_pos);
								
							os << ",{ data: { id: 'n" << ii_neu << "'}, group:'nodes', ";
							os << "position:{x:" << neu_pos.x_coor << ", ";
							os << "y:" << neu_pos.y_coor << "}";
							os << ", classes:'neuron' }";
							os << std::endl;
								
							os << ",{ data: { id: 'q" << ii_qua << "', ";
							os << "source: 'n" << ii_neu << "', ";
							os << "target: 'd" << QQ_mm_ii << "'}, ";
							os << "group:'edges', classes: 'in_neg' }";
							os << std::endl;
							ii_qua++;
							
							os << ",{ data: { id: 'q" << ii_qua << "', ";
							os << "source: 'n" << ii_neu << "', ";
							os << "target: 'd" << QQ_mm2_jj << "'}, ";
							os << "group:'edges', classes: 'in_neg' }";
							os << std::endl;
							ii_qua++;

							os << ",{ data: { id: 'q" << ii_qua << "', ";
							os << "source: 'n" << ii_neu << "', ";
							os << "target: 'd" << PP_ii_jj << "'}, ";
							os << "group:'edges', classes: 'in_pos' }";
							os << std::endl;
							ii_qua++;

							//os	<< PP_ii_jj << " " 
							//	<< -QQ_mm_ii << " " 
							//	<< -QQ_mm2_jj << " 0" 
							//	<< std::endl;
						}
						ii_neu++;
					}
				}
				os << std::endl;
			}
		}
	}
	os << std::endl << std::endl;

	double b_width = 1.2 * r_cir; 
	double sep_rr = ((b_width * 2) / num_nod);
	double rr_x_pos = -b_width - (sep_rr / 2);
	double rr_y_pos = 2.5 * r_cir;
	
	for(ii = 1; ii <= num_nod; ii++){
		rr_x_pos += sep_rr;
		if(pos_skip != ii_neu){
			
			os << ",{ data: { id: 'n" << ii_neu << "'}, group:'nodes', ";
			os << "position:{x:" << rr_x_pos << ", ";
			os << "y:" << rr_y_pos << "}";
			os << ", classes:'neuron' }";
			os << std::endl;
			
			for(ll = 1; ll <= num_col; ll++){
				long RR_ii_ll = prop_var_id(RR, num_nod, ii, ll);
				xy_vec rr_ii_ll_pos = get_rr_pos(num_col, ii, ll);
				
				os << ",{ data: { id: 'q" << ii_qua << "', ";
				os << "source: 'n" << ii_neu << "', ";
				os << "target: 'd" << RR_ii_ll << "'}, ";
				os << "group:'edges', classes: 'in_pos' }";
				os << std::endl;
				ii_qua++;
				//os << RR_ii_ll << " ";
			}
		}
		ii_neu++;
	}
	os << std::endl << std::endl;
	

	for(ll = 1; ll <= num_col; ll++){
		for(ii = 1; ii <= num_nod; ii++){
			long RR_ii_ll = prop_var_id(RR, num_nod, ii, ll);
			xy_vec rr_ii_ll_pos = get_rr_pos(num_col, ii, ll);
			for(jj = ii + 1; jj <= num_nod; jj++){
				long RR_jj_ll = prop_var_id(RR, num_nod, jj, ll);
				xy_vec rr_jj_ll_pos = get_rr_pos(num_col, jj, ll);
				long PP_ii_jj = prop_diag_id(PP, ii, jj);
				if(pos_skip != ii_neu){
					neu_pos = get_grad_vec(rr_jj_ll_pos, rr_ii_ll_pos, 0.5);
					
					os << ",{ data: { id: 'n" << ii_neu << "'}, group:'nodes', ";
					os << "position:{x:" << neu_pos.x_coor << ", ";
					os << "y:" << neu_pos.y_coor << "}";
					os << ", classes:'neuron' }";
					os << std::endl;
					
					os << ",{ data: { id: 'q" << ii_qua << "', ";
					os << "source: 'n" << ii_neu << "', ";
					os << "target: 'd" << RR_ii_ll << "'}, ";
					os << "group:'edges', classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;
					
					os << ",{ data: { id: 'q" << ii_qua << "', ";
					os << "source: 'n" << ii_neu << "', ";
					os << "target: 'd" << RR_jj_ll << "'}, ";
					os << "group:'edges', classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;

					os << ",{ data: { id: 'q" << ii_qua << "', ";
					os << "source: 'n" << ii_neu << "', ";
					os << "target: 'd" << PP_ii_jj << "'}, ";
					os << "group:'edges', classes: 'in_neg' }";
					os << std::endl;
					ii_qua++;

					//os	<< -PP_ii_jj << " " 
					//	<< -RR_ii_ll << " " 
					//	<< -RR_jj_ll << " 0" 
					//	<< std::endl;
				}
				ii_neu++;
			}
		}
		os << std::endl;
	}
	os << std::endl << std::endl;
	
	//DBG_CK(ii_neu == num_neu);
	

	os << "];";
	os << std::endl;
	
	return os;
}

std::ostream&
print_noclique_edges(std::ostream& os, long num_col, long pos_skip = 0, 
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

	long verif_num_neu = 0;

	if(with_txt){
		os << "// sum(QQ_mm_ii, ii) >= 1";
		os << "\t\t for 1 <= mm <= kk" << std::endl;
		os << "// ______________________________________________" << std::endl;
		os << std::endl;
	}
	for(mm = 1; mm <= clique_sz; mm++){
		if(pos_skip != verif_num_neu){
			if(with_txt){ os << "// "; }
			for(ii = 1; ii <= num_nod; ii++){
				long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
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
		os << "// sum(QQ_mm_ii, mm) <= 1";
		os << "\t\t for 1 <= ii <= nn" << std::endl;
		os << "// ______________________________________________" << std::endl;
		os << std::endl;
	}
	for(ii = 1; ii <= num_nod; ii++){
		for(mm = 1; mm <= clique_sz; mm++){
			long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
			for(cc = mm + 1; cc <= clique_sz; cc++){
				long QQ_cc_ii = prop_var_id(QQ, clique_sz, cc, ii);
				if(with_txt){
					os << "// -QQ." << mm << "." << ii << " ";
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
		os << "// PP_ii_jj - QQ_mm_ii - QQ_mm2_jj >= -1" << std::endl;
		os << "// \t\t for 1 <= ii < jj <= nn,  " << std::endl;
		os << "// \t\t for 1 <= mm, mm2 <= kk  ";
		os << "\t\t and mm != mm2" << std::endl;
		os << "// ______________________________________________" << std::endl;
		os << std::endl;
	}
	for(mm = 1; mm <= clique_sz; mm++){
		for(mm2 = 1; mm2 <= clique_sz; mm2++){
			if(mm != mm2){
				for(ii = 1; ii <= num_nod; ii++){
					long QQ_mm_ii = prop_var_id(QQ, clique_sz, mm, ii);
					for(jj = ii + 1; jj <= num_nod; jj++){
						long QQ_mm2_jj = 
							prop_var_id(QQ, clique_sz, mm2, jj);
						long PP_ii_jj = prop_diag_id(PP, ii, jj);
						if(with_txt){
							os << "// PP." << ii << "." << jj << " ";
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
		os << "// sum(RR_ii_ll, ll) >= 1";
		os << "\t\t for 1 <= ii <= nn" << std::endl;
		os << "// ______________________________________________" << std::endl;
		os << std::endl;
	}
	for(ii = 1; ii <= num_nod; ii++){
		if(pos_skip != verif_num_neu){
			if(with_txt){ os << "// "; }
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
		os << "// PP_ii_jj + RR_ii_ll + RR_jj_ll <= 2" << std::endl;
		os << "// \t\t for 1 <= ii < jj <= nn,  " << std::endl;
		os << "// \t\t for 1 <= ll <= (kk -1) " << std::endl;
		os << "// ______________________________________________" << std::endl;
		os << std::endl;
	}
	for(ll = 1; ll <= num_col; ll++){
		for(ii = 1; ii <= num_nod; ii++){
			long RR_ii_ll = prop_var_id(RR, num_nod, ii, ll);
			for(jj = ii + 1; jj <= num_nod; jj++){
				long RR_jj_ll = prop_var_id(RR, num_nod, jj, ll);
				long PP_ii_jj = prop_diag_id(PP, ii, jj);
				if(with_txt){
					os << "// -PP." << ii << "." << jj << " ";
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

void	get_name(std::string& inst_nam, long num_vert, long digs_nm, long ii, bool with_txt){
	std::string f_nam = "cyto_no_clique_v";
	if(with_txt){
		f_nam = "cyto_no_clique_txt_v";
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
generate_noclique(long num_vert, long digs_nm, long ii, bool with_txt){
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
		get_name(inst_nam, num_vert, digs_nm, kk, with_txt);

		std::ofstream ff;
		ff.open(inst_nam.c_str());
		if(ff.good() && ff.is_open()){
			print_noclique_nodes(ff, num_vert, kk);
			//print_cy_circle(ff);
			//print_noclique(ff, num_vert, kk, with_txt);
			ff.close();
		}
	}
}

int
main(int argc, char** argv){
	std::ostream& os = std::cout;

	bool with_txt = false;
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

	long digs_nm = num_digits(last_num);

	for(long num_vert = first_num; num_vert < last_num; num_vert++){
		generate_noclique(num_vert, digs_nm, ii, with_txt);
		os << CARRIAGE_RETURN;
		os << "Generating instance " << num_vert << ".";
		os.flush();
	}
	os << std::endl;
	return 0;
}

