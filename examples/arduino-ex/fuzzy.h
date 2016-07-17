/*
	Author Ahmed El Fakharany afakharany93@gmail.com

	This is a library used to calculate the change required for the output by measuring the feedback element using fuzzy logic control
	error input is percentized	en = ((e - emin)*100)/(emax - emin)
	it takes the number of sets as a constructor input then calculates the the fuzzy sets boundaries 	0.5 range of set = 100/(n+1)
	the sets are numbered from 0 to (n-1), 0 is the number of the most negative value while (n-1) is the set number of the most +ve value
*/

#ifndef __FUZ_ZY__
#define __FUZ_ZY__

#include "Arduino.h"
#include <math.h>

#define UNDEFINED_SET_NUMBER	150

struct membr_set_val
{
	short int set_1;
	float deg_truth_1;

	short int set_2;
	float deg_truth_2;
};

struct op_membr_val
{
	short int set_1;
	float deg_truth_1;

	short int set_2;
	float deg_truth_2;

	short int set_3;
	float deg_truth_3;
};

class fuzzy
{
	private:
		short int _n_membr_sets;
	public:
		int set_point ;
		
		int crisp_ip ;	//crisp input for fuzzy logic i.e. measured value  
		int ip_max ;
		int ip_min ;

		int op_max ;		//output max value
		int op_min ;		//output minimum value

		int error ;
		int error_1 ;	//old error
		int error_max ;
		int error_min ;
		float error_p ;	//percentized error
		

		int ch_error ;
		int ch_error_max ;
		int ch_error_min ;
		float ch_error_p;

		int ch_op_max;
		int ch_op_min;
		int ch_op;
		float ch_op_p;

		int max_st ;	//maximum sampling time milliseconds
		int zero_st;	//sampling time when error equals zero

		struct membr_set_val err_set;
		struct membr_set_val ch_err_set;

		struct op_membr_val ch_op_set;
		
		fuzzy(short int n, int imax, int imin, int omax, int omin);

		float percentizer	 (int val, int val_max, int val_min);
		float depercentizer  (float val, int val_max, int val_min);
		
		/* calculates error percentized and saves value in error_p*/
		float error_calc(int val, int set_val);
		float ch_error_calc (int en, int *en_1);	//calculate percentized change of error
		
		struct membr_set_val membership_determiner(short int n, float val);	//used to determine membership values of a variable
		
		struct op_membr_val ch_op_determiner(short int n, struct membr_set_val ip_mem_1, struct membr_set_val ip_mem_2);
		
		float defuzzifier (short int n, struct op_membr_val u);

		int fuzzy_controller(int input, int s_point);

		int sample_t_det ();
};

#endif
