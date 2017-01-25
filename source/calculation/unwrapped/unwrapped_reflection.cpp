#include "unwrapped_reflection.h"

Unwrapped_Reflection::Unwrapped_Reflection()
{

}

Unwrapped_Reflection::Unwrapped_Reflection(Unwrapped_Structure* unwrapped_Structure, int num_Media, QString active_Whats_This, Measurement& measurement) :
	num_Threads		(reflectivity_Calc_Threads),
	num_Media		(num_Media),
	num_Boundaries	(num_Media - 1),
	num_Layers		(num_Media - 2),
	active_Whats_This(active_Whats_This),
	unwrapped_Structure(unwrapped_Structure),
	measurement(measurement),

	r_Fresnel_s_RE(num_Threads,MyVector<double>(num_Boundaries)),
	r_Fresnel_s_IM(num_Threads,MyVector<double>(num_Boundaries)),
	r_Fresnel_p_RE(num_Threads,MyVector<double>(num_Boundaries)),
	r_Fresnel_p_IM(num_Threads,MyVector<double>(num_Boundaries)),
	r_Local_s_RE  (num_Threads,MyVector<double>(num_Boundaries)),
	r_Local_s_IM  (num_Threads,MyVector<double>(num_Boundaries)),
	r_Local_p_RE  (num_Threads,MyVector<double>(num_Boundaries)),
	r_Local_p_IM  (num_Threads,MyVector<double>(num_Boundaries)),
	hi_RE		  (num_Threads,MyVector<double>(num_Media)),
	hi_IM		  (num_Threads,MyVector<double>(num_Media)),
	exponenta_RE  (num_Threads,MyVector<double>(num_Boundaries)),
	exponenta_IM  (num_Threads,MyVector<double>(num_Boundaries)),

//	r_Fresnel_s	(num_Threads,MyVector<complex<double>>(num_Boundaries)),
//	r_Fresnel_p	(num_Threads,MyVector<complex<double>>(num_Boundaries)),
//	r_Local_s	(num_Threads,MyVector<complex<double>>(num_Boundaries)),
//	r_Local_p	(num_Threads,MyVector<complex<double>>(num_Boundaries)),
//	hi			(num_Threads,MyVector<complex<double>>(num_Media)),
//	exponenta	(num_Threads,MyVector<complex<double>>(num_Layers)),
	weak_Factor	(num_Threads,MyVector<double>(num_Boundaries))
{
	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if angle is changing
	if(active_Whats_This_List[1] == whats_This_Angle)
	{
//		r_s.resize(measurement.cos2.size());
//		r_p.resize(measurement.cos2.size());
//		R_s.resize(measurement.cos2.size());
//		R_p.resize(measurement.cos2.size());
//		R.resize(measurement.cos2.size());

		r_s_RV.resize(measurement.cos2.size());
		r_p_RV.resize(measurement.cos2.size());
		R_s_RV.resize(measurement.cos2.size());
		R_p_RV.resize(measurement.cos2.size());
		R_RV.resize(measurement.cos2.size());
	}
	// TODO spectral
}

void Unwrapped_Reflection::calc_Hi(double k, double cos2, int thread_Index)
{
	double re, im, phase, mod;
	complex<double> com;
	for (int i = 0; i < num_Media; ++i)
	{
		com = unwrapped_Structure->epsilon[i] - cos2;
		re = real(com);
		im = imag(com);

		phase = atan2(im, re)/2;
		mod = k*sqrt(sqrt(re*re + im*im));

		hi[thread_Index][i] = mod*(cos(phase) + I*sin(phase));
	}
}

void Unwrapped_Reflection::calc_Hi_Real_Val(double k, double cos2, int thread_Index)
{
	double re, im, phase, mod;
	for (int i = 0; i < num_Media; ++i)
	{
		re = unwrapped_Structure->epsilon_RE[i] - cos2;
		im = unwrapped_Structure->epsilon_IM[i];

		phase = atan2(im, re)/2;
		mod = k*sqrt(sqrt(re*re + im*im));

		hi_RE[thread_Index][i] = mod*cos(phase);
		hi_IM[thread_Index][i] = mod*sin(phase);
	}
}

void Unwrapped_Reflection::calc_Weak_Factor(int thread_Index)
{
	double a = M_PI/sqrt(M_PI*M_PI - 8.);

	for(auto i=0; i<num_Boundaries; ++i)
	{
		bool is_Norm = false;
		for(int func_Index=0; func_Index<transition_Layer_Functions.size(); ++func_Index)
		{
			is_Norm = is_Norm || unwrapped_Structure->boundary_Interlayer_Composition[i][func_Index].enabled;
		}

		if(is_Norm && (unwrapped_Structure->sigma[i] != 0))
		{
			Interlayer interlayer;

//			double norm = 0;
			weak_Factor[thread_Index][i] = 0;

			//-------------------------------------------------------------------------------
			// erf interlayer
			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][0];
			if(interlayer.enabled)
			{
				double ind_Erf = - 2 * real(hi[thread_Index][i])*real(hi[thread_Index][i+1]);
				double erf = exp(ind_Erf * pow(unwrapped_Structure->sigma[i],2));

				weak_Factor[thread_Index][i] += /*interlayer.interlayer.value **/ erf;
//				norm += interlayer.interlayer.value;
			}/*
			//-------------------------------------------------------------------------------
//			// common value TODO i+1->i ?
//			double s = 2 * sqrt(real(hi[thread_Index][i])*real(hi[thread_Index][i+1]));
//			// lin interlayer
//			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][1];
//			if(interlayer.enabled)
//			{
//				double x_Lin = sqrt(3.) * s * unwrapped_Structure->sigma[i];
//				double lin = sin(x_Lin) / (x_Lin);

//				weak_Factor[thread_Index][i] += interlayer.interlayer.value * lin;
//				norm += interlayer.interlayer.value;
//			}
//			//-------------------------------------------------------------------------------
//			// exp interlayer
//			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][2];
//			if(interlayer.enabled)
//			{
//				double x_Exp = pow(s * unwrapped_Structure->sigma[i], 2) / 2.;
//				double Exp = 1. / (1. + x_Exp);

//				weak_Factor[thread_Index][i] += interlayer.interlayer.value * Exp;
//				norm += interlayer.interlayer.value;
//			}
//			//-------------------------------------------------------------------------------
//			// tanh interlayer TODO
////			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][2];
////			if(interlayer.enabled)
////			{
////				double s_Exp = 2 * sqrt(real(hi[thread_Index][i])*real(hi[thread_Index][i+1]));
////				double x_Exp = pow(s_Exp * unwrapped_Structure->sigma[i], 2) / 2.;
////				double Exp = 1. / (1. + x_Exp);

////				weak_Factor[thread_Index][i] += interlayer.interlayer.value * Exp;
////				norm += interlayer.interlayer.value;
////			}
//			//-------------------------------------------------------------------------------
//			// sin interlayer
//			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][4];
//			if(interlayer.enabled)
//			{
//				double x_Sin = a * s * unwrapped_Structure->sigma[i] - M_PI_2;
//				double y_Sin = x_Sin + M_PI;
//				double Sin = M_PI_4*(sin(x_Sin)/x_Sin + sin(y_Sin)/y_Sin);

//				weak_Factor[thread_Index][i] += interlayer.interlayer.value * Sin;
//				norm += interlayer.interlayer.value;
//			}
//			//-------------------------------------------------------------------------------
//			// normalization
//			if(is_Norm)
//			{
//				weak_Factor[thread_Index][i] /= norm;
//			}
			*/
		} else
		{
			weak_Factor[thread_Index][i] = 1.;
		}
//		qInfo() << "wf = " << weak_Factor[thread_Index][i];
	}
}

void Unwrapped_Reflection::calc_Weak_Factor_Real_Val(int thread_Index)
{
	double a = M_PI/sqrt(M_PI*M_PI - 8.);

	for(auto i=0; i<num_Boundaries; ++i)
	{
		bool is_Norm = false;
		for(int func_Index=0; func_Index<transition_Layer_Functions.size(); ++func_Index)
		{
			is_Norm = is_Norm || unwrapped_Structure->boundary_Interlayer_Composition[i][func_Index].enabled;
		}

		if(is_Norm && (unwrapped_Structure->sigma[i] != 0))
		{
			Interlayer interlayer;

//			double norm = 0;
			weak_Factor[thread_Index][i] = 0;

			//-------------------------------------------------------------------------------
			// erf interlayer
			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][0];
			if(interlayer.enabled)
			{
//				double ind_Erf = - 2 * real(hi[thread_Index][i])*real(hi[thread_Index][i+1]);
				double ind_Erf = - 2 * hi_RE[thread_Index][i]*hi_RE[thread_Index][i+1];
				double erf = exp(ind_Erf * pow(unwrapped_Structure->sigma[i],2));

				weak_Factor[thread_Index][i] += /*interlayer.interlayer.value **/ erf;
//				norm += interlayer.interlayer.value;
			}/*
			//-------------------------------------------------------------------------------
//			// common value TODO i+1->i ?
//			double s = 2 * sqrt(real(hi[thread_Index][i])*real(hi[thread_Index][i+1]));
//			// lin interlayer
//			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][1];
//			if(interlayer.enabled)
//			{
//				double x_Lin = sqrt(3.) * s * unwrapped_Structure->sigma[i];
//				double lin = sin(x_Lin) / (x_Lin);

//				weak_Factor[thread_Index][i] += interlayer.interlayer.value * lin;
//				norm += interlayer.interlayer.value;
//			}
//			//-------------------------------------------------------------------------------
//			// exp interlayer
//			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][2];
//			if(interlayer.enabled)
//			{
//				double x_Exp = pow(s * unwrapped_Structure->sigma[i], 2) / 2.;
//				double Exp = 1. / (1. + x_Exp);

//				weak_Factor[thread_Index][i] += interlayer.interlayer.value * Exp;
//				norm += interlayer.interlayer.value;
//			}
//			//-------------------------------------------------------------------------------
//			// tanh interlayer TODO
////			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][2];
////			if(interlayer.enabled)
////			{
////				double s_Exp = 2 * sqrt(real(hi[thread_Index][i])*real(hi[thread_Index][i+1]));
////				double x_Exp = pow(s_Exp * unwrapped_Structure->sigma[i], 2) / 2.;
////				double Exp = 1. / (1. + x_Exp);

////				weak_Factor[thread_Index][i] += interlayer.interlayer.value * Exp;
////				norm += interlayer.interlayer.value;
////			}
//			//-------------------------------------------------------------------------------
//			// sin interlayer
//			interlayer = unwrapped_Structure->boundary_Interlayer_Composition[i][4];
//			if(interlayer.enabled)
//			{
//				double x_Sin = a * s * unwrapped_Structure->sigma[i] - M_PI_2;
//				double y_Sin = x_Sin + M_PI;
//				double Sin = M_PI_4*(sin(x_Sin)/x_Sin + sin(y_Sin)/y_Sin);

//				weak_Factor[thread_Index][i] += interlayer.interlayer.value * Sin;
//				norm += interlayer.interlayer.value;
//			}
//			//-------------------------------------------------------------------------------
//			// normalization
//			if(is_Norm)
//			{
//				weak_Factor[thread_Index][i] /= norm;
//			}
			*/
		} else
		{
			weak_Factor[thread_Index][i] = 1.;
		}
//		qInfo() << "wf = " << weak_Factor[thread_Index][i];
	}
}

void Unwrapped_Reflection::calc_Exponenta(int thread_Index)
{
	double re, im;
	complex<double> com;
	for (int i = 0; i < num_Layers; ++i)
	{
		com = 2.*(I*hi[thread_Index][i+1])*unwrapped_Structure->thickness[i]; // best
		re = real(com);
		im = imag(com);

		exponenta[thread_Index][i] = exp(re)*(cos(im) + I*sin(im));
	}
}

void Unwrapped_Reflection::calc_Exponenta_Real_Val(int thread_Index)
{
	double re, im, recom, imcom, ere;
	for (int i = 0; i < num_Layers; ++i)
	{
		recom = -2.*hi_IM[thread_Index][i+1];
		imcom =  2.*hi_RE[thread_Index][i+1];
		re = recom*unwrapped_Structure->thickness[i];
		im = imcom*unwrapped_Structure->thickness[i];
		ere = exp(re);

		exponenta_RE[thread_Index][i] = ere*cos(im);
		exponenta_IM[thread_Index][i] = ere*sin(im);
	}
}

void Unwrapped_Reflection::calc_Fresnel(double polarization, int thread_Index)
{
	complex<double> temp_Fre_Numer, temp_Fre_Denom, temp_1, temp_2;
	// s-polarization
	if (polarization >-1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_Fre_Denom = hi[thread_Index][i] + hi[thread_Index][i+1];
		if (temp_Fre_Denom != 0.)
		{
			temp_Fre_Numer = hi[thread_Index][i] - hi[thread_Index][i+1];
			r_Fresnel_s[thread_Index][i] = weak_Factor[thread_Index][i] * temp_Fre_Numer / temp_Fre_Denom;
		} else
		{
			r_Fresnel_s[thread_Index][i] = 0;
		}
	}
	// p-polarization
	if (polarization < 1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_1 = hi[thread_Index][i]/unwrapped_Structure->epsilon[i];
		temp_2 = hi[thread_Index][i+1]/unwrapped_Structure->epsilon[i+1];
		temp_Fre_Denom = temp_1 + temp_2;
		if (temp_Fre_Denom != 0.)
		{
			temp_Fre_Numer = temp_1 - temp_2;
			r_Fresnel_p[thread_Index][i] = weak_Factor[thread_Index][i] * temp_Fre_Numer / temp_Fre_Denom;
		} else
		{
			r_Fresnel_p[thread_Index][i] = 0;
		}
	}
}

void Unwrapped_Reflection::calc_Fresnel_Real_Val(double polarization, int thread_Index)
{
	double temp_Fre_Numer_RE, temp_Fre_Numer_IM, temp_Fre_Denom_SQARE, temp_Fre_Denom_RE, temp_Fre_Denom_IM, temp_1_RE, temp_1_IM, temp_2_RE, temp_2_IM;
	// s-polarization
	if (polarization >-1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_Fre_Denom_RE = hi_RE[thread_Index][i] + hi_RE[thread_Index][i+1];
		temp_Fre_Denom_IM = hi_IM[thread_Index][i] + hi_IM[thread_Index][i+1];
		temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

		if (temp_Fre_Denom_SQARE != 0.)
		{
			temp_Fre_Numer_RE = hi_RE[thread_Index][i] - hi_RE[thread_Index][i+1];
			temp_Fre_Numer_IM = hi_IM[thread_Index][i] - hi_IM[thread_Index][i+1];

			r_Fresnel_s_RE[thread_Index][i] = weak_Factor[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM);
			r_Fresnel_s_IM[thread_Index][i] = weak_Factor[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_IM*temp_Fre_Denom_RE - temp_Fre_Numer_RE*temp_Fre_Denom_IM);
		} else
		{
			r_Fresnel_s_RE[thread_Index][i] = 0;
			r_Fresnel_s_IM[thread_Index][i] = 0;
		}
	}
	// p-polarization
	if (polarization < 1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_1_RE = (hi_RE[thread_Index][i]*unwrapped_Structure->epsilon_RE[i] + hi_IM[thread_Index][i]*unwrapped_Structure->epsilon_IM[i]) / unwrapped_Structure->epsilon_Norm[i];
		temp_1_IM = (hi_IM[thread_Index][i]*unwrapped_Structure->epsilon_RE[i] - hi_RE[thread_Index][i]*unwrapped_Structure->epsilon_IM[i]) / unwrapped_Structure->epsilon_Norm[i];

		temp_2_RE = (hi_RE[thread_Index][i+1]*unwrapped_Structure->epsilon_RE[i+1] + hi_IM[thread_Index][i+1]*unwrapped_Structure->epsilon_IM[i+1]) / unwrapped_Structure->epsilon_Norm[i+1];
		temp_2_IM = (hi_IM[thread_Index][i+1]*unwrapped_Structure->epsilon_RE[i+1] - hi_RE[thread_Index][i+1]*unwrapped_Structure->epsilon_IM[i+1]) / unwrapped_Structure->epsilon_Norm[i+1];

		temp_Fre_Denom_RE = temp_1_RE + temp_2_RE;
		temp_Fre_Denom_IM = temp_1_IM + temp_2_IM;
		temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

		if (temp_Fre_Denom_SQARE != 0.)
		{
			temp_Fre_Numer_RE = temp_1_RE - temp_2_RE;
			temp_Fre_Numer_IM = temp_1_IM - temp_2_IM;

			r_Fresnel_p_RE[thread_Index][i] = weak_Factor[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM);
			r_Fresnel_p_IM[thread_Index][i] = weak_Factor[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_IM*temp_Fre_Denom_RE - temp_Fre_Numer_RE*temp_Fre_Denom_IM);

		} else
		{
			r_Fresnel_p_RE[thread_Index][i] = 0;
			r_Fresnel_p_IM[thread_Index][i] = 0;
		}
	}
}

void Unwrapped_Reflection::calc_Local(double polarization, int thread_Index)
{
	complex<double> loc_Denom;
	// s-polarization
	if (polarization >-1)
	{
		r_Local_s[thread_Index].back() = r_Fresnel_s[thread_Index].back();	// last boundary
		for (int i = num_Layers-1; i >= 0; --i)
		{
			loc_Denom = 1. + r_Fresnel_s[thread_Index][i]*r_Local_s[thread_Index][i+1]*exponenta[thread_Index][i];
			r_Local_s[thread_Index][i] = (r_Fresnel_s[thread_Index][i] + r_Local_s[thread_Index][i+1]*exponenta[thread_Index][i]) / loc_Denom;
		}
	}
	// p-polarization
	if (polarization <1)
	{
		r_Local_p[thread_Index].back() = r_Fresnel_p[thread_Index].back();	// last boundary
		for (int i = num_Layers-1; i >= 0; --i)
		{
			loc_Denom = 1. + r_Fresnel_p[thread_Index][i]*r_Local_p[thread_Index][i+1]*exponenta[thread_Index][i];
			r_Local_p[thread_Index][i] = (r_Fresnel_p[thread_Index][i] + r_Local_p[thread_Index][i+1]*exponenta[thread_Index][i]) / loc_Denom;
		}
	}
}

void Unwrapped_Reflection::calc_Local_Real_Val(double polarization, int thread_Index)
{
	double loc_Denom_RE, loc_Denom_IM, loc_Denom_SQUARE, temp_RE, temp_IM, loc_Numer_RE, loc_Numer_IM;
	// s-polarization
	if (polarization >-1)
	{
		r_Local_s_RE[thread_Index].back() = r_Fresnel_s_RE[thread_Index].back();	// last boundary
		r_Local_s_IM[thread_Index].back() = r_Fresnel_s_IM[thread_Index].back();	// last boundary

		for (int i = num_Layers-1; i >= 0; --i)
		{
			temp_RE = r_Local_s_RE[thread_Index][i+1]*exponenta_RE[thread_Index][i] - r_Local_s_IM[thread_Index][i+1]*exponenta_IM[thread_Index][i];
			temp_IM = r_Local_s_IM[thread_Index][i+1]*exponenta_RE[thread_Index][i] + r_Local_s_RE[thread_Index][i+1]*exponenta_IM[thread_Index][i];
			loc_Denom_RE = 1. + (temp_RE*r_Fresnel_s_RE[thread_Index][i] - temp_IM*r_Fresnel_s_IM[thread_Index][i]);
			loc_Denom_IM =       temp_IM*r_Fresnel_s_RE[thread_Index][i] + temp_RE*r_Fresnel_s_IM[thread_Index][i];
			loc_Denom_SQUARE = loc_Denom_RE*loc_Denom_RE + loc_Denom_IM*loc_Denom_IM;

			loc_Numer_RE = r_Fresnel_s_RE[thread_Index][i] + temp_RE;
			loc_Numer_IM = r_Fresnel_s_IM[thread_Index][i] + temp_IM;

			r_Local_s_RE[thread_Index][i] = (loc_Numer_RE*loc_Denom_RE + loc_Numer_IM*loc_Denom_IM) / loc_Denom_SQUARE;
			r_Local_s_IM[thread_Index][i] = (loc_Numer_IM*loc_Denom_RE - loc_Numer_RE*loc_Denom_IM) / loc_Denom_SQUARE;
		}
	}
	// p-polarization
	if (polarization <1)
	{
		r_Local_p_RE[thread_Index].back() = r_Fresnel_p_RE[thread_Index].back();	// last boundary
		r_Local_p_IM[thread_Index].back() = r_Fresnel_p_IM[thread_Index].back();	// last boundary

		for (int i = num_Layers-1; i >= 0; --i)
		{
			temp_RE = r_Local_p_RE[thread_Index][i+1]*exponenta_RE[thread_Index][i] - r_Local_p_IM[thread_Index][i+1]*exponenta_IM[thread_Index][i];
			temp_IM = r_Local_p_IM[thread_Index][i+1]*exponenta_RE[thread_Index][i] + r_Local_p_RE[thread_Index][i+1]*exponenta_IM[thread_Index][i];
			loc_Denom_RE = 1. + (temp_RE*r_Fresnel_p_RE[thread_Index][i] - temp_IM*r_Fresnel_p_IM[thread_Index][i]);
			loc_Denom_IM =       temp_IM*r_Fresnel_p_RE[thread_Index][i] + temp_RE*r_Fresnel_p_IM[thread_Index][i];
			loc_Denom_SQUARE = loc_Denom_RE*loc_Denom_RE + loc_Denom_IM*loc_Denom_IM;

			loc_Numer_RE = r_Fresnel_p_RE[thread_Index][i] + temp_RE;
			loc_Numer_IM = r_Fresnel_p_IM[thread_Index][i] + temp_IM;

			r_Local_p_RE[thread_Index][i] = (loc_Numer_RE*loc_Denom_RE + loc_Numer_IM*loc_Denom_IM) / loc_Denom_SQUARE;
			r_Local_p_IM[thread_Index][i] = (loc_Numer_IM*loc_Denom_RE - loc_Numer_RE*loc_Denom_IM) / loc_Denom_SQUARE;
		}
	}
}

void Unwrapped_Reflection::calc_Reflectivity_1_Point_1_Thread(double k, double cos2, double polarization, int thread_Index)
{
//	auto start = std::chrono::system_clock::now();
//		calc_Hi			(k, cos2, thread_Index);
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Hi   : "<< elapsed.count()/1000. << " seconds";
//	start = std::chrono::system_clock::now();
//		calc_Hi_Real_Val(k, cos2, thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Hi RV: "<< elapsed.count()/1000. << " seconds";
////------------------------------------------------------------------------
//	start = std::chrono::system_clock::now();
//		calc_Weak_Factor(thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Weak Factor   : "<< elapsed.count()/1000. << " seconds";
//	start = std::chrono::system_clock::now();
//		calc_Weak_Factor_Real_Val(thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Weak Factor RV: "<< elapsed.count()/1000. << " seconds";
////------------------------------------------------------------------------
//	start = std::chrono::system_clock::now();
//		calc_Exponenta  (thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Exponenta   : "<< elapsed.count()/1000. << " seconds";
//	start = std::chrono::system_clock::now();
//		calc_Exponenta_Real_Val(thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Exponenta RV: "<< elapsed.count()/1000. << " seconds";
////------------------------------------------------------------------------
//	start = std::chrono::system_clock::now();
//		calc_Fresnel    (polarization, thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Fresnel   : "<< elapsed.count()/1000. << " seconds";
//	start = std::chrono::system_clock::now();
//		calc_Fresnel_Real_Val(polarization, thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Fresnel RV: "<< elapsed.count()/1000. << " seconds";
//	//------------------------------------------------------------------------
//	start = std::chrono::system_clock::now();
//		calc_Local      (polarization, thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Local   : "<< elapsed.count()/1000. << " seconds";
//	start = std::chrono::system_clock::now();
//		calc_Local_Real_Val(polarization, thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Local RV: "<< elapsed.count()/1000. << " seconds";


//	start = std::chrono::system_clock::now();
//		calc_Hi			(k, cos2, thread_Index);
//		calc_Weak_Factor(thread_Index);
//		calc_Exponenta  (thread_Index);
//		calc_Fresnel    (polarization, thread_Index);
//		calc_Local      (polarization, thread_Index);
//	end = std::chrono::system_clock::now();
//	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Bare Reflectivity   : "<< elapsed.count()/1000. << " seconds";

	calc_Hi_Real_Val(k, cos2, thread_Index);
	calc_Weak_Factor_Real_Val(thread_Index);
	calc_Exponenta_Real_Val(thread_Index);
	calc_Fresnel_Real_Val(polarization, thread_Index);
	calc_Local_Real_Val(polarization, thread_Index);

}

void Unwrapped_Reflection::calc_Reflectivity_Angular_nMin_nMax_1_Thread(double k, QVector<double>& cos2_Vec, int n_Min, int n_Max, double polarization, int thread_Index)
{
	// polarization
	double s_Weight = (1. + polarization) / 2.;
	double p_Weight = (1. - polarization) / 2.;

	for(int point=n_Min; point<n_Max; ++point)
	{
		if(cos2_Vec[point]!=1)
		{
			calc_Reflectivity_1_Point_1_Thread(k, cos2_Vec[point], polarization, thread_Index);

//			r_s[point] = r_Local_s[thread_Index][0];
//			r_p[point] = r_Local_p[thread_Index][0];
//			R_s[point] = pow(abs(r_s[point]),2);
//			R_p[point] = pow(abs(r_p[point]),2);
//			R  [point] = s_Weight * R_s[point] + p_Weight * R_p[point];

			r_s_RV[point] = complex<double>(r_Local_s_RE[thread_Index][0], r_Local_s_IM[thread_Index][0]);
			r_p_RV[point] = complex<double>(r_Local_p_RE[thread_Index][0], r_Local_p_IM[thread_Index][0]);
			R_s_RV[point] = pow(abs(r_s_RV[point]),2);
			R_p_RV[point] = pow(abs(r_p_RV[point]),2);
			R_RV  [point] = s_Weight * R_s_RV[point] + p_Weight * R_p_RV[point];
		}
		else
		{
//			r_s[point] = 1;
//			r_p[point] = 1;
//			R_s[point] = 1;
//			R_p[point] = 1;
//			R  [point] = 1;

			r_s_RV[point] = 1;
			r_p_RV[point] = 1;
			R_s_RV[point] = 1;
			R_p_RV[point] = 1;
			R_RV  [point] = 1;
		}
	}
}	// TODO spectral

void Unwrapped_Reflection::calc_Reflectivity()
{
	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if angle is changing
	if(active_Whats_This_List[1] == whats_This_Angle)
	{
		// ----------------------------------------------------------------------------------------------------------------------
		// parallelization
		vector<thread> workers(num_Threads);
		auto delta_N = measurement.cos2.size() / num_Threads;	// number of points per thread

		for (auto thread_Index = 0; thread_Index < num_Threads - 1; ++thread_Index)
		{
			auto n_Min = thread_Index*delta_N;
			auto n_Max = (thread_Index + 1)*delta_N;
			workers[thread_Index] = thread(&Unwrapped_Reflection::calc_Reflectivity_Angular_nMin_nMax_1_Thread, this, measurement.k_Value, measurement.cos2, n_Min, n_Max, measurement.polarization.value, thread_Index);
		}
		auto thread_Index = num_Threads - 1;
		auto n_Min = thread_Index*delta_N;
		auto n_Max = measurement.cos2.size();
		workers[thread_Index] = thread(&Unwrapped_Reflection::calc_Reflectivity_Angular_nMin_nMax_1_Thread, this, measurement.k_Value, measurement.cos2, n_Min, n_Max, measurement.polarization.value, thread_Index);

		for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
		{
			if (workers[thread_Index].joinable()) workers[thread_Index].join();	// присоединение потоков
		}
		// ----------------------------------------------------------------------------------------------------------------------
	}
	// TODO spectral
}
