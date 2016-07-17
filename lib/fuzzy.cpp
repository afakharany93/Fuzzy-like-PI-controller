/*
	Author Ahmed El Fakharany afakharany93@gmail.com

	This is a library used to calculate the change required for the output by measuring the feedback element using fuzzy logic control
	error input is percentized	en = ((e - emin)*100)/(emax - emin)
	it takes the number of sets as a constructor input then calculates the the fuzzy sets boundaries 	0.5 range of set = 100/(n+1)
	the sets are numbered from 0 to (n-1), 0 is the number of the most negative value while (n-1) is the set number of the most +ve value
*/

#include "fuzzy.h"


fuzzy::fuzzy(short int n, int imax, int imin, int omax, int omin)
{
	if(n%2 == 0)	//number of sets must be odd
	{
		_n_membr_sets = n+1;
	}
	else
	{
		_n_membr_sets = n;
	}

	set_point = 0;
		
	crisp_ip = 0;	//crisp input for fuzzy logic i.e. measured value  
	ip_max = imax;
	ip_min = imin;

	op_max = 0;		//output max value
	op_min = 0;		//output minimum value

	error = 0;
	error_max = ip_max - ip_min;
	error_min = ip_min - ip_max;
	error_p = 0.0;	//percentized error
	error_1 = error_min;	//old error
	

	ch_error = 0;
	ch_error_max = error_max - error_min;
	ch_error_min = error_min - error_max;
	ch_error_p = 0.0;

	ch_op_max = omax - omin;
	ch_op_min = omin - omax;
	ch_op = 0;
	ch_op_p = 0.0;

	max_st = 500;	//maximum sampling time milliseconds
	zero_st = 10;	//sampling time when error equals zero
}


float fuzzy::percentizer (int val, int val_max, int val_min)
{
	long nem = val - val_min;
	nem = nem * 100;
	long den = (val_max - val_min);
	float res =   nem / den;
	/*Serial.print( "val:  ");
  	Serial.println(val);
  	Serial.print( "val_min:  ");
  	Serial.println(val_min);
  	Serial.print( "val - val_min:  ");
  	Serial.println((val - val_min));
	Serial.print( "nem:  ");
  	Serial.println(nem);
  	Serial.print( "den:  ");
  	Serial.println(den);*/
	return(res);
}

float fuzzy::depercentizer (float val, int val_max, int val_min)
{
	float nem = val_max - val_min;
	nem = nem * val;
	float res = nem / 100;
	res = res + val_min;
	return( res );
}

float fuzzy::error_calc(int val, int set_val)
{
	float res;
	error = set_val - val;
	res = percentizer (error, error_max, error_min);
	return (res);
}

//honey here is a mistake, don't calculate the change of error by using normalized en and en-1, calculate it before normalization
float fuzzy::ch_error_calc (int en, int *en_1)	//calculate percentized change of error
{
	float res;
	ch_error = en - *en_1;
	*en_1 = en;	//make current error old error for next round
	res = percentizer (ch_error, ch_error_max, ch_error_min);
	return (res);
}

struct membr_set_val fuzzy::membership_determiner(short int n, float val)
{
	struct membr_set_val u = {UNDEFINED_SET_NUMBER, UNDEFINED_SET_NUMBER, UNDEFINED_SET_NUMBER, UNDEFINED_SET_NUMBER};	//used to hold membership values, initialized with undefined value

	short int count;
	float b_range = 100/(n+1);

	float a;
	float b;
	float c;

	float u_val;

	float den = 0;
	float nem = 0;

	for (count = 0; count < n; count ++)	//loop all fuzzy sets, number of sets is determined by the user
	{
		b =  (count * b_range);	//calculate center of every set, each set at a time
		b = b + b_range;
		if (count == 0 && val <= b)						//if its the most negative set & if its the most negative set and the value is less that the center of it, then its a full member of the most negative set and get out of the loop
		{	
			u.set_1 = count;
			u.deg_truth_1 = 100;
			u.set_2 = UNDEFINED_SET_NUMBER;
			u.deg_truth_2 = UNDEFINED_SET_NUMBER;
			break;
		}
		else if (count == (n-1) && val >= b)	//if it is the most +ve set & if it is the most +ve set and the value is more that the center, then its a full member of that set and get out of the loop 
		{
			u.set_1 = count;
			u.deg_truth_1 = 100;
			u.set_2 = UNDEFINED_SET_NUMBER;
			u.deg_truth_2 = UNDEFINED_SET_NUMBER;
			break;
		}
		else	//if value belongs to the rest of the sets
		{
			a = b - b_range;	//get both a and c
			c = b + b_range;
			if(val <= b && val > a) //if val is between both a and b
			{
				//u_val = (val - a)*100/(b - a);
				den = b -a;
				nem = val - a;
				nem = nem * 100;
				u_val = nem / den;
				if(u_val > 0 && u_val <= 100)	//if the degree of truth is within range
				{
					if(u.set_1 == UNDEFINED_SET_NUMBER)	//if set_1 is not taken, take it
					{
						u.set_1 = count;
						u.deg_truth_1 = u_val;
					}
					else if (u.set_2 == UNDEFINED_SET_NUMBER) //if set_1 is taken and if set_2 is not taken, take it
					{
						u.set_2 = count;
						u.deg_truth_2 = u_val;
					}
				}
			}
			else if(val > b && val < c) //if val is between both c and b
			{
				//u_val = (c - val)*100 / (c - b);	//like before but with c and b
				den = c - b;
				nem = c - val;
				nem = nem * 100;
				u_val = nem / den;
				if(u_val > 0 && u_val <= 100)
				{
					if(u.set_1 == UNDEFINED_SET_NUMBER)
					{
						u.set_1 = count;
						u.deg_truth_1 = u_val;
					}
					else if (u.set_2 == UNDEFINED_SET_NUMBER)
					{
						u.set_2 = count;
						u.deg_truth_2 = u_val;
					}
				}
			}
		}
		if(u.set_1 != UNDEFINED_SET_NUMBER && u.set_2 != UNDEFINED_SET_NUMBER)	//variable will have values in at most 2 sets, no need to continue looping if they are obtained
		{
			break;
		}
	}
	return (u);
}

struct op_membr_val fuzzy::ch_op_determiner(short int n, struct membr_set_val ip_mem_1, struct membr_set_val ip_mem_2)
{
	//short int dia_n = (2*n)-1;	//number of diagonals in the table, which is also the number of ch op sets, always an odd number
	short int dia = 0;	//used to hold the number of the current diagonal
	float dia_val = 0;

	struct op_membr_val u = {UNDEFINED_SET_NUMBER, UNDEFINED_SET_NUMBER, UNDEFINED_SET_NUMBER, UNDEFINED_SET_NUMBER, UNDEFINED_SET_NUMBER, UNDEFINED_SET_NUMBER};

	if((ip_mem_1.set_1 != UNDEFINED_SET_NUMBER) && (ip_mem_2.set_1 != UNDEFINED_SET_NUMBER))
	{
		dia = ip_mem_1.set_1 + ip_mem_2.set_1;
		if(ip_mem_1.deg_truth_1 < ip_mem_2.deg_truth_1)
		{
			dia_val = ip_mem_1.deg_truth_1;
		}
		else
		{
			dia_val = ip_mem_2.deg_truth_1;
		}

		u.set_1 = dia;
		u.deg_truth_1 = dia_val;
	}
	if((ip_mem_1.set_1 != UNDEFINED_SET_NUMBER) && (ip_mem_2.set_2 != UNDEFINED_SET_NUMBER))
	{
		dia = ip_mem_1.set_1 + ip_mem_2.set_2;
		if(ip_mem_1.deg_truth_1 < ip_mem_2.deg_truth_2)
		{
			dia_val = ip_mem_1.deg_truth_1;
		}
		else
		{
			dia_val = ip_mem_2.deg_truth_2;
		}

		if(dia == u.set_1)
		{
			if(dia_val > u.deg_truth_1)
			{
				u.deg_truth_1 = dia_val;
			}
		}
		else
		{
			u.set_2 = dia;
			u.deg_truth_2 = dia_val;
		}
	}
	if((ip_mem_1.set_2 != UNDEFINED_SET_NUMBER) && (ip_mem_2.set_1 != UNDEFINED_SET_NUMBER))
	{
		dia = ip_mem_1.set_2 + ip_mem_2.set_1;
		if(ip_mem_1.deg_truth_2 < ip_mem_2.deg_truth_1)
		{
			dia_val = ip_mem_1.deg_truth_2;
		}
		else
		{
			dia_val = ip_mem_2.deg_truth_1;
		}

		if(dia == u.set_1)
		{
			if(dia_val > u.deg_truth_1)
			{
				u.deg_truth_1 = dia_val;
			}
		}
		else if(dia == u.set_2)
		{
			if(dia_val > u.deg_truth_2)
			{
				u.deg_truth_2 = dia_val;
			}
		}
		else
		{
			u.set_3 = dia;
			u.deg_truth_3 = dia_val;
		}
	}
	if((ip_mem_1.set_2 != UNDEFINED_SET_NUMBER) && (ip_mem_2.set_2 != UNDEFINED_SET_NUMBER))
	{
		dia = ip_mem_1.set_2 + ip_mem_2.set_2;
		if(ip_mem_1.deg_truth_2 < ip_mem_2.deg_truth_2)
		{
			dia_val = ip_mem_1.deg_truth_2;
		}
		else
		{
			dia_val = ip_mem_2.deg_truth_2;
		}

		if(dia == u.set_1)
		{
			if(dia_val > u.deg_truth_1)
			{
				u.deg_truth_1 = dia_val;
			}
		}
		else if(dia == u.set_2)
		{
			if(dia_val > u.deg_truth_2)
			{
				u.deg_truth_2 = dia_val;
			}
		}
		else
		{
			u.set_3 = dia;
			u.deg_truth_3 = dia_val;
		}
	}

	return (u);
}

float fuzzy::defuzzifier (short int n, struct op_membr_val u)
{
	short int dia_n = (2*n);	//number of diagonals in the table, which is also the number of ch op sets, always an odd number
	dia_n = dia_n - 1;
	float b_range = (dia_n+1);
	b_range = 100 / b_range;
	float b;
	
	float den = 0;
	float nom = 0;

	float temp;
	float res = 0;

	if(u.set_1 != UNDEFINED_SET_NUMBER)
	{
		b = (u.set_1 * b_range);
		b = b + b_range;
		/*Serial.print("b1: ");
  		Serial.println(b);*/
		temp = (b * u.deg_truth_1);
		nom = nom + temp;
		den = den + u.deg_truth_1;	
	}
	if(u.set_2 != UNDEFINED_SET_NUMBER)
	{
		b = (u.set_2 * b_range);
		b = b + b_range;
		/*Serial.print("b2: ");
  		Serial.println(b);*/
		temp = (b * u.deg_truth_2);
		nom = nom + temp;
		den = den + u.deg_truth_2;	
	}
	if(u.set_3 != UNDEFINED_SET_NUMBER)
	{
		b = (u.set_3 * b_range);
		b = b + b_range;
		/*Serial.print("b3: ");
  		Serial.println(b);*/
		temp = (b * u.deg_truth_3);
		nom = nom + temp;
		den = den + u.deg_truth_3;	
	}

	
  		/*Serial.print("nom: ");
  		Serial.println(nom);
  		Serial.print("den: ");
  		Serial.println(den);*/
	res = nom / den;
	return (res);
}

int fuzzy::fuzzy_controller(int input, int s_point)
{
	set_point = s_point;
	crisp_ip = input;

	error_p = error_calc(crisp_ip, set_point);
	ch_error_p = ch_error_calc(error, &error_1);

	err_set = membership_determiner(_n_membr_sets, error_p);
	ch_err_set = membership_determiner(_n_membr_sets, ch_error_p);

	ch_op_set = ch_op_determiner(_n_membr_sets, err_set, ch_err_set);

	ch_op_p = defuzzifier(_n_membr_sets, ch_op_set);

	ch_op = depercentizer(ch_op_p, ch_op_max, ch_op_min);	

	return (ch_op);
}

int fuzzy::sample_t_det ()
{
	float r;
	float st;
	r = log(max_st);
	r = r - log(zero_st);
	r = r / error_max;

	st = r * error;
	st = exp(st);
	st = st*zero_st;

	return ((int) st);

}