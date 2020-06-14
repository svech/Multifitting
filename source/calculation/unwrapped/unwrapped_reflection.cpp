#include "unwrapped_reflection.h"
#include "multilayer_approach/multilayer/multilayer.h"

struct Params
{
	Unwrapped_Reflection* unwrapped_Reflection;
	int thread_Index;
	double cos_Theta;
	double cos_Theta_0;
	double incoherent_Sum_s;
	double incoherent_Sum_p;
	bool phi_As_Angle;
};
struct Params_DWBA_SA_CSA
{
	Unwrapped_Reflection* unwrapped_Reflection;
	int thread_Index;
	Data* struct_Data;
	double p;
};

double function_Scattering_ABC_2D_s (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(phi*M_PI/180);
	else
		cos_Phi = phi;

	double common_PSD_2D_Factor = Global_Variables::PSD_ABC_2D(u->substrate.PSD_ABC_2D_Factor,
	                                                           u->substrate.roughness_Model.cor_radius.value,
	                                                           u->substrate.roughness_Model.fractal_alpha.value,
	                                                           u->measurement.k_Value,
	                                                           params->cos_Theta,
	                                                           params->cos_Theta_0,
	                                                           cos_Phi);
	double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->unwrapped_Structure->alpha[thread_Index].back(), // substrate
	                                                  u->measurement.k_Value,
	                                                  params->cos_Theta,
	                                                  params->cos_Theta_0,
	                                                  cos_Phi);
	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries; i++)
	{
		u->cross_Exp_Factor_2D[thread_Index][i][i-1] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_mu_alpha_h[thread_Index][i-1]);
		partially_Coherent_Sum += u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_s[thread_Index][i][i-1];

		for (int j = i-2; j>=0; j--)
		{
			u->cross_Exp_Factor_2D[thread_Index][i][j] = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->cross_Exp_Factor_2D[thread_Index][i-1][j];
			partially_Coherent_Sum += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_s[thread_Index][i][j];
		}
	}
	return (partially_Coherent_Sum + params->incoherent_Sum_s) * common_PSD_2D_Factor;
}
double function_Scattering_ABC_2D_p (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(phi*M_PI/180);
	else
		cos_Phi = phi;

	double common_PSD_2D_Factor = Global_Variables::PSD_ABC_2D(u->substrate.PSD_ABC_2D_Factor,
	                                                           u->substrate.roughness_Model.cor_radius.value,
	                                                           u->substrate.roughness_Model.fractal_alpha.value,
	                                                           u->measurement.k_Value,
	                                                           params->cos_Theta,
	                                                           params->cos_Theta_0,
	                                                           cos_Phi);
	double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->unwrapped_Structure->alpha[thread_Index].back(), // substrate
	                                                  u->measurement.k_Value,
	                                                  params->cos_Theta,
	                                                  params->cos_Theta_0,
	                                                  cos_Phi);
	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries; i++)
	{
		u->cross_Exp_Factor_2D[thread_Index][i][i-1] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_mu_alpha_h[thread_Index][i-1]);
		partially_Coherent_Sum += u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_p[thread_Index][i][i-1];

		for (int j = i-2; j>=0; j--)
		{
			u->cross_Exp_Factor_2D[thread_Index][i][j] = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->cross_Exp_Factor_2D[thread_Index][i-1][j];
			partially_Coherent_Sum += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_p[thread_Index][i][j];
		}
	}
	return (partially_Coherent_Sum + params->incoherent_Sum_p) * common_PSD_2D_Factor;
}
double function_Scattering_ABC_2D_sp(double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(phi*M_PI/180);
	else
		cos_Phi = phi;

	double common_PSD_2D_Factor = Global_Variables::PSD_ABC_2D(u->substrate.PSD_ABC_2D_Factor,
	                                                           u->substrate.roughness_Model.cor_radius.value,
	                                                           u->substrate.roughness_Model.fractal_alpha.value,
	                                                           u->measurement.k_Value,
	                                                           params->cos_Theta,
	                                                           params->cos_Theta_0,
	                                                           cos_Phi);
	double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->unwrapped_Structure->alpha[thread_Index].back(), // substrate
	                                                  u->measurement.k_Value,
	                                                  params->cos_Theta,
	                                                  params->cos_Theta_0,
	                                                  cos_Phi);
	double partially_Coherent_Sum_s = 0;
	double partially_Coherent_Sum_p = 0;
	for (int i = 1; i<u->num_Boundaries; i++)
	{
		u->cross_Exp_Factor_2D[thread_Index][i][i-1] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_mu_alpha_h[thread_Index][i-1]);
		partially_Coherent_Sum_s += u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_s[thread_Index][i][i-1];
		partially_Coherent_Sum_p += u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_p[thread_Index][i][i-1];

		for (int j = i-2; j>=0; j--)
		{
			u->cross_Exp_Factor_2D[thread_Index][i][j] = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->cross_Exp_Factor_2D[thread_Index][i-1][j];
			partially_Coherent_Sum_s += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_s[thread_Index][i][j];
			partially_Coherent_Sum_p += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_p[thread_Index][i][j];
		}
	}
	return (u->s_Weight*(params->incoherent_Sum_s + partially_Coherent_Sum_s) +
	        u->p_Weight*(params->incoherent_Sum_p + partially_Coherent_Sum_p) ) * common_PSD_2D_Factor;
}

double function_Scattering_Linear_2D_s (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(phi*M_PI/180);
	else
		cos_Phi = phi;

	// incoherent part and PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries-1] = Global_Variables::PSD_ABC_2D(u->substrate.PSD_ABC_2D_Factor,
	                                                                                         u->substrate.roughness_Model.cor_radius.value,
	                                                                                         u->substrate.roughness_Model.fractal_alpha.value,
	                                                                                         u->measurement.k_Value,
	                                                                                         params->cos_Theta,
	                                                                                         params->cos_Theta_0,
	                                                                                         cos_Phi);

	double incoherent_Sum = u->intensity_Term_Boundary_s[thread_Index][u->num_Boundaries-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries-1];
	for (int i = u->num_Boundaries-2; i>=0; i--)
	{
		double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->unwrapped_Structure->alpha[thread_Index][i],
		                                                  u->measurement.k_Value,
		                                                  params->cos_Theta,
		                                                  params->cos_Theta_0,
		                                                  cos_Phi);
		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_mu_alpha_h[thread_Index][i]);


		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
		                                               + Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
		                                                                                        nu_Alpha_2*u->unwrapped_Structure->PSD_mu_alpha[thread_Index][i],
		                                                                                        u->unwrapped_Structure->omega[thread_Index][i],
		                                                                                        nu_Alpha_2*u->unwrapped_Structure->omega_pow23[thread_Index][i],
		                                                                                        u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum += u->intensity_Term_Boundary_s[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries; i++)
	{
		double i_Term = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_s[thread_Index][i][i-1];

		for (int j = i-2; j>=0; j--)
		{
			u->cross_Exp_Factor_2D[thread_Index][i][j] = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->cross_Exp_Factor_2D[thread_Index][i-1][j];
			i_Term += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_s[thread_Index][i][j];
		}
		partially_Coherent_Sum += i_Term * u->PSD_Factor_Boundary[thread_Index][i];
	}
	return (partially_Coherent_Sum + incoherent_Sum);
}
double function_Scattering_Linear_2D_p (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(phi*M_PI/180);
	else
		cos_Phi = phi;

	// incoherent part and PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries-1] = Global_Variables::PSD_ABC_2D(u->substrate.PSD_ABC_2D_Factor,
	                                                                                         u->substrate.roughness_Model.cor_radius.value,
	                                                                                         u->substrate.roughness_Model.fractal_alpha.value,
	                                                                                         u->measurement.k_Value,
	                                                                                         params->cos_Theta,
	                                                                                         params->cos_Theta_0,
	                                                                                         cos_Phi);

	double incoherent_Sum = u->intensity_Term_Boundary_p[thread_Index][u->num_Boundaries-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries-1];
	for (int i = u->num_Boundaries-2; i>=0; i--)
	{
		double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->unwrapped_Structure->alpha[thread_Index][i],
		                                                  u->measurement.k_Value,
		                                                  params->cos_Theta,
		                                                  params->cos_Theta_0,
		                                                  cos_Phi);
		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_mu_alpha_h[thread_Index][i]);


		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
		                                               + Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
		                                                                                        nu_Alpha_2*u->unwrapped_Structure->PSD_mu_alpha[thread_Index][i],
		                                                                                        u->unwrapped_Structure->omega[thread_Index][i],
		                                                                                        nu_Alpha_2*u->unwrapped_Structure->omega_pow23[thread_Index][i],
		                                                                                        u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum += u->intensity_Term_Boundary_p[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries; i++)
	{
		double i_Term = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_p[thread_Index][i][i-1];

		for (int j = i-2; j>=0; j--)
		{
			u->cross_Exp_Factor_2D[thread_Index][i][j] = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->cross_Exp_Factor_2D[thread_Index][i-1][j];
			i_Term += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_p[thread_Index][i][j];
		}
		partially_Coherent_Sum += i_Term * u->PSD_Factor_Boundary[thread_Index][i];
	}
	return (partially_Coherent_Sum + incoherent_Sum);
}
double function_Scattering_Linear_2D_sp(double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(phi*M_PI/180);
	else
		cos_Phi = phi;

	// incoherent part and PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries-1] = Global_Variables::PSD_ABC_2D(u->substrate.PSD_ABC_2D_Factor,
	                                                                                         u->substrate.roughness_Model.cor_radius.value,
	                                                                                         u->substrate.roughness_Model.fractal_alpha.value,
	                                                                                         u->measurement.k_Value,
	                                                                                         params->cos_Theta,
	                                                                                         params->cos_Theta_0,
	                                                                                         cos_Phi);

	double incoherent_Sum_s = u->intensity_Term_Boundary_s[thread_Index][u->num_Boundaries-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries-1];
	double incoherent_Sum_p = u->intensity_Term_Boundary_p[thread_Index][u->num_Boundaries-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries-1];
	for (int i = u->num_Boundaries-2; i>=0; i--)
	{
		double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->unwrapped_Structure->alpha[thread_Index][i],
		                                                  u->measurement.k_Value,
		                                                  params->cos_Theta,
		                                                  params->cos_Theta_0,
		                                                  cos_Phi);
		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_mu_alpha_h[thread_Index][i]);


		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
		                                               + Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
		                                                                                        nu_Alpha_2*u->unwrapped_Structure->PSD_mu_alpha[thread_Index][i],
		                                                                                        u->unwrapped_Structure->omega[thread_Index][i],
		                                                                                        nu_Alpha_2*u->unwrapped_Structure->omega_pow23[thread_Index][i],
		                                                                                        u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum_s += u->intensity_Term_Boundary_s[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
		incoherent_Sum_p += u->intensity_Term_Boundary_p[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum_s = 0;
	double partially_Coherent_Sum_p = 0;
	for (int i = 1; i<u->num_Boundaries; i++)
	{
		double i_Term_s = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_s[thread_Index][i][i-1];
		double i_Term_p = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_p[thread_Index][i][i-1];

		for (int j = i-2; j>=0; j--)
		{
			u->cross_Exp_Factor_2D[thread_Index][i][j] = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->cross_Exp_Factor_2D[thread_Index][i-1][j];
			i_Term_s += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_s[thread_Index][i][j];
			i_Term_p += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_p[thread_Index][i][j];
		}
		partially_Coherent_Sum_s += i_Term_s * u->PSD_Factor_Boundary[thread_Index][i];
		partially_Coherent_Sum_p += i_Term_p * u->PSD_Factor_Boundary[thread_Index][i];
	}
	return  u->s_Weight*(partially_Coherent_Sum_s + incoherent_Sum_s) +
	        u->p_Weight*(partially_Coherent_Sum_p + incoherent_Sum_p);
}

double function_DWBA_SA_CSA_Batch_Integrand(double r, void* par)
{
	Params_DWBA_SA_CSA* params = reinterpret_cast<Params_DWBA_SA_CSA*>(par);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;
	Data& struct_Data = *(params->struct_Data);
//	double p = params->p;

	double alpha = struct_Data.roughness_Model.fractal_alpha.value;
	double sigma = struct_Data.roughness_Model.sigma.value;
	double xi = struct_Data.roughness_Model.cor_radius.value;

	double integrand = 0;
	double cor = -2020;
	if(u->multilayer->imperfections_Model.common_Model == ABC_model)
		cor = Global_Variables::Cor_ABC_Pow_n		   (xi, alpha, r, pow(pow(2,1-alpha)/tgamma(alpha),1), 1);
	if(u->multilayer->imperfections_Model.common_Model == fractal_Gauss_Model)
		cor = Global_Variables::Cor_Fractal_Gauss_Pow_n(xi, alpha, r, 1);

	double cor_n_Power = 1;
	for(int n_Power=1; n_Power<=n_Max_Series; n_Power++)
	{
		cor_n_Power *= cor;
		integrand += u->pre_Fourier_Factor[thread_Index][n_Power-1]*cor_n_Power;
	}

	return sigma*sigma*integrand;
}

Unwrapped_Reflection::Unwrapped_Reflection(Multilayer* multilayer, Unwrapped_Structure* unwrapped_Structure, int num_Media,
                                           const Data& measurement, bool depth_Grading, bool sigma_Grading,
										   const Calc_Functions& calc_Functions, Calculated_Values& calculated_Values, QString calc_Mode, QString spec_Scat_mode):
    num_Threads		(reflectivity_Calc_Threads),
    num_Layers		(num_Media-2),
    num_Boundaries	(num_Media-1),
    num_Media		(num_Media),
    max_Depth		(unwrapped_Structure->max_Depth),
    depth_Threshold	(unwrapped_Structure->depth_Threshold),
    depth_Grading	(depth_Grading),
    sigma_Grading	(sigma_Grading),
    calc_Functions  (calc_Functions),
    calculated_Values(calculated_Values),
    calc_Mode		(calc_Mode),
    spec_Scat_mode	(spec_Scat_mode),
    unwrapped_Structure(unwrapped_Structure),
    multilayer (multilayer),
    measurement(measurement),
    phi_Points (measurement.detector_Phi_Angle_Vec.size()),
    short_Phi_Points(measurement.end_Phi_Number - measurement.start_Phi_Index),

    r_Fresnel_s(num_Threads,vector<complex<double>>(num_Boundaries)),
    r_Fresnel_p(num_Threads,vector<complex<double>>(num_Boundaries)),
    r_Local_s  (num_Threads,vector<complex<double>>(num_Boundaries)),
    r_Local_p  (num_Threads,vector<complex<double>>(num_Boundaries)),

    t_Fresnel_s(num_Threads,vector<complex<double>>(num_Boundaries)),
    t_Fresnel_p(num_Threads,vector<complex<double>>(num_Boundaries)),
    t_Local_s  (num_Threads,vector<complex<double>>(num_Boundaries)),
    t_Local_p  (num_Threads,vector<complex<double>>(num_Boundaries)),

//	hi		   (num_Threads,vector<complex<double>>(num_Media)),
    hi	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_Hi : calculated_Values.q_Hi ),

    exponenta  (num_Threads,vector<complex<double>>(num_Boundaries)),
    exponenta_2(num_Threads,vector<complex<double>>(num_Boundaries)),

//	U_i_s	(num_Threads, vector<complex<double>>(num_Media)),
//	U_r_s	(num_Threads, vector<complex<double>>(num_Media)),
//	U_i_p	(num_Threads, vector<complex<double>>(num_Media)),
//	U_r_p	(num_Threads, vector<complex<double>>(num_Media)),
    U_i_s	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_i_s : calculated_Values.q_U_i_s ),
    U_r_s	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_r_s : calculated_Values.q_U_r_s ),
    U_i_p	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_i_p : calculated_Values.q_U_i_p ),
    U_r_p	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_r_p : calculated_Values.q_U_r_p ),

    environment_Factor_s(num_Threads),
    environment_Factor_p(num_Threads),

    epsilon_Ambient(num_Threads),
    epsilon_Substrate(num_Threads),

    weak_Factor_R(num_Threads,vector<complex<double>>(num_Boundaries)),
    weak_Factor_T(num_Threads,vector<complex<double>>(num_Boundaries)),

    /// roughness

    PSD_1D_Func_Vec(num_Threads),
    PSD_2D_Func_Vec(num_Threads),

    boundary_Item_Vec(num_Boundaries),
    PSD_Factor_Item	 (num_Threads),

    spline_Vec(num_Threads),
    acc_Vec(num_Threads),
    GISAS_Slice(num_Threads),
    phi_Slice(num_Threads)
{
	s_Weight = (1. + measurement.polarization) / 2.;
	p_Weight = (1. - measurement.polarization) / 2.;

	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type  == argument_Types[Beam_Grazing_Angle] )	num_Points = measurement.beam_Theta_0_Angle_Vec.size();
		if( measurement.argument_Type  == argument_Types[Wavelength_Energy] )	num_Points = measurement.lambda_Vec.size();
	}
	if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
	    measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		if(spec_Scat_mode == SPECULAR_MODE) 	num_Points = 1;
		if(spec_Scat_mode == SCATTERED_MODE) 	num_Points = measurement.detector_Theta_Angle_Vec.size();
	}
	if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
	    measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		if(spec_Scat_mode == SPECULAR_MODE) 	num_Points = measurement.beam_Theta_0_Angle_Vec.size();
		if(spec_Scat_mode == SCATTERED_MODE) 	num_Points = measurement.beam_Theta_0_Angle_Vec.size();
	}

	// initial procedure for both SPECULAR_MODE and SCATTERED_MODE
	{
		hi.resize(num_Points);
		U_i_s.resize(num_Points);
		U_r_s.resize(num_Points);
		U_i_p.resize(num_Points);
		U_r_p.resize(num_Points);

		for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
		{
			hi[point_Index].resize(num_Media);
			U_i_s[point_Index].resize(num_Media);
			U_r_s[point_Index].resize(num_Media);
			U_i_p[point_Index].resize(num_Media);
			U_r_p[point_Index].resize(num_Media);
		}
	}

	// reflectance
	if(	unwrapped_Structure->calc_Functions.check_Reflectance ||
	    unwrapped_Structure->calc_Functions.check_Absorptance)
	{
		calculated_Values.Phi_R_s.resize(num_Points);
		calculated_Values.Phi_R_p.resize(num_Points);
		calculated_Values.R_s.resize(num_Points);
		calculated_Values.R_p.resize(num_Points);
		calculated_Values.R  .resize(num_Points);
		calculated_Values.R_Instrumental.resize(num_Points);
	}
	// transmittance
	if(	unwrapped_Structure->calc_Functions.check_Transmittance ||
	    unwrapped_Structure->calc_Functions.check_Absorptance)
	{
		calculated_Values.Phi_T_s.resize(num_Points);
		calculated_Values.Phi_T_p.resize(num_Points);
		calculated_Values.T_s.resize(num_Points);
		calculated_Values.T_p.resize(num_Points);
		calculated_Values.T  .resize(num_Points);
		calculated_Values.T_Instrumental.resize(num_Points);
	}
	// absorptance
	if(	unwrapped_Structure->calc_Functions.check_Absorptance)
	{
		calculated_Values.A_s.resize(num_Points);
		calculated_Values.A_p.resize(num_Points);
		calculated_Values.A  .resize(num_Points);
	}
	// field intensity in-depth
	if(unwrapped_Structure->calc_Functions.check_Field || unwrapped_Structure->calc_Functions.check_Joule)
	{
		if(!unwrapped_Structure->discretization_Parameters.enable_Discretization)
		{
			boundaries_Enlarged = unwrapped_Structure->boundaries;
		} else
		{
			boundaries_Enlarged = unwrapped_Structure->discretized_Slices_Boundaries;
		}
		// plus one element to end
		boundaries_Enlarged.push_back(boundaries_Enlarged.back());

		// if too much slices
//		unwrapped_Structure->num_Field_Slices = min(unwrapped_Structure->num_Field_Slices, 1000000/num_Points);
//		unwrapped_Structure->field_Z_Positions.resize(unwrapped_Structure->num_Field_Slices);

		calculated_Values.field_Intensity.resize(num_Points);
		calculated_Values.absorption_Map.resize(num_Points);
		for(size_t i=0; i<num_Points; i++)
		{
			calculated_Values.field_Intensity[i].resize(unwrapped_Structure->num_Field_Slices);
			calculated_Values.absorption_Map [i].resize(unwrapped_Structure->num_Field_Slices);
		}
	}
	// scattering
	if(	unwrapped_Structure->calc_Functions.check_Scattering)
	{
		calculated_Values.S_s.resize(num_Points);
		calculated_Values.S_p.resize(num_Points);
		calculated_Values.S  .resize(num_Points);
		calculated_Values.S_Instrumental.resize(num_Points);
	}
	// GISAS
	if(	unwrapped_Structure->calc_Functions.check_GISAS && spec_Scat_mode == SCATTERED_MODE)
	{
		phi_Points = measurement.detector_Phi_Angle_Vec.size();

		calculated_Values.GISAS_Instrumental.resize(phi_Points);
		for(size_t i=0; i<phi_Points; i++)
		{
			calculated_Values.GISAS_Instrumental[i].resize(num_Points);
		}

		for(int thread_Index = 0; thread_Index<num_Threads; thread_Index++)
		{
			acc_Vec[thread_Index] = gsl_interp_accel_alloc();
			spline_Vec[thread_Index] = gsl_spline_alloc(gsl_interp_steffen, short_Phi_Points);
			GISAS_Slice[thread_Index].resize(short_Phi_Points);
			phi_Slice[thread_Index].resize(short_Phi_Points);
		}
	}

	/// supplementary
	if(	unwrapped_Structure->calc_Functions.check_Field ||
	    unwrapped_Structure->calc_Functions.check_Joule ||
	    unwrapped_Structure->calc_Functions.check_Scattering ||
	    unwrapped_Structure->calc_Functions.check_GISAS )
	{
		if( spec_Scat_mode == SCATTERED_MODE )
		{
			/// PT
			if(  multilayer->imperfections_Model.vertical_Correlation == zero_Correlation &&
			    !multilayer->imperfections_Model.use_Common_Roughness_Function )
			{
				fill_Item_Id_Map();
				fill_Boundary_Item_PSD(unwrapped_Structure->calc_Tree.begin());
			}
			intensity_Term_Boundary_s.resize(num_Threads);
			intensity_Term_Boundary_p.resize(num_Threads);
			field_Term_Boundary_s	 .resize(num_Threads);
			field_Term_Boundary_p	 .resize(num_Threads);
			half_Sum_Field_Term_s	 .resize(num_Threads);
			half_Sum_Field_Term_p	 .resize(num_Threads);
			cross_Exp_Factor_2D		 .resize(num_Threads);
			PSD_Factor_Boundary		 .resize(num_Threads);

			for(int thread_Index = 0; thread_Index<num_Threads; thread_Index++)
			{
				intensity_Term_Boundary_s[thread_Index].resize(num_Boundaries);
				intensity_Term_Boundary_p[thread_Index].resize(num_Boundaries);
				field_Term_Boundary_s	 [thread_Index].resize(num_Boundaries);
				field_Term_Boundary_p	 [thread_Index].resize(num_Boundaries);
				half_Sum_Field_Term_s	 [thread_Index].resize(num_Boundaries);
				half_Sum_Field_Term_p	 [thread_Index].resize(num_Boundaries);
				cross_Exp_Factor_2D		 [thread_Index].resize(num_Boundaries);
				PSD_Factor_Boundary		 [thread_Index].resize(num_Boundaries);

				if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation)
				{
					for(int boundary_Index = 0; boundary_Index<num_Boundaries; boundary_Index++)
					{
						half_Sum_Field_Term_s[thread_Index][boundary_Index].resize(num_Boundaries-1);
						half_Sum_Field_Term_p[thread_Index][boundary_Index].resize(num_Boundaries-1);
						cross_Exp_Factor_2D	 [thread_Index][boundary_Index].resize(num_Boundaries-1);
					}
				}
			}

			/// DWBA SA CSA
			if( multilayer->imperfections_Model.approximation == DWBA_approximation ||
			    multilayer->imperfections_Model.approximation == SA_approximation   ||
			    multilayer->imperfections_Model.approximation == CSA_approximation  )
			{
				// Hermite polinomials tabulation
				if(multilayer->imperfections_Model.approximation == DWBA_approximation)
				{
					hermites.resize(n_Max_Series-1);
					for(int n=0; n<n_Max_Series-1; n++)
					{
						hermites[n] = hermite(n, 0);
					}
				}

				k1_Up_Boundary.resize(num_Threads);
				k2_Up_Boundary.resize(num_Threads);
				k3_Up_Boundary.resize(num_Threads);
				k4_Up_Boundary.resize(num_Threads);

				k1_Low_Boundary.resize(num_Threads);
				k2_Low_Boundary.resize(num_Threads);
				k3_Low_Boundary.resize(num_Threads);
				k4_Low_Boundary.resize(num_Threads);

				D1_Up.resize(num_Threads);
				D2_Up.resize(num_Threads);
				D3_Up.resize(num_Threads);
				D4_Up.resize(num_Threads);

				D1_Low.resize(num_Threads);
				D2_Low.resize(num_Threads);
				D3_Low.resize(num_Threads);
				D4_Low.resize(num_Threads);

				pre_Fourier_Factor.resize(num_Threads);

				for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
				{
					k1_Up_Boundary[thread_Index].resize(num_Boundaries);
					k2_Up_Boundary[thread_Index].resize(num_Boundaries);
					k3_Up_Boundary[thread_Index].resize(num_Boundaries);
					k4_Up_Boundary[thread_Index].resize(num_Boundaries);

					k1_Low_Boundary[thread_Index].resize(num_Boundaries);
					k2_Low_Boundary[thread_Index].resize(num_Boundaries);
					k3_Low_Boundary[thread_Index].resize(num_Boundaries);
					k4_Low_Boundary[thread_Index].resize(num_Boundaries);

					D1_Up[thread_Index].resize(n_Max_Series);
					D2_Up[thread_Index].resize(n_Max_Series);
					D3_Up[thread_Index].resize(n_Max_Series);
					D4_Up[thread_Index].resize(n_Max_Series);

					D1_Low[thread_Index].resize(n_Max_Series);
					D2_Low[thread_Index].resize(n_Max_Series);
					D3_Low[thread_Index].resize(n_Max_Series);
					D4_Low[thread_Index].resize(n_Max_Series);

					pre_Fourier_Factor[thread_Index].resize(n_Max_Series);
				}

				// s-polarization
				if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
				{
					b1_Up_Boundary_s.resize(num_Threads);
					b2_Up_Boundary_s.resize(num_Threads);
					b3_Up_Boundary_s.resize(num_Threads);
					b4_Up_Boundary_s.resize(num_Threads);

					b1_Low_Boundary_s.resize(num_Threads);
					b2_Low_Boundary_s.resize(num_Threads);
					b3_Low_Boundary_s.resize(num_Threads);
					b4_Low_Boundary_s.resize(num_Threads);

					K_Factor_Boundary_s.resize(num_Threads);

					for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
					{
						b1_Up_Boundary_s[thread_Index].resize(num_Boundaries);
						b2_Up_Boundary_s[thread_Index].resize(num_Boundaries);
						b3_Up_Boundary_s[thread_Index].resize(num_Boundaries);
						b4_Up_Boundary_s[thread_Index].resize(num_Boundaries);

						b1_Low_Boundary_s[thread_Index].resize(num_Boundaries);
						b2_Low_Boundary_s[thread_Index].resize(num_Boundaries);
						b3_Low_Boundary_s[thread_Index].resize(num_Boundaries);
						b4_Low_Boundary_s[thread_Index].resize(num_Boundaries);

						K_Factor_Boundary_s[thread_Index].resize(num_Boundaries);

						for(int bound=0; bound<num_Boundaries; bound++)
						{
							K_Factor_Boundary_s[thread_Index][bound].resize(n_Max_Series);
						}
					}
				}

				// p-polarization
				if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
				{
					b1_Up_Boundary_p.resize(num_Threads);
					b2_Up_Boundary_p.resize(num_Threads);
					b3_Up_Boundary_p.resize(num_Threads);
					b4_Up_Boundary_p.resize(num_Threads);

					b1_Low_Boundary_p.resize(num_Threads);
					b2_Low_Boundary_p.resize(num_Threads);
					b3_Low_Boundary_p.resize(num_Threads);
					b4_Low_Boundary_p.resize(num_Threads);

					K_Factor_Boundary_p.resize(num_Threads);

					for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
					{
						b1_Up_Boundary_p[thread_Index].resize(num_Boundaries);
						b2_Up_Boundary_p[thread_Index].resize(num_Boundaries);
						b3_Up_Boundary_p[thread_Index].resize(num_Boundaries);
						b4_Up_Boundary_p[thread_Index].resize(num_Boundaries);

						b1_Low_Boundary_p[thread_Index].resize(num_Boundaries);
						b2_Low_Boundary_p[thread_Index].resize(num_Boundaries);
						b3_Low_Boundary_p[thread_Index].resize(num_Boundaries);
						b4_Low_Boundary_p[thread_Index].resize(num_Boundaries);

						K_Factor_Boundary_p[thread_Index].resize(num_Boundaries);

						for(int bound=0; bound<num_Boundaries; bound++)
						{
							K_Factor_Boundary_p[thread_Index][bound].resize(n_Max_Series);
						}
					}
				}
			}
		}

		// s-polarization
		if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
		{
			/// PT
			if(spec_Scat_mode == SPECULAR_MODE)
			{
				calculated_Values.q0_Boundary_Field_s.resize(num_Points);
				for(size_t i=0; i<num_Points; i++)		{
					calculated_Values.q0_Boundary_Field_s[i].resize(num_Boundaries);
				}
			}
			if(spec_Scat_mode == SCATTERED_MODE)
			{
				calculated_Values.q_Boundary_Field_s.resize(num_Points);
				for(size_t i=0; i<num_Points; i++)		{
					calculated_Values.q_Boundary_Field_s[i].resize(num_Boundaries);
				}
			}
		}
		// p-polarization
		if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
		{
			/// PT
			if(spec_Scat_mode == SPECULAR_MODE)
			{
				calculated_Values.q0_Boundary_Field_p.resize(num_Points);
				for(size_t i=0; i<num_Points; i++)		{
					calculated_Values.q0_Boundary_Field_p[i].resize(num_Boundaries);
				}
			}
			if(spec_Scat_mode == SCATTERED_MODE)
			{
				calculated_Values.q_Boundary_Field_p.resize(num_Points);
				for(size_t i=0; i<num_Points; i++)		{
					calculated_Values.q_Boundary_Field_p[i].resize(num_Boundaries);
				}
			}
		}

		// for getting common PSD parameters
		int substrate_Index = unwrapped_Structure->calc_Tree.begin().number_of_children()-1;
		tree<Node>::post_order_iterator substrate_Child = tree<Node>::child(unwrapped_Structure->calc_Tree.begin(), substrate_Index);
		substrate_Node = &(substrate_Child.node->data);
		substrate = substrate_Node->struct_Data;
		if(substrate.item_Type != item_Type_Substrate ) {qInfo() << "Unwrapped_Reflection::Unwrapped_Reflection  :  last item is not substrate!" << endl;}
	}
}

Unwrapped_Reflection::~Unwrapped_Reflection()
{
	if(	unwrapped_Structure->calc_Functions.check_GISAS && spec_Scat_mode == SCATTERED_MODE)
	{
		for(int thread_Index = 0; thread_Index<num_Threads; thread_Index++)
		{
			gsl_spline_free(spline_Vec[thread_Index]);
			gsl_interp_accel_free(acc_Vec[thread_Index]);
		}
	}
}

int Unwrapped_Reflection::fill_s__Max_Depth_3(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		const Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			hi         [point_Index ][media_Index  ] = child.node->data.hi         [point_Index];
			r_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s[point_Index] * child.node->data.weak_Factor_T[point_Index];
			exponenta  [thread_Index][media_Index-1] = child.node->data.exponenta  [point_Index];
			exponenta_2[thread_Index][media_Index-1] = child.node->data.exponenta_2[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Substrate )
		{
			epsilon_Substrate[thread_Index] = child.node->data.epsilon[point_Index];
			hi         [point_Index ][media_Index  ] = child.node->data.hi		   [point_Index];
			r_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s[point_Index] * child.node->data.weak_Factor_T[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Ambient)
		{
			epsilon_Ambient[thread_Index] = child.node->data.epsilon[point_Index];
			hi[point_Index ][media_Index] = child.node->data.hi[point_Index];
			++media_Index;
		} else
		if( child_Data.item_Type == item_Type_Multilayer ||
		    child_Data.item_Type == item_Type_Regular_Aperiodic	)
		{
			int start_Period = 0;
			if(child_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
			{	start_Period = 1;	}

			if(max_Depth == 2)
			{
				for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
				{
					for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
					{
						tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
						hi         [point_Index ][media_Index  ] = grandchild.node->data.hi			[point_Index];
						r_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_s[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
						t_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_s[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
						exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta  [point_Index];
						exponenta_2[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2[point_Index];
						++media_Index;
					}
				}
			} else
			{
				for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
				{
					for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
					{
						tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
						const Data& grandchild_Data = grandchild.node->data.struct_Data;

						if(grandchild_Data.item_Type == item_Type_Layer)
						{
							hi         [point_Index ][media_Index  ] = grandchild.node->data.hi			[point_Index];
							r_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_s[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
							t_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_s[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
							exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta  [point_Index];
							exponenta_2[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2[point_Index];
							++media_Index;
						} else
						if( grandchild_Data.item_Type == item_Type_Multilayer ||
						    grandchild_Data.item_Type == item_Type_Regular_Aperiodic )
						{
							int start_Period_1 = 0;
							if(grandchild_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
							{	start_Period_1 = 1;	}

							for(int period_Index_1=start_Period_1; period_Index_1<grandchild_Data.num_Repetition.value(); ++period_Index_1)
							{
								for(unsigned grandgrandchild_Index=0; grandgrandchild_Index<grandchild.number_of_children(); ++grandgrandchild_Index)
								{
									tree<Node>::post_order_iterator grandgrandchild = tree<Node>::child(grandchild,grandgrandchild_Index);
									hi         [point_Index ][media_Index  ] = grandgrandchild.node->data.hi		 [point_Index];
									r_Fresnel_s[thread_Index][media_Index-1] = grandgrandchild.node->data.Fresnel_R_s[point_Index] * grandgrandchild.node->data.weak_Factor_R[point_Index];
									t_Fresnel_s[thread_Index][media_Index-1] = grandgrandchild.node->data.Fresnel_T_s[point_Index] * grandgrandchild.node->data.weak_Factor_T[point_Index];
									exponenta  [thread_Index][media_Index-1] = grandgrandchild.node->data.exponenta  [point_Index];
									exponenta_2[thread_Index][media_Index-1] = grandgrandchild.node->data.exponenta_2[point_Index];
									++media_Index;
								}
							}
						}
					}
				}
			}
		}
	}
	return media_Index;
}
int Unwrapped_Reflection::fill_p__Max_Depth_3(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		const Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			hi         [point_Index ][media_Index  ] = child.node->data.hi		   [point_Index];
			r_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p[point_Index] * child.node->data.weak_Factor_T[point_Index];
			exponenta  [thread_Index][media_Index-1] = child.node->data.exponenta  [point_Index];
			exponenta_2[thread_Index][media_Index-1] = child.node->data.exponenta_2[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Substrate )
		{
			epsilon_Substrate[thread_Index] = child.node->data.epsilon[point_Index];
			hi         [point_Index ][media_Index  ] = child.node->data.hi		   [point_Index];
			r_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p[point_Index] * child.node->data.weak_Factor_T[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Ambient)
		{
			epsilon_Ambient[thread_Index] = child.node->data.epsilon[point_Index];
			hi[point_Index ][media_Index] = child.node->data.hi[point_Index];
			++media_Index;
		} else
		if( child_Data.item_Type == item_Type_Multilayer ||
		    child_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			int start_Period = 0;
			if(child_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
			{	start_Period = 1;	}

			if(max_Depth == 2)
			{
				for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
				{
					for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
					{
						tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
						hi         [point_Index ][media_Index  ] = grandchild.node->data.hi		    [point_Index];
						r_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_p[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
						t_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_p[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
						exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta  [point_Index];
						exponenta_2[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2[point_Index];
						++media_Index;
					}
				}
			} else
			{
				for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
				{
					for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
					{
						tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
						const Data& grandchild_Data = grandchild.node->data.struct_Data;

						if(grandchild_Data.item_Type == item_Type_Layer)
						{
							hi         [point_Index ][media_Index  ] = grandchild.node->data.hi			[point_Index];
							r_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_p[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
							t_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_p[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
							exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta  [point_Index];
							exponenta_2[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2[point_Index];
							++media_Index;
						} else
						if( grandchild_Data.item_Type == item_Type_Multilayer ||
						    grandchild_Data.item_Type == item_Type_Regular_Aperiodic )
						{
							int start_Period_1 = 0;
							if(grandchild_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
							{	start_Period_1 = 1;	}

							for(int period_Index_1=start_Period_1; period_Index_1<grandchild_Data.num_Repetition.value(); ++period_Index_1)
							{
								for(unsigned grandgrandchild_Index=0; grandgrandchild_Index<grandchild.number_of_children(); ++grandgrandchild_Index)
								{
									tree<Node>::post_order_iterator grandgrandchild = tree<Node>::child(grandchild,grandgrandchild_Index);
									hi         [point_Index ][media_Index  ] = grandgrandchild.node->data.hi		 [point_Index];
									r_Fresnel_p[thread_Index][media_Index-1] = grandgrandchild.node->data.Fresnel_R_p[point_Index] * grandgrandchild.node->data.weak_Factor_R[point_Index];
									t_Fresnel_p[thread_Index][media_Index-1] = grandgrandchild.node->data.Fresnel_T_p[point_Index] * grandgrandchild.node->data.weak_Factor_T[point_Index];
									exponenta  [thread_Index][media_Index-1] = grandgrandchild.node->data.exponenta  [point_Index];
									exponenta_2[thread_Index][media_Index-1] = grandgrandchild.node->data.exponenta_2[point_Index];
									++media_Index;
								}
							}
						}
					}
				}
			}
		}
	}
	return media_Index;
}
int Unwrapped_Reflection::fill_sp_Max_Depth_3(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		const Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			hi         [point_Index ][media_Index  ] = child.node->data.hi         [point_Index];
			r_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p[point_Index] * child.node->data.weak_Factor_T[point_Index];
			exponenta  [thread_Index][media_Index-1] = child.node->data.exponenta  [point_Index];
			exponenta_2[thread_Index][media_Index-1] = child.node->data.exponenta_2[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Substrate )
		{
			epsilon_Substrate[thread_Index] = child.node->data.epsilon[point_Index];
			hi         [point_Index ][media_Index  ] = child.node->data.hi[point_Index];
			r_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p[point_Index] * child.node->data.weak_Factor_T[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Ambient)
		{
			epsilon_Ambient[thread_Index] = child.node->data.epsilon[point_Index];
			hi[point_Index ][media_Index] = child.node->data.hi[point_Index];
			++media_Index;
		} else
		if( child_Data.item_Type == item_Type_Multilayer ||
		    child_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			int start_Period = 0;
			if(child_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
			{   start_Period = 1;	}

			if(max_Depth == 2)
			{
				for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
				{
					for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
					{
						tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
						hi         [point_Index ][media_Index  ] = grandchild.node->data.hi			[point_Index];
						r_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_s[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
						r_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_p[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
						t_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_s[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
						t_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_p[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
						exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta  [point_Index];
						exponenta_2[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2[point_Index];
						++media_Index;
					}
				}
			} else
			{
				for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
				{
					for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
					{
						tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
						const Data& grandchild_Data = grandchild.node->data.struct_Data;

						if(grandchild_Data.item_Type == item_Type_Layer)
						{
							hi         [point_Index ][media_Index  ] = grandchild.node->data.hi			[point_Index];
							r_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_s[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
							r_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_p[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
							t_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_s[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
							t_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_p[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
							exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta  [point_Index];
							exponenta_2[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2[point_Index];
							++media_Index;
						} else
						if( grandchild_Data.item_Type == item_Type_Multilayer ||
						    grandchild_Data.item_Type == item_Type_Regular_Aperiodic )
						{
							int start_Period_1 = 0;
							if(grandchild_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
							{	start_Period_1 = 1;	}

							for(int period_Index_1=start_Period_1; period_Index_1<grandchild_Data.num_Repetition.value(); ++period_Index_1)
							{
								for(unsigned grandgrandchild_Index=0; grandgrandchild_Index<grandchild.number_of_children(); ++grandgrandchild_Index)
								{
									tree<Node>::post_order_iterator grandgrandchild = tree<Node>::child(grandchild,grandgrandchild_Index);
									hi         [point_Index ][media_Index  ] = grandgrandchild.node->data.hi		 [point_Index];
									r_Fresnel_s[thread_Index][media_Index-1] = grandgrandchild.node->data.Fresnel_R_s[point_Index] * grandgrandchild.node->data.weak_Factor_R[point_Index];
									r_Fresnel_p[thread_Index][media_Index-1] = grandgrandchild.node->data.Fresnel_R_p[point_Index] * grandgrandchild.node->data.weak_Factor_R[point_Index];
									t_Fresnel_s[thread_Index][media_Index-1] = grandgrandchild.node->data.Fresnel_T_s[point_Index] * grandgrandchild.node->data.weak_Factor_T[point_Index];
									t_Fresnel_p[thread_Index][media_Index-1] = grandgrandchild.node->data.Fresnel_T_p[point_Index] * grandgrandchild.node->data.weak_Factor_T[point_Index];
									exponenta  [thread_Index][media_Index-1] = grandgrandchild.node->data.exponenta  [point_Index];
									exponenta_2[thread_Index][media_Index-1] = grandgrandchild.node->data.exponenta_2[point_Index];
									++media_Index;
								}
							}
						}
					}
				}
			}
		}
	}
	return media_Index;
}

void Unwrapped_Reflection::fill_Item_Id_Map()
{
	tree<Node>::pre_order_iterator iter = tree<Node>::child(unwrapped_Structure->calc_Tree.begin(),0);

	int counter = 0;
	appropriate_Item_Vec.clear();
	appropriate_Node_Vec.clear();
	while(unwrapped_Structure->calc_Tree.is_valid(iter))
	{
		Node& node = iter.node->data;
		const Data& item = node.struct_Data;

		if(item.item_Type == item_Type_Layer || item.item_Type == item_Type_Substrate)
		{
			id_Item_Map.insert(item.id, counter);
			appropriate_Item_Vec.push_back(item);
			appropriate_Node_Vec.push_back(node);
			counter++;
		}
		iter++;
	}

	for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
	{
		PSD_Factor_Item[thread_Index].resize(counter);
	}
}

inline void Unwrapped_Reflection::fill_Item_PSD_1D(int thread_Index, double cos_Theta, double cos_Theta_0)
{
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size(); item_Index++)
	{
		Node& node = appropriate_Node_Vec[item_Index];
		Data& item = appropriate_Item_Vec[item_Index];

		choose_PSD_1D_Function(item, thread_Index);
		double value = PSD_1D_Func_Vec[thread_Index](item.PSD_ABC_1D_Factor,
		                                             item.roughness_Model.cor_radius.value,
		                                             item.roughness_Model.fractal_alpha.value,
		                                             measurement.k_Value,
		                                             cos_Theta,
		                                             cos_Theta_0,
		                                             node.spline,
		                                             node.acc);
		PSD_Factor_Item[thread_Index][item_Index] = max(value,0.);
	}
}

inline void Unwrapped_Reflection::fill_Item_PSD_2D(int thread_Index, int point_Index, int phi_Index)
{
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size(); item_Index++)
	{
		Data& item = appropriate_Item_Vec[item_Index];
		double value = PSD_2D_Func_Vec[thread_Index](item.PSD_ABC_2D_Factor,
		                                             item.roughness_Model.cor_radius.value,
		                                             item.roughness_Model.fractal_alpha.value,
		                                             measurement.k_Value,
		                                             measurement.detector_Theta_Cos_Vec[point_Index],
		                                             measurement.beam_Theta_0_Cos_Value,
		                                             measurement.detector_Phi_Cos_Vec[phi_Index]);
		PSD_Factor_Item[thread_Index][item_Index] = value;
	}
}

int Unwrapped_Reflection::fill_Boundary_Item_PSD(const tree<Node>::iterator& parent, int boundary_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		Node& child_Node = child.node->data;
		const Data& child_Data = child_Node.struct_Data;

		if(child_Data.item_Type == item_Type_Layer || child_Data.item_Type == item_Type_Substrate)
		{
			boundary_Item_Vec[boundary_Index] = id_Item_Map.value(child_Data.id);
			++boundary_Index;
		} else
		if( child_Data.item_Type == item_Type_Multilayer ||
		    child_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			int start_Period = 0;
			if(child_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
			{   start_Period = 1;	}

			if(max_Depth == 2)
			{
				for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
				{
					for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
					{
						const Data& grandchild_Data = tree<Node>::child(child,grandchild_Index).node->data.struct_Data;
						boundary_Item_Vec[boundary_Index] = id_Item_Map.value(grandchild_Data.id);
						++boundary_Index;
					}
				}
			} else
			{
				if(max_Depth == 3)
				{
					for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
					{
						for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
						{
							tree<Node>::post_order_iterator grandchild = tree<Node>::child(parent,child_Index);
							const Data& grandchild_Data = grandchild.node->data.struct_Data;

							if(grandchild_Data.item_Type == item_Type_Layer)
							{
								boundary_Item_Vec[boundary_Index] = id_Item_Map.value(grandchild_Data.id);
								++boundary_Index;
							} else
							if( grandchild_Data.item_Type == item_Type_Multilayer ||
							    grandchild_Data.item_Type == item_Type_Regular_Aperiodic )
							{
								int start_Period_1 = 0;
								if(grandchild_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
								{	start_Period_1 = 1;	}

								for(int period_Index_1=start_Period_1; period_Index_1<grandchild_Data.num_Repetition.value(); ++period_Index_1)
								{
									for(unsigned grandgrandchild_Index=0; grandgrandchild_Index<grandchild.number_of_children(); ++grandgrandchild_Index)
									{
										const Data& grandgrandchild_Data = tree<Node>::child(grandchild,grandgrandchild_Index).node->data.struct_Data;
										boundary_Item_Vec[boundary_Index] = id_Item_Map.value(grandgrandchild_Data.id);
										++boundary_Index;
									}
								}
							}
						}
					}
				} else // if max_Depth >= 4
				{
					boundary_Index = fill_Boundary_Item_PSD(child, boundary_Index);
				}
			}
		}
	}
	return boundary_Index;
}

void Unwrapped_Reflection::fill_Epsilon_Ambient_Substrate(int thread_Index, const vector<complex<double>>& epsilon_Vector)
{
	epsilon_Ambient  [thread_Index] = epsilon_Vector.front();
	epsilon_Substrate[thread_Index] = epsilon_Vector.back();
}

void Unwrapped_Reflection::calc_Hi(int point_Index, double k, double cos2, const vector<complex<double>>& epsilon_Vector)
{
	for (int i = 0; i < num_Media; ++i)
	{
		hi[point_Index][i] = k*sqrt(epsilon_Vector[i] - cos2);
	}
}

void Unwrapped_Reflection::calc_Weak_Factor(int thread_Index, int point_Index)
{
	bool is_Norm = false;
	double norm, a = M_PI/sqrt(M_PI*M_PI - 8.);
	complex<double> s_r, factor_r, x_r, y_r, six_r, siy_r;
	complex<double> s_t, factor_t, x_t, y_t, six_t, siy_t;
	double my_Sigma_Diffuse = 0;
	for (int i = 0; i < num_Boundaries; ++i)
	{
		// if >=1 interlayer is turned on
		is_Norm = false;
		for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
		{
			is_Norm = is_Norm || unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][func_Index].enabled;
		}

		weak_Factor_R[thread_Index][i] = 0;
		weak_Factor_T[thread_Index][i] = 0;

		if(is_Norm && (abs(unwrapped_Structure->sigma_Diffuse[i]) > DBL_MIN)) //-V674
		{
			my_Sigma_Diffuse = unwrapped_Structure->sigma_Diffuse[i];	// by default, otherwise we change it
			norm = 0;
			s_r = sqrt(hi[point_Index][i]*hi[point_Index][i+1]);
			s_t =     (hi[point_Index][i]-hi[point_Index][i+1])/2.;
			//-------------------------------------------------------------------------------
			// erf interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Erf].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Erf].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Erf].interlayer.value;
				if(!unwrapped_Structure->common_Sigma_Diffuse[i]) {
					my_Sigma_Diffuse = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Erf].my_Sigma_Diffuse.value; }

				// reflectance
				factor_r = exp( - s_r * s_r * my_Sigma_Diffuse * my_Sigma_Diffuse * 2.);
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Erf].interlayer.value * factor_r;

				// transmittance
				factor_t = exp( - s_t * s_t * my_Sigma_Diffuse * my_Sigma_Diffuse * 2.);
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Erf].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// lin interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Lin].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Lin].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Lin].interlayer.value;
				if(!unwrapped_Structure->common_Sigma_Diffuse[i]) {
					my_Sigma_Diffuse = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Lin].my_Sigma_Diffuse.value; }

				// reflectance
				x_r = sqrt(3.) * s_r * my_Sigma_Diffuse * 2.;
				if(abs(x_r)>DBL_MIN)	{
					factor_r = sin(x_r) / (x_r);
				} else {
					factor_r = 1;
				}
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Lin].interlayer.value * factor_r;

				// transmittance
				x_t = sqrt(3.) * s_t * my_Sigma_Diffuse * 2.;
				if(abs(x_t)>DBL_MIN)	{
					factor_t = sin(x_t) / (x_t);
				} else {
					factor_t = 1;
				}
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Lin].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// exp interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Exp].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Exp].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Exp].interlayer.value;
				if(!unwrapped_Structure->common_Sigma_Diffuse[i]) {
					my_Sigma_Diffuse = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Exp].my_Sigma_Diffuse.value; }

				// reflectance
				x_r = 2. * pow(s_r * my_Sigma_Diffuse, 2);
				factor_r = 1. / (1. + x_r);
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Exp].interlayer.value * factor_r;

				// transmittance
				x_t = 2. * pow(s_t * my_Sigma_Diffuse, 2);
				factor_t = 1. / (1. + x_t);
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Exp].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// tanh interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Tanh].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Tanh].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Tanh].interlayer.value;
				if(!unwrapped_Structure->common_Sigma_Diffuse[i]) {
					my_Sigma_Diffuse = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Tanh].my_Sigma_Diffuse.value; }

				// reflectance
				x_r = 2 * sqrt(3.) * s_r * my_Sigma_Diffuse;
				if(abs(x_r)>DBL_MIN)				{
					factor_r = x_r / sinh(x_r);
				} else {
					factor_r = 1;
				}
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Tanh].interlayer.value * factor_r;

				// transmittance
				x_t = 2 * sqrt(3.) * s_t * my_Sigma_Diffuse;
				if(abs(x_t)>DBL_MIN)				{
					factor_t = x_t / sinh(x_t);
				} else {
					factor_t = 1;
				}
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Tanh].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// sin interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Sin].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Sin].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Sin].interlayer.value;
				if(!unwrapped_Structure->common_Sigma_Diffuse[i]) {
					my_Sigma_Diffuse = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Sin].my_Sigma_Diffuse.value; }

				// reflectance
				x_r = 2 * a * s_r * my_Sigma_Diffuse - M_PI_2;
				y_r = x_r + M_PI;
				if(abs(x_r)>DBL_MIN) six_r = sin(x_r)/x_r; else six_r = 1;
				if(abs(y_r)>DBL_MIN) siy_r = sin(y_r)/y_r; else siy_r = 1;
				factor_r = M_PI_4 * (six_r + siy_r);
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Sin].interlayer.value * factor_r;

				// transmittance
				x_t = 2 * a * s_t * my_Sigma_Diffuse - M_PI_2;
				y_t = x_t + M_PI;
				if(abs(x_t)>DBL_MIN) six_t = sin(x_t)/x_t; else six_t = 1;
				if(abs(y_t)>DBL_MIN) siy_t = sin(y_t)/y_t; else siy_t = 1;
				factor_t = M_PI_4 * (six_t + siy_t);
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Sin].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// step interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Step].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Step].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Step].interlayer.value;
				if(!unwrapped_Structure->common_Sigma_Diffuse[i]) {
					my_Sigma_Diffuse = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Step].my_Sigma_Diffuse.value; }

				// reflectance
				factor_r = cos(2. * s_r * my_Sigma_Diffuse);
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Step].interlayer.value * factor_r;

				// transmittance
				factor_t = cos(2. * s_t * my_Sigma_Diffuse);
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Step].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// normalization
			{
				if( abs(norm) > DBL_MIN )
				{
					weak_Factor_R[thread_Index][i] /= norm;
					weak_Factor_T[thread_Index][i] /= norm;
				} else
				{
					weak_Factor_R[thread_Index][i] = 1;
					weak_Factor_T[thread_Index][i] = 1;
				}
			}
		} else
		{
			weak_Factor_R[thread_Index][i] = 1;
			weak_Factor_T[thread_Index][i] = 1;
		}
	}
}

void Unwrapped_Reflection::calc_Fresnel(int thread_Index, int point_Index, const vector<complex<double>>& epsilon_Vector)
{
	// s-polarization
	if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
	{
		// reflectance only
		if( calc_Functions.if_Reflectance_Only() )
		{
			for (int i = 0; i < num_Boundaries; ++i)
			{
				if ( abs(hi[point_Index][i] + hi[point_Index][i+1]) > DBL_MIN )
				{
					r_Fresnel_s[thread_Index][i] = weak_Factor_R[thread_Index][i] * (hi[point_Index][i] - hi[point_Index][i+1]) /
					                                                                (hi[point_Index][i] + hi[point_Index][i+1]);
				} else
				{
					r_Fresnel_s[thread_Index][i] = 0;
				}
			}
		} else
		// reflectance and transmittance
		{
			for (int i = 0; i < num_Boundaries; ++i)
			{
				if ( abs(hi[point_Index][i] + hi[point_Index][i+1]) > DBL_MIN )
				{
					// reflectance
					r_Fresnel_s[thread_Index][i] = weak_Factor_R[thread_Index][i] * (hi[point_Index][i] - hi[point_Index][i+1]) /
					                                                                (hi[point_Index][i] + hi[point_Index][i+1]);
					// transmittance
					t_Fresnel_s[thread_Index][i] = weak_Factor_T[thread_Index][i] * 2.*hi[point_Index][i] / (hi[point_Index][i] + hi[point_Index][i+1]);
				} else
				{
					// reflectance
					r_Fresnel_s[thread_Index][i] = 0;
					// transmittance
					t_Fresnel_s[thread_Index][i] = 1;
				}
			}
		}
	}
	// p-polarization
	if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
	{
		complex<double> hi_je, hi_j1e;
		// reflectance only
		if( calc_Functions.if_Reflectance_Only() )
		{
			for (int i = 0; i < num_Boundaries; ++i)
			{
				hi_je  = hi[point_Index][i  ]/epsilon_Vector[i];
				hi_j1e = hi[point_Index][i+1]/epsilon_Vector[i+1];

				if ( abs(hi_je+hi_j1e) > DBL_MIN )
				{
					r_Fresnel_p[thread_Index][i] = weak_Factor_R[thread_Index][i] * (hi_je-hi_j1e)/ (hi_je+hi_j1e);
				} else
				{
					r_Fresnel_p[thread_Index][i] = 0;
				}
			}
		} else
		// reflectance and transmittance
		{
			for (int i = 0; i < num_Boundaries; ++i)
			{
				hi_je  = hi[point_Index][i  ]/epsilon_Vector[i];
				hi_j1e = hi[point_Index][i+1]/epsilon_Vector[i+1];

				if ( abs(hi_je+hi_j1e) > DBL_MIN )
				{
					// reflectance
					r_Fresnel_p[thread_Index][i] = weak_Factor_R[thread_Index][i] * (hi_je-hi_j1e)/ (hi_je+hi_j1e);
					// transmittance
					t_Fresnel_p[thread_Index][i] = weak_Factor_T[thread_Index][i] * 2.*hi_je/ (hi_je+hi_j1e);
				} else
				{
					// reflectance
					r_Fresnel_p[thread_Index][i] = 0;
					// transmittance
					t_Fresnel_p[thread_Index][i] = 1;
				}
			}
		}
	}
}

void Unwrapped_Reflection::calc_Exponenta(int thread_Index, int point_Index, const vector<double>& thickness)
{
	// reflectance only
	if( calc_Functions.if_Reflectance_Only() )
	{
		for (int i = 0; i < num_Layers; ++i)
		{
			exponenta_2[thread_Index][i] = exp(2.*I*hi[point_Index][i+1]*thickness[i]);
		}
	} else
	// reflectance and transmittance
	{
		for (int i = 0; i < num_Layers; ++i)
		{
			exponenta[thread_Index][i] = exp(I*hi[point_Index][i+1]*thickness[i]);
			exponenta_2[thread_Index][i] = pow(exponenta[thread_Index][i],2);
		}
	}
}

void Unwrapped_Reflection::calc_Local(int thread_Index, int point_Index)
{
	// s-polarization
	if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
	{
		// reflectance only
		if( calc_Functions.if_Reflectance_Only() )
		{
			complex<double> r_exp;
			r_Local_s[thread_Index].back() = r_Fresnel_s[thread_Index].back();	// last boundary
			for (int i = num_Layers-1; i >= 0; --i)
			{
				r_exp = r_Local_s[thread_Index][i+1]*exponenta_2[thread_Index][i];
				r_Local_s[thread_Index][i] = (r_Fresnel_s[thread_Index][i] + r_exp) / (1. + r_Fresnel_s[thread_Index][i]*r_exp);
			}
		} else
		// reflectance and transmittance
		{
			complex<double> r_exp,denom;
			r_Local_s[thread_Index].back() = r_Fresnel_s[thread_Index].back();	// last boundary
			t_Local_s[thread_Index].back() = t_Fresnel_s[thread_Index].back();	// last boundary
			for (int i = num_Layers-1; i >= 0; --i)
			{
				r_exp = r_Local_s[thread_Index][i+1]*exponenta_2[thread_Index][i];
				denom = 1. + r_Fresnel_s[thread_Index][i]*r_exp;
				r_Local_s[thread_Index][i] = (r_Fresnel_s[thread_Index][i] + r_exp) / denom;

				t_Local_s[thread_Index][i] = (t_Local_s[thread_Index][i+1]*t_Fresnel_s[thread_Index][i]*exponenta[thread_Index][i]) / denom;
			}
		}
	}
	// p-polarization
	if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
	{
		// reflectance only
		if( calc_Functions.if_Reflectance_Only() )
		{
			complex<double> r_exp;
			r_Local_p[thread_Index].back() = r_Fresnel_p[thread_Index].back();	// last boundary
			for (int i = num_Layers-1; i >= 0; --i)
			{
				r_exp = r_Local_p[thread_Index][i+1]*exponenta_2[thread_Index][i];
				r_Local_p[thread_Index][i] = (r_Fresnel_p[thread_Index][i] + r_exp) / (1. + r_Fresnel_p[thread_Index][i]*r_exp);
			}
		} else
		// reflectance and transmittance
		{
			complex<double> r_exp,denom;
			r_Local_p[thread_Index].back() = r_Fresnel_p[thread_Index].back();	// last boundary
			t_Local_p[thread_Index].back() = t_Fresnel_p[thread_Index].back();	// last boundary
			for (int i = num_Layers-1; i >= 0; --i)
			{
				r_exp = r_Local_p[thread_Index][i+1]*exponenta_2[thread_Index][i];
				denom = 1. + r_Fresnel_p[thread_Index][i]*r_exp;
				r_Local_p[thread_Index][i] = (r_Fresnel_p[thread_Index][i] + r_exp) / denom;

				t_Local_p[thread_Index][i] = (t_Local_p[thread_Index][i+1]*t_Fresnel_p[thread_Index][i]*exponenta[thread_Index][i]) / denom;
			}
		}
	}
}

void Unwrapped_Reflection::calc_Amplitudes_Field(int thread_Index, int point_Index, QString polarization)
{
	vector<vector<complex<double>>>* boundary_Field;
	vector<vector<complex<double>>>* U_i;
	vector<vector<complex<double>>>* U_r;
	vector<vector<complex<double>>>* r_Local;
	vector<vector<complex<double>>>* t_Local;

	if(polarization == "s")
	{
		r_Local = &(r_Local_s);
		t_Local = &(t_Local_s);

		if(spec_Scat_mode == SPECULAR_MODE)
		{
			boundary_Field = &(calculated_Values.q0_Boundary_Field_s);

			U_i = &(calculated_Values.q0_U_i_s);
			U_r = &(calculated_Values.q0_U_r_s);
		}
		if(spec_Scat_mode == SCATTERED_MODE)
		{
			boundary_Field = &(calculated_Values.q_Boundary_Field_s);

			U_i = &(calculated_Values.q_U_i_s);
			U_r = &(calculated_Values.q_U_r_s);
		}
	}
	if(polarization == "p")
	{
		r_Local = &(r_Local_p);
		t_Local = &(t_Local_p);

		if(spec_Scat_mode == SPECULAR_MODE)
		{
			boundary_Field = &(calculated_Values.q0_Boundary_Field_p);

			U_i = &(calculated_Values.q0_U_i_p);
			U_r = &(calculated_Values.q0_U_r_p);
		}
		if(spec_Scat_mode == SCATTERED_MODE)
		{
			boundary_Field = &(calculated_Values.q_Boundary_Field_p);

			U_i = &(calculated_Values.q_U_i_p);
			U_r = &(calculated_Values.q_U_r_p);
		}
	}

	// main part

	(*U_i)[point_Index].front() = 1;
	(*U_r)[point_Index].front() = (*r_Local)[thread_Index].front();
	(*boundary_Field)[point_Index].front() = (*U_i)[point_Index].front() + (*U_r)[point_Index].front();

	for (int j = 1; j<num_Boundaries; j++)
	{
		(*U_i)[point_Index][j] = (*U_i)[point_Index][j-1] * (*t_Local)[thread_Index][j-1] / (*t_Local)[thread_Index][j];
		(*U_r)[point_Index][j] = (*U_i)[point_Index][j  ] * (*r_Local)[thread_Index][j];
		(*boundary_Field)[point_Index][j] = (*U_i)[point_Index][j] + (*U_r)[point_Index][j];
	}
	(*U_i)[point_Index].back() = (*U_i)[point_Index][num_Boundaries-1] * (*t_Local)[thread_Index].back();
	(*U_r)[point_Index].back() = 0;
}

void Unwrapped_Reflection::calc_k_Wavenumber_DWBA_SA_CSA(int thread_Index, int point_Index)
{
	vector<complex<double>>* q0_Hi;
	for (int boundary_Index = 0; boundary_Index<num_Boundaries; boundary_Index++)
	{
		if( measurement.measurement_Type == measurement_Types[GISAS_Map] ||
		    measurement.measurement_Type == measurement_Types[Detector_Scan] )	{ q0_Hi  = &(calculated_Values.q0_Hi.front()); }

		if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		    measurement.measurement_Type == measurement_Types[Offset_Scan] )	{ q0_Hi  = &(calculated_Values.q0_Hi[point_Index]); }

		k1_Up_Boundary [thread_Index][boundary_Index] =  (*q0_Hi)[boundary_Index  ] + calculated_Values.q_Hi[point_Index][boundary_Index  ];
		k2_Up_Boundary [thread_Index][boundary_Index] =  (*q0_Hi)[boundary_Index  ] - calculated_Values.q_Hi[point_Index][boundary_Index  ];
		k3_Up_Boundary [thread_Index][boundary_Index] = -(*q0_Hi)[boundary_Index  ] + calculated_Values.q_Hi[point_Index][boundary_Index  ];
		k4_Up_Boundary [thread_Index][boundary_Index] = -(*q0_Hi)[boundary_Index  ] - calculated_Values.q_Hi[point_Index][boundary_Index  ];

		k1_Low_Boundary[thread_Index][boundary_Index] =  (*q0_Hi)[boundary_Index+1] + calculated_Values.q_Hi[point_Index][boundary_Index+1];
		k2_Low_Boundary[thread_Index][boundary_Index] =  (*q0_Hi)[boundary_Index+1] - calculated_Values.q_Hi[point_Index][boundary_Index+1];
		k3_Low_Boundary[thread_Index][boundary_Index] = -(*q0_Hi)[boundary_Index+1] + calculated_Values.q_Hi[point_Index][boundary_Index+1];
		k4_Low_Boundary[thread_Index][boundary_Index] = -(*q0_Hi)[boundary_Index+1] - calculated_Values.q_Hi[point_Index][boundary_Index+1];
	}
}

void Unwrapped_Reflection::calc_Field_DWBA_SA_CSA(int thread_Index, int point_Index, QString polarization)
{
	vector<complex<double>>* q0_U_i;
	vector<complex<double>>* q0_U_r;
	vector<complex<double>>* q_U_i;
	vector<complex<double>>* q_U_r;

	vector<complex<double>>* b1_Up_Boundary;
	vector<complex<double>>* b2_Up_Boundary;
	vector<complex<double>>* b3_Up_Boundary;
	vector<complex<double>>* b4_Up_Boundary;

	vector<complex<double>>* b1_Low_Boundary;
	vector<complex<double>>* b2_Low_Boundary;
	vector<complex<double>>* b3_Low_Boundary;
	vector<complex<double>>* b4_Low_Boundary;

	if(polarization == "s")
	{
		if( measurement.measurement_Type == measurement_Types[GISAS_Map] ||
		    measurement.measurement_Type == measurement_Types[Detector_Scan] )	{ q0_U_i = &(calculated_Values.q0_U_i_s.front());
			                                                                      q0_U_r = &(calculated_Values.q0_U_r_s.front());}

		if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		    measurement.measurement_Type == measurement_Types[Offset_Scan] )	{ q0_U_i = &(calculated_Values.q0_U_i_s[point_Index]);
			                                                                      q0_U_r = &(calculated_Values.q0_U_r_s[point_Index]);}
		q_U_i = &(calculated_Values.q_U_i_s[point_Index]);
		q_U_r = &(calculated_Values.q_U_r_s[point_Index]);

		b1_Up_Boundary = &(b1_Up_Boundary_s[thread_Index]);
		b2_Up_Boundary = &(b2_Up_Boundary_s[thread_Index]);
		b3_Up_Boundary = &(b3_Up_Boundary_s[thread_Index]);
		b4_Up_Boundary = &(b4_Up_Boundary_s[thread_Index]);

		b1_Low_Boundary = &(b1_Low_Boundary_s[thread_Index]);
		b2_Low_Boundary = &(b2_Low_Boundary_s[thread_Index]);
		b3_Low_Boundary = &(b3_Low_Boundary_s[thread_Index]);
		b4_Low_Boundary = &(b4_Low_Boundary_s[thread_Index]);
	}
	if(polarization == "p")
	{
		if( measurement.measurement_Type == measurement_Types[GISAS_Map] ||
		    measurement.measurement_Type == measurement_Types[Detector_Scan] )	{ q0_U_i = &(calculated_Values.q0_U_i_p.front());
			                                                                      q0_U_r = &(calculated_Values.q0_U_r_p.front());}

		if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		    measurement.measurement_Type == measurement_Types[Offset_Scan] )	{ q0_U_i = &(calculated_Values.q0_U_i_p[point_Index]);
			                                                                      q0_U_r = &(calculated_Values.q0_U_r_p[point_Index]);}

		q_U_i  = &(calculated_Values.q_U_i_p[point_Index]);
		q_U_r  = &(calculated_Values.q_U_r_p[point_Index]);

		b1_Up_Boundary = &(b1_Up_Boundary_p[thread_Index]);
		b2_Up_Boundary = &(b2_Up_Boundary_p[thread_Index]);
		b3_Up_Boundary = &(b3_Up_Boundary_p[thread_Index]);
		b4_Up_Boundary = &(b4_Up_Boundary_p[thread_Index]);

		b1_Low_Boundary = &(b1_Low_Boundary_p[thread_Index]);
		b2_Low_Boundary = &(b2_Low_Boundary_p[thread_Index]);
		b3_Low_Boundary = &(b3_Low_Boundary_p[thread_Index]);
		b4_Low_Boundary = &(b4_Low_Boundary_p[thread_Index]);
	}

	for (int boundary_Index = 0; boundary_Index<num_Boundaries-1; boundary_Index++)
	{
		(*b1_Up_Boundary)[boundary_Index] = (*q0_U_i)[boundary_Index] * (*q_U_i)[boundary_Index];
		(*b2_Up_Boundary)[boundary_Index] = (*q0_U_i)[boundary_Index] * (*q_U_r)[boundary_Index];
		(*b3_Up_Boundary)[boundary_Index] = (*q0_U_r)[boundary_Index] * (*q_U_i)[boundary_Index];
		(*b4_Up_Boundary)[boundary_Index] = (*q0_U_r)[boundary_Index] * (*q_U_r)[boundary_Index];

		(*b1_Low_Boundary)[boundary_Index] = (*q0_U_i)[boundary_Index+1] * (*q_U_i)[boundary_Index+1] * exp(-I*k1_Low_Boundary[thread_Index][boundary_Index]*unwrapped_Structure->thickness[boundary_Index]);
		(*b2_Low_Boundary)[boundary_Index] = (*q0_U_i)[boundary_Index+1] * (*q_U_r)[boundary_Index+1] * exp(-I*k2_Low_Boundary[thread_Index][boundary_Index]*unwrapped_Structure->thickness[boundary_Index]);
		(*b3_Low_Boundary)[boundary_Index] = (*q0_U_r)[boundary_Index+1] * (*q_U_i)[boundary_Index+1] * exp(-I*k3_Low_Boundary[thread_Index][boundary_Index]*unwrapped_Structure->thickness[boundary_Index]);
		(*b4_Low_Boundary)[boundary_Index] = (*q0_U_r)[boundary_Index+1] * (*q_U_r)[boundary_Index+1] * exp(-I*k4_Low_Boundary[thread_Index][boundary_Index]*unwrapped_Structure->thickness[boundary_Index]);
	}
	// last boundary
	int boundary_Index = num_Boundaries-1;
	{
		(*b1_Up_Boundary)[boundary_Index] = (*q0_U_i)[boundary_Index] * (*q_U_i)[boundary_Index];
		(*b2_Up_Boundary)[boundary_Index] = (*q0_U_i)[boundary_Index] * (*q_U_r)[boundary_Index];
		(*b3_Up_Boundary)[boundary_Index] = (*q0_U_r)[boundary_Index] * (*q_U_i)[boundary_Index];
		(*b4_Up_Boundary)[boundary_Index] = (*q0_U_r)[boundary_Index] * (*q_U_r)[boundary_Index];

		(*b1_Low_Boundary)[boundary_Index] = (*q0_U_i)[boundary_Index+1] * (*q_U_i)[boundary_Index+1];
		(*b2_Low_Boundary)[boundary_Index] = 0;
		(*b3_Low_Boundary)[boundary_Index] = 0;
		(*b4_Low_Boundary)[boundary_Index] = 0;
	}
}

void Unwrapped_Reflection::calc_K_Factor_DWBA_SA_CSA(int thread_Index, QString polarization)
{
	vector<complex<double>>* b1_Up_Boundary;
	vector<complex<double>>* b2_Up_Boundary;
	vector<complex<double>>* b3_Up_Boundary;
	vector<complex<double>>* b4_Up_Boundary;

	vector<complex<double>>* b1_Low_Boundary;
	vector<complex<double>>* b2_Low_Boundary;
	vector<complex<double>>* b3_Low_Boundary;
	vector<complex<double>>* b4_Low_Boundary;

	vector<complex<double>>* b1_Boundary;
	vector<complex<double>>* b2_Boundary;
	vector<complex<double>>* b3_Boundary;
	vector<complex<double>>* b4_Boundary;

	vector<complex<double>>* k1_Boundary;
	vector<complex<double>>* k2_Boundary;
	vector<complex<double>>* k3_Boundary;
	vector<complex<double>>* k4_Boundary;

	vector<complex<double>>* D1;
	vector<complex<double>>* D2;
	vector<complex<double>>* D3;
	vector<complex<double>>* D4;

	vector<vector<complex<double>>>* K_Factor_Boundary;

	if(polarization == "s")
	{
		b1_Up_Boundary = &(b1_Up_Boundary_s[thread_Index]);
		b2_Up_Boundary = &(b2_Up_Boundary_s[thread_Index]);
		b3_Up_Boundary = &(b3_Up_Boundary_s[thread_Index]);
		b4_Up_Boundary = &(b4_Up_Boundary_s[thread_Index]);

		b1_Low_Boundary = &(b1_Low_Boundary_s[thread_Index]);
		b2_Low_Boundary = &(b2_Low_Boundary_s[thread_Index]);
		b3_Low_Boundary = &(b3_Low_Boundary_s[thread_Index]);
		b4_Low_Boundary = &(b4_Low_Boundary_s[thread_Index]);

		K_Factor_Boundary = &(K_Factor_Boundary_s[thread_Index]);
	}
	if(polarization == "p")
	{
		b1_Up_Boundary = &(b1_Up_Boundary_p[thread_Index]);
		b2_Up_Boundary = &(b2_Up_Boundary_p[thread_Index]);
		b3_Up_Boundary = &(b3_Up_Boundary_p[thread_Index]);
		b4_Up_Boundary = &(b4_Up_Boundary_p[thread_Index]);

		b1_Low_Boundary = &(b1_Low_Boundary_p[thread_Index]);
		b2_Low_Boundary = &(b2_Low_Boundary_p[thread_Index]);
		b3_Low_Boundary = &(b3_Low_Boundary_p[thread_Index]);
		b4_Low_Boundary = &(b4_Low_Boundary_p[thread_Index]);

		K_Factor_Boundary = &(K_Factor_Boundary_p[thread_Index]);
	}

	if(multilayer->imperfections_Model.approximation == SA_approximation)
	{
		b1_Boundary = b1_Low_Boundary;
		b2_Boundary = b2_Low_Boundary;
		b3_Boundary = b3_Low_Boundary;
		b4_Boundary = b4_Low_Boundary;

		k1_Boundary = &(k1_Low_Boundary[thread_Index]);
		k2_Boundary = &(k2_Low_Boundary[thread_Index]);
		k3_Boundary = &(k3_Low_Boundary[thread_Index]);
		k4_Boundary = &(k4_Low_Boundary[thread_Index]);

		D1 = &(D1_Low[thread_Index]);
		D2 = &(D2_Low[thread_Index]);
		D3 = &(D3_Low[thread_Index]);
		D4 = &(D4_Low[thread_Index]);
	}
	if(multilayer->imperfections_Model.approximation == CSA_approximation)
	{
		b1_Boundary = b1_Up_Boundary;
		b2_Boundary = b2_Up_Boundary;
		b3_Boundary = b3_Up_Boundary;
		b4_Boundary = b4_Up_Boundary;

		k1_Boundary = &(k1_Up_Boundary[thread_Index]);
		k2_Boundary = &(k2_Up_Boundary[thread_Index]);
		k3_Boundary = &(k3_Up_Boundary[thread_Index]);
		k4_Boundary = &(k4_Up_Boundary[thread_Index]);

		D1 = &(D1_Up[thread_Index]);
		D2 = &(D2_Up[thread_Index]);
		D3 = &(D3_Up[thread_Index]);
		D4 = &(D4_Up[thread_Index]);
	}

	if(multilayer->imperfections_Model.approximation == SA_approximation ||
	   multilayer->imperfections_Model.approximation == CSA_approximation)
	{
		for (int boundary_Index = 0; boundary_Index<num_Boundaries; boundary_Index++)
		{
			double sigma_j = unwrapped_Structure->sigma_Roughness_Threaded[thread_Index][boundary_Index];
			double sigma_j2_2 = sigma_j*sigma_j/2;
			complex<double> factor = SQRT_PI * (unwrapped_Structure->epsilon[boundary_Index+1]-unwrapped_Structure->epsilon[boundary_Index]);

			complex<double>& k1 = (*k1_Boundary)[boundary_Index];
			complex<double>& k2 = (*k2_Boundary)[boundary_Index];
			complex<double>& k3 = (*k3_Boundary)[boundary_Index];
			complex<double>& k4 = (*k4_Boundary)[boundary_Index];

			complex<double>& b1 = (*b1_Boundary)[boundary_Index];
			complex<double>& b2 = (*b2_Boundary)[boundary_Index];
			complex<double>& b3 = (*b3_Boundary)[boundary_Index];
			complex<double>& b4 = (*b4_Boundary)[boundary_Index];

			int n=1;
			{
				(*D1)[n-1] = factor * b1 * exp(-sigma_j2_2*k1*k1);
				(*D2)[n-1] = factor * b2 * exp(-sigma_j2_2*k2*k2);
				(*D3)[n-1] = factor * b3 * exp(-sigma_j2_2*k3*k3);
				(*D4)[n-1] = factor * b4 * exp(-sigma_j2_2*k4*k4);

				(*K_Factor_Boundary)[boundary_Index][n-1] = (*D1)[n-1] + (*D2)[n-1] + (*D3)[n-1] + (*D4)[n-1];
			}
			for(int n=2; n<=n_Max_Series; n++)
			{
				(*D1)[n-1] = (*D1)[n-2] * ( I*M_SQRT2*sigma_j*k1 );
				(*D2)[n-1] = (*D2)[n-2] * ( I*M_SQRT2*sigma_j*k2 );
				(*D3)[n-1] = (*D3)[n-2] * ( I*M_SQRT2*sigma_j*k3 );
				(*D4)[n-1] = (*D4)[n-2] * ( I*M_SQRT2*sigma_j*k4 );

				(*K_Factor_Boundary)[boundary_Index][n-1] = (*D1)[n-1] + (*D2)[n-1] + (*D3)[n-1] + (*D4)[n-1];
			}
		}
	}
	if(multilayer->imperfections_Model.approximation == DWBA_approximation)
	{
		for (int boundary_Index = 0; boundary_Index<num_Boundaries; boundary_Index++)
		{
			double sigma_j = unwrapped_Structure->sigma_Roughness_Threaded[thread_Index][boundary_Index];

			complex<double>& k1_Up = k1_Up_Boundary[thread_Index][boundary_Index];
			complex<double>& k2_Up = k2_Up_Boundary[thread_Index][boundary_Index];
			complex<double>& k3_Up = k3_Up_Boundary[thread_Index][boundary_Index];
			complex<double>& k4_Up = k4_Up_Boundary[thread_Index][boundary_Index];

			complex<double>& k1_Low = k1_Low_Boundary[thread_Index][boundary_Index];
			complex<double>& k2_Low = k2_Low_Boundary[thread_Index][boundary_Index];
			complex<double>& k3_Low = k3_Low_Boundary[thread_Index][boundary_Index];
			complex<double>& k4_Low = k4_Low_Boundary[thread_Index][boundary_Index];

			complex<double>& b1_Up = (*b1_Up_Boundary)[boundary_Index];
			complex<double>& b2_Up = (*b2_Up_Boundary)[boundary_Index];
			complex<double>& b3_Up = (*b3_Up_Boundary)[boundary_Index];
			complex<double>& b4_Up = (*b4_Up_Boundary)[boundary_Index];

			complex<double>& b1_Low = (*b1_Low_Boundary)[boundary_Index];
			complex<double>& b2_Low = (*b2_Low_Boundary)[boundary_Index];
			complex<double>& b3_Low = (*b3_Low_Boundary)[boundary_Index];
			complex<double>& b4_Low = (*b4_Low_Boundary)[boundary_Index];

			int n=1;
			{
				D1_Up [thread_Index][n-1] = SQRT_PI/2 * Faddeeva::erfcx( I*sigma_j*k1_Up/M_SQRT2 );
				D2_Up [thread_Index][n-1] = SQRT_PI/2 * Faddeeva::erfcx( I*sigma_j*k2_Up/M_SQRT2 );
				D3_Up [thread_Index][n-1] = SQRT_PI/2 * Faddeeva::erfcx( I*sigma_j*k3_Up/M_SQRT2 );
				D4_Up [thread_Index][n-1] = SQRT_PI/2 * Faddeeva::erfcx( I*sigma_j*k4_Up/M_SQRT2 );

				D1_Low[thread_Index][n-1] = SQRT_PI/2 * Faddeeva::erfcx(-I*sigma_j*k1_Low/M_SQRT2 );
				D2_Low[thread_Index][n-1] = SQRT_PI/2 * Faddeeva::erfcx(-I*sigma_j*k2_Low/M_SQRT2 );
				D3_Low[thread_Index][n-1] = SQRT_PI/2 * Faddeeva::erfcx(-I*sigma_j*k3_Low/M_SQRT2 );
				D4_Low[thread_Index][n-1] = SQRT_PI/2 * Faddeeva::erfcx(-I*sigma_j*k4_Low/M_SQRT2 );

				(*K_Factor_Boundary)[boundary_Index][n-1] = b1_Up *D1_Up [thread_Index][n-1] + b2_Up *D2_Up [thread_Index][n-1] + b3_Up *D3_Up [thread_Index][n-1] + b4_Up *D4_Up [thread_Index][n-1] +
															b1_Low*D1_Low[thread_Index][n-1] + b2_Low*D2_Low[thread_Index][n-1] + b3_Low*D3_Low[thread_Index][n-1] + b4_Low*D4_Low[thread_Index][n-1];
				(*K_Factor_Boundary)[boundary_Index][n-1] *= (unwrapped_Structure->epsilon[boundary_Index+1]-unwrapped_Structure->epsilon[boundary_Index]);
			}
			for(int n=2; n<=n_Max_Series; n++)
			{
				D1_Up [thread_Index][n-1] = D1_Up [thread_Index][n-2] * ( I*M_SQRT2*sigma_j*k1_Up  ) - hermites[n-2];
				D2_Up [thread_Index][n-1] = D2_Up [thread_Index][n-2] * ( I*M_SQRT2*sigma_j*k2_Up  ) - hermites[n-2];
				D3_Up [thread_Index][n-1] = D3_Up [thread_Index][n-2] * ( I*M_SQRT2*sigma_j*k3_Up  ) - hermites[n-2];
				D4_Up [thread_Index][n-1] = D4_Up [thread_Index][n-2] * ( I*M_SQRT2*sigma_j*k4_Up  ) - hermites[n-2];

				D1_Low[thread_Index][n-1] = D1_Low[thread_Index][n-2] * ( I*M_SQRT2*sigma_j*k1_Low ) + hermites[n-2];
				D2_Low[thread_Index][n-1] = D2_Low[thread_Index][n-2] * ( I*M_SQRT2*sigma_j*k2_Low ) + hermites[n-2];
				D3_Low[thread_Index][n-1] = D3_Low[thread_Index][n-2] * ( I*M_SQRT2*sigma_j*k3_Low ) + hermites[n-2];
				D4_Low[thread_Index][n-1] = D4_Low[thread_Index][n-2] * ( I*M_SQRT2*sigma_j*k4_Low ) + hermites[n-2];

				(*K_Factor_Boundary)[boundary_Index][n-1] = b1_Up *D1_Up [thread_Index][n-1] + b2_Up *D2_Up [thread_Index][n-1] + b3_Up *D3_Up [thread_Index][n-1] + b4_Up *D4_Up [thread_Index][n-1] +
															b1_Low*D1_Low[thread_Index][n-1] + b2_Low*D2_Low[thread_Index][n-1] + b3_Low*D3_Low[thread_Index][n-1] + b4_Low*D4_Low[thread_Index][n-1];
				(*K_Factor_Boundary)[boundary_Index][n-1] *= (unwrapped_Structure->epsilon[boundary_Index+1]-unwrapped_Structure->epsilon[boundary_Index]);
			}
		}
	}
}

double Unwrapped_Reflection::calc_K_Factor_Term_Sum_DWBA_SA_CSA(int thread_Index, QString polarization, int n_Power)
{
	vector<vector<complex<double>>>* K_Factor_Boundary;

	if(polarization == "s")	K_Factor_Boundary = &(K_Factor_Boundary_s[thread_Index]);
	if(polarization == "p")	K_Factor_Boundary = &(K_Factor_Boundary_p[thread_Index]);

	double incoherent_Diagonal_Sum = 0;
	complex<double> coherent_Diagonal_Sum = 0;
	for (int boundary_Index = 0; boundary_Index<num_Boundaries; boundary_Index++)
	{
		// incoherent diagonal sum
		incoherent_Diagonal_Sum += pow(abs((*K_Factor_Boundary)[boundary_Index][n_Power-1]),2);

		// coherent diagonal sum
		coherent_Diagonal_Sum += (*K_Factor_Boundary)[boundary_Index][n_Power-1];
	}

	if(multilayer->imperfections_Model.vertical_Correlation == full_Correlation) return pow(abs(coherent_Diagonal_Sum),2);
	if(multilayer->imperfections_Model.vertical_Correlation == zero_Correlation) return incoherent_Diagonal_Sum;
	return -2020;
}

void Unwrapped_Reflection::calc_Sliced_Field(int thread_Index, int point_Index, const vector<complex<double>>& epsilon_Vector)
{
	// s-polarization
	if( calc_Functions.check_Field ||
	    calc_Functions.check_Joule )
	{
		if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)  calc_Amplitudes_Field(thread_Index, point_Index, "s");
		if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE) calc_Amplitudes_Field(thread_Index, point_Index, "p");

		// field intensity
		double s_Weight = (1. + measurement.polarization) / 2.;
		double p_Weight = (1. - measurement.polarization) / 2.;

		complex<double> U_s, U_p, iChi, e_i, e_r;
		for(int z_Index=0; z_Index<unwrapped_Structure->num_Field_Slices; z_Index++)
		{
			double z = unwrapped_Structure->field_Z_Positions[z_Index];
			int layer_Index = unwrapped_Structure->get_Layer_or_Slice_Index(z);
			int media_Index = layer_Index+1;

			iChi = I*hi[point_Index][media_Index];
			e_i = exp(+iChi*(z-boundaries_Enlarged[media_Index]));
			e_r = exp(-iChi*(z-boundaries_Enlarged[media_Index]));

			double field_Value = 0;
			// s-polarization
			if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
			{
				U_s = U_i_s[thread_Index][media_Index] * e_i + U_r_s[thread_Index][media_Index] * e_r;
				field_Value+=s_Weight*pow(abs(U_s),2);
			}
			// p-polarization
			if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
			{
				U_p = U_i_p[thread_Index][media_Index] * e_i + U_r_p[thread_Index][media_Index] * e_r;
				field_Value+=p_Weight*pow(abs(U_p),2);
			}

			calculated_Values.field_Intensity[point_Index][z_Index] = field_Value;
			calculated_Values.absorption_Map [point_Index][z_Index] = field_Value * imag(epsilon_Vector[media_Index]); // consider sigma?
//			Kossel[point_Index] += field_Value*unwrapped_Structure->calc_Functions.field_Step;
		}
	}
}

double Unwrapped_Reflection::calc_Field_Term_Sum(QString polarization, int point_Index, int thread_Index)
{
	vector<complex<double>>* q_Boundary_Field;
	vector<complex<double>>* q0_Boundary_Field;
	vector<vector<double>>* intensity_Term_Boundary;
	vector<vector<complex<double>>>* field_Term_Boundary;
	vector<vector<vector<double>>>* half_Sum_Field_Term;

	if(polarization == "s")
	{
		if( measurement.measurement_Type == measurement_Types[GISAS_Map] ||
		    measurement.measurement_Type == measurement_Types[Detector_Scan] )	{ q0_Boundary_Field = &(calculated_Values.q0_Boundary_Field_s.front()); }

		if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		    measurement.measurement_Type == measurement_Types[Offset_Scan] )	{ q0_Boundary_Field = &(calculated_Values.q0_Boundary_Field_s[point_Index]); }

		q_Boundary_Field = &(calculated_Values.q_Boundary_Field_s[point_Index]);
		intensity_Term_Boundary = &(intensity_Term_Boundary_s);
		field_Term_Boundary = &(field_Term_Boundary_s);
		half_Sum_Field_Term = &(half_Sum_Field_Term_s);
	}
	if(polarization == "p")
	{
		if( measurement.measurement_Type == measurement_Types[GISAS_Map] ||
		    measurement.measurement_Type == measurement_Types[Detector_Scan] )	{ q0_Boundary_Field = &(calculated_Values.q0_Boundary_Field_p.front());	}

		if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		    measurement.measurement_Type == measurement_Types[Offset_Scan] )	{ q0_Boundary_Field = &(calculated_Values.q0_Boundary_Field_p[point_Index]); }

		q_Boundary_Field = &(calculated_Values.q_Boundary_Field_p[point_Index]);
		intensity_Term_Boundary = &(intensity_Term_Boundary_p);
		field_Term_Boundary = &(field_Term_Boundary_p);
		half_Sum_Field_Term = &(half_Sum_Field_Term_p);
	}

	/// ------------------------------------------------------
	/// common
	/// ------------------------------------------------------
	double incoherent_Diagonal_Sum = 0;
	complex<double> coherent_Diagonal_Sum = 0;
	for (int j = 0; j<num_Boundaries; j++)
	{
		// all fields
		(*field_Term_Boundary)[thread_Index][j] = (*q_Boundary_Field)[j] *
		                                          (*q0_Boundary_Field)[j]*
		                                          (unwrapped_Structure->epsilon[j+1]-unwrapped_Structure->epsilon[j]);

		// diagonal intensities
		(*intensity_Term_Boundary)[thread_Index][j] = pow(abs((*field_Term_Boundary)[thread_Index][j]),2);

		// incoherent diagonal sum
		incoherent_Diagonal_Sum += (*intensity_Term_Boundary)[thread_Index][j];

		// coherent diagonal sum
		coherent_Diagonal_Sum += (*field_Term_Boundary)[thread_Index][j];
	}

	if(multilayer->imperfections_Model.vertical_Correlation == full_Correlation) return pow(abs(coherent_Diagonal_Sum),2);
	if(multilayer->imperfections_Model.vertical_Correlation == zero_Correlation) return incoherent_Diagonal_Sum;


	if(multilayer->imperfections_Model.vertical_Correlation == partial_Correlation)
	{
		// start from substrate, i>j ( i lies deeper than j )
		for (int i = num_Boundaries-1; i>=1; i--)
		{
			for (int j = i-1; j>=0; j--)
			{
				(*half_Sum_Field_Term)[thread_Index][i][j] = 2*real((*field_Term_Boundary)[thread_Index][i]*
				                                               conj((*field_Term_Boundary)[thread_Index][j]));
			}
		}
		if(multilayer->imperfections_Model.common_Model == ABC_model)			 return incoherent_Diagonal_Sum;
	}
	return -2020;
}

void Unwrapped_Reflection::calc_Environmental_Factor(int thread_Index, int point_Index)
{
	environment_Factor_s[thread_Index] = real( hi[point_Index].back() /hi[point_Index].front());
	environment_Factor_p[thread_Index] = real((hi[point_Index].back() /epsilon_Substrate[thread_Index] )/
	                                          (hi[point_Index].front()/epsilon_Ambient  [thread_Index]));

	if(isinf(environment_Factor_s[thread_Index])) {environment_Factor_s[thread_Index] = 0;}
	if(isinf(environment_Factor_p[thread_Index])) {environment_Factor_p[thread_Index] = 0;}
	if(isnan(environment_Factor_s[thread_Index])) {environment_Factor_s[thread_Index] = 0;}
	if(isnan(environment_Factor_p[thread_Index])) {environment_Factor_p[thread_Index] = 0;}
}

void Unwrapped_Reflection::choose_PSD_1D_Function(const Data& struct_Data, int thread_Index)
{
	if(multilayer->imperfections_Model.common_Model == ABC_model)
	{
		PSD_1D_Func_Vec[thread_Index] = Global_Variables::PSD_ABC_1D;
	} else
	if(multilayer->imperfections_Model.common_Model == fractal_Gauss_Model)
	{
		if(struct_Data.roughness_Model.fractal_alpha.value<1)
		{
			if(abs(struct_Data.roughness_Model.fractal_alpha.value - 0.5) < DBL_EPSILON)
			{
				PSD_1D_Func_Vec[thread_Index] = Global_Variables::PSD_ABC_1D;
			} else
			{
				PSD_1D_Func_Vec[thread_Index] = Global_Variables::PSD_Fractal_Gauss_1D;
			}
		} else
		{
			PSD_1D_Func_Vec[thread_Index] = Global_Variables::PSD_Real_Gauss_1D;
		}
	}
}

void Unwrapped_Reflection::choose_PSD_2D_Function(int point_Index, int thread_Index)
{
	if(multilayer->imperfections_Model.common_Model == ABC_model)
	{
		PSD_2D_Func_Vec[thread_Index] = Global_Variables::PSD_ABC_2D;
	} else
	if(multilayer->imperfections_Model.common_Model == fractal_Gauss_Model)
	{
		if(point_Index == 0) qInfo() << "Unwrapped_Reflection::choose_PSD_2D_Function  :  fractal_Gauss_Model can't be used in 2D" << endl;
	}
}

void Unwrapped_Reflection::multifly_Fresnel_And_Weak_Factor(int thread_Index)
{
	/// used only in if Fresnel was taken from Node
	// s-polarization
	if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
	{
		for (int i = 0; i < num_Boundaries; ++i)
		{
			// reflectance
			r_Fresnel_s[thread_Index][i] *= weak_Factor_R[thread_Index][i];

			// transmittance
			t_Fresnel_s[thread_Index][i] *= weak_Factor_T[thread_Index][i];
		}
	}

	// p-polarization
	if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
	{
		for (int i = 0; i < num_Boundaries; ++i)
		{
			// reflectance
			r_Fresnel_p[thread_Index][i] *= weak_Factor_R[thread_Index][i];

			// transmittance
			t_Fresnel_p[thread_Index][i] *= weak_Factor_T[thread_Index][i];
		}
	}
}

void Unwrapped_Reflection::calc_Specular_1_Point_1_Thread(int thread_Index, int point_Index)
{
//	auto start = std::chrono::system_clock::now();
	if(!unwrapped_Structure->discretization_Parameters.enable_Discretization)
	{
		if( max_Depth <= depth_Threshold)
		{
			// in case of grading, some of these values are temporary and will be recalculated
			if( (measurement.polarization - 1) > -POLARIZATION_TOLERANCE )		// s-polarization only
			{
				fill_s__Max_Depth_3(unwrapped_Structure->calc_Tree.begin(), thread_Index, point_Index);
			} else
			if( (measurement.polarization + 1) < POLARIZATION_TOLERANCE )		// p-polarization only
			{
				fill_p__Max_Depth_3(unwrapped_Structure->calc_Tree.begin(), thread_Index, point_Index);
			} else																// both polarizations
			{
				fill_sp_Max_Depth_3(unwrapped_Structure->calc_Tree.begin(), thread_Index, point_Index);
			}

			// if we have some grading
			if( depth_Grading )
			{
				calc_Exponenta(thread_Index, point_Index,unwrapped_Structure->thickness);
			}
			if( sigma_Grading && measurement.measurement_Type == measurement_Types[Specular_Scan] )
			{
				calc_Weak_Factor(thread_Index, point_Index);
				multifly_Fresnel_And_Weak_Factor(thread_Index);
			}
		} else
		{
			if( measurement.argument_Type  == argument_Types[Wavelength_Energy] )
			{
				fill_Epsilon_Ambient_Substrate(thread_Index, unwrapped_Structure->epsilon_Dependent[point_Index]);
				calc_Hi(point_Index, measurement.k_Vec[point_Index], measurement.beam_Theta_0_Cos2_Value, unwrapped_Structure->epsilon_Dependent[point_Index]);
				calc_Weak_Factor(thread_Index, point_Index);
				calc_Exponenta(thread_Index, point_Index, unwrapped_Structure->thickness);
				calc_Fresnel(thread_Index, point_Index, unwrapped_Structure->epsilon_Dependent[point_Index]);
			} else
			// for all angular simulations, including scattering
			{
				fill_Epsilon_Ambient_Substrate(thread_Index, unwrapped_Structure->epsilon);
				calc_Hi(thread_Index, measurement.k_Value, measurement.beam_Theta_0_Cos2_Vec[point_Index],unwrapped_Structure->epsilon);
				if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
				{ calc_Weak_Factor(thread_Index, point_Index); }
				calc_Exponenta(thread_Index, point_Index,unwrapped_Structure->thickness);
				calc_Fresnel(thread_Index, point_Index, unwrapped_Structure->epsilon);
			}
		}
	} else
	// discretization
	{
		// TODO scattering
		if(measurement.measurement_Type != measurement_Types[Specular_Scan])
		{
			qInfo() <<  "Unwrapped_Reflection::calc_Specular_1_Point_1_Thread  :  discretization is not done" << endl;
		}

		// weak factors are "not used" with discretization. only formally
		for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
		{
			weak_Factor_R[thread_Index].assign(num_Boundaries,1);
			weak_Factor_T[thread_Index].assign(num_Boundaries,1);
		}

		if( measurement.measurement_Type == measurement_Types[Specular_Scan] &&
		    measurement.argument_Type  == argument_Types[Wavelength_Energy] )
		{
			fill_Epsilon_Ambient_Substrate(thread_Index, unwrapped_Structure->epsilon_Dependent[point_Index]); // here can be passed non-discretized epsilon
			calc_Hi(thread_Index, measurement.k_Vec[point_Index], measurement.beam_Theta_0_Cos2_Value, unwrapped_Structure->discretized_Epsilon_Dependent[point_Index]);
			calc_Exponenta(thread_Index, point_Index,unwrapped_Structure->discretized_Thickness);
			calc_Fresnel(thread_Index, point_Index, unwrapped_Structure->discretized_Epsilon_Dependent[point_Index]);
		}
		if( measurement.measurement_Type == measurement_Types[Specular_Scan] &&
		    measurement.argument_Type  == argument_Types[Beam_Grazing_Angle] )
		{
			fill_Epsilon_Ambient_Substrate(thread_Index, unwrapped_Structure->epsilon); // here can be passed non-discretized epsilon
			calc_Hi(thread_Index, measurement.k_Value, measurement.beam_Theta_0_Cos2_Vec[point_Index], unwrapped_Structure->discretized_Epsilon);
			calc_Exponenta(thread_Index, point_Index,unwrapped_Structure->discretized_Thickness);
			calc_Fresnel(thread_Index, point_Index, unwrapped_Structure->discretized_Epsilon);
		}
	}

//	auto end = std::chrono::system_clock::now();

	calc_Local(thread_Index, point_Index);

//	auto enD = std::chrono::system_clock::now();
//	auto elapseD = std::chrono::duration_cast<std::chrono::nanoseconds>(enD - start);
//	if(point_Index==0) qInfo() << endl << measurement.measurement_Type << spec_Scat_mode << elapseD.count()/1000000. << " seconds" << endl;


	// if we need to calculate detailed field distribution
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type  == argument_Types[Beam_Grazing_Angle] )
		{
			if(!unwrapped_Structure->discretization_Parameters.enable_Discretization)	{
				calc_Sliced_Field(thread_Index, point_Index, unwrapped_Structure->epsilon);
			} else	{
				calc_Sliced_Field(thread_Index, point_Index, unwrapped_Structure->discretized_Epsilon);
			}
		}
		if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			if(!unwrapped_Structure->discretization_Parameters.enable_Discretization)	{
				calc_Sliced_Field(thread_Index, point_Index, unwrapped_Structure->epsilon_Dependent[point_Index]);
			} else	{
				calc_Sliced_Field(thread_Index, point_Index, unwrapped_Structure->discretized_Epsilon_Dependent[point_Index]);
			}
		}
	}

	/// SCATTERING
	if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
	    measurement.measurement_Type == measurement_Types[GISAS_Map]     ||
	    measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
	    measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		if(!unwrapped_Structure->discretization_Parameters.enable_Discretization)
		{
			if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)  calc_Amplitudes_Field(thread_Index, point_Index, "s");
			if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE) calc_Amplitudes_Field(thread_Index, point_Index, "p");

			// in specular mode we stop here
			// in scattered mode we go further and use calculated q and q0 fields
			if(spec_Scat_mode == SCATTERED_MODE)
			{
				double sin_Theta_0 = max(measurement.beam_Theta_0_Sin_Value, DBL_EPSILON);
				double cos_Theta_0 = max(measurement.beam_Theta_0_Cos_Value, DBL_EPSILON);
				double e_Factor_PT_1D		   = pow(measurement.k_Value,3)/(16*M_PI     *sin_Theta_0*sqrt(cos_Theta_0*measurement.detector_Theta_Cos_Vec[point_Index]));
				double e_Factor_PT_2D		   = pow(measurement.k_Value,4)/(16*M_PI*M_PI*sin_Theta_0);
				double e_Factor_DWBA_SA_CSA_1D = pow(measurement.k_Value,3)/(2 *M_PI*M_PI*sin_Theta_0*sqrt(cos_Theta_0*measurement.detector_Theta_Cos_Vec[point_Index]));

				if( measurement.measurement_Type == measurement_Types[Offset_Scan] ||
				    measurement.measurement_Type == measurement_Types[Rocking_Curve] )
				{
					sin_Theta_0 = max(measurement.beam_Theta_0_Sin_Vec[point_Index], DBL_EPSILON);
					cos_Theta_0 = max(measurement.beam_Theta_0_Cos_Vec[point_Index], DBL_EPSILON);
				}

				if(multilayer->imperfections_Model.approximation == PT_approximation)
				{
					if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
					    measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
					    measurement.measurement_Type == measurement_Types[Offset_Scan] )
					{
						if( multilayer->imperfections_Model.vertical_Correlation == full_Correlation ||
						    multilayer->imperfections_Model.vertical_Correlation == zero_Correlation )
						{
							if(multilayer->imperfections_Model.use_Common_Roughness_Function)
							{
								choose_PSD_1D_Function(substrate, thread_Index);
								double PSD_Factor = PSD_1D_Func_Vec[thread_Index](substrate.PSD_ABC_1D_Factor,
								                                                 substrate.roughness_Model.cor_radius.value,
								                                                 substrate.roughness_Model.fractal_alpha.value,
								                                                 measurement.k_Value,
								                                                 measurement.detector_Theta_Cos_Vec[point_Index],
								                                                 cos_Theta_0,
								                                                 substrate_Node->spline,
								                                                 substrate_Node->acc);
								PSD_Factor = max(PSD_Factor,0.);

								// s-polarization
								if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
								{
									double field_Term_Sum = calc_Field_Term_Sum("s", point_Index, thread_Index);
									calculated_Values.S_s[point_Index] = e_Factor_PT_1D * field_Term_Sum * PSD_Factor;
								}
								// p-polarization
								if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
								{
									double field_Term_Sum = calc_Field_Term_Sum("p", point_Index, thread_Index);
									calculated_Values.S_p[point_Index] = e_Factor_PT_1D * field_Term_Sum * PSD_Factor;
								}
							} else
							/// individual PSD of items
							{
								fill_Item_PSD_1D(thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0);

								// s-polarization
								if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									double field_Term_Sum_PSD = 0;
									for (int j = 0; j<num_Boundaries; j++)
									{
										int item_Index = boundary_Item_Vec[j];
										double PSD_Factor = PSD_Factor_Item[thread_Index][item_Index];
										field_Term_Sum_PSD += intensity_Term_Boundary_s[thread_Index][j] * PSD_Factor;
									}
									calculated_Values.S_s[point_Index] = e_Factor_PT_1D * field_Term_Sum_PSD;
								}
								// p-polarization
								if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
								{
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									double field_Term_Sum_PSD = 0;
									for (int j = 0; j<num_Boundaries; j++)
									{
										int item_Index = boundary_Item_Vec[j];
										double PSD_Factor = PSD_Factor_Item[thread_Index][item_Index];
										field_Term_Sum_PSD += intensity_Term_Boundary_p[thread_Index][j] * PSD_Factor;
									}
									calculated_Values.S_p[point_Index] = e_Factor_PT_1D * field_Term_Sum_PSD;
								}
							}
						}
						if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation )
						{
							if(multilayer->imperfections_Model.common_Model == ABC_model)
							{
								// pure s-polarization
								if( (measurement.polarization - 1) > -POLARIZATION_TOLERANCE)
								{
									double incoherent_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, incoherent_Sum_s, 0, true };
									gsl_function F = { &function_Scattering_ABC_2D_s, &params };

									calculated_Values.S_s[point_Index] = e_Factor_PT_2D * azimuthal_Integration(&F, abs(measurement.detector_Theta_Cos_Vec[point_Index]-cos_Theta_0));
								} else
								// pure p-polarization
								if( (measurement.polarization + 1) < POLARIZATION_TOLERANCE)
								{
									double incoherent_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, 0, incoherent_Sum_p, true };
									gsl_function F = { &function_Scattering_ABC_2D_p, &params };

									calculated_Values.S_p[point_Index] = e_Factor_PT_2D * azimuthal_Integration(&F, abs(measurement.detector_Theta_Cos_Vec[point_Index]-cos_Theta_0));
								} else
								// mixed sp-polarization
								{
									double incoherent_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									double incoherent_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, incoherent_Sum_s, incoherent_Sum_p, true };
									gsl_function F = { &function_Scattering_ABC_2D_sp, &params };

									// crutch: s and p only on mixed state. no different s and p calculations
									calculated_Values.S_s[point_Index] = e_Factor_PT_2D * azimuthal_Integration(&F, abs(measurement.detector_Theta_Cos_Vec[point_Index]-cos_Theta_0));
									calculated_Values.S_p[point_Index] = calculated_Values.S_s[point_Index];
								}
							} else
							if(multilayer->imperfections_Model.common_Model == linear_Growth_and_ABC_Model)
							{
								// pure s-polarization
								if( (measurement.polarization - 1) > -POLARIZATION_TOLERANCE)
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, 0, 0, true };
									gsl_function F = { &function_Scattering_Linear_2D_s, &params };

									calculated_Values.S_s[point_Index] = e_Factor_PT_2D * azimuthal_Integration(&F, abs(measurement.detector_Theta_Cos_Vec[point_Index]-cos_Theta_0));
								} else
								// pure p-polarization
								if( (measurement.polarization + 1) < POLARIZATION_TOLERANCE)
								{
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, 0, 0, true };
									gsl_function F = { &function_Scattering_Linear_2D_p, &params };

									calculated_Values.S_p[point_Index] = e_Factor_PT_2D * azimuthal_Integration(&F, abs(measurement.detector_Theta_Cos_Vec[point_Index]-cos_Theta_0));
								} else
								// mixed sp-polarization
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, 0, 0, true };
									gsl_function F = { &function_Scattering_Linear_2D_sp, &params };

									// crutch: s and p only on mixed state. no different s and p calculations
									calculated_Values.S_s[point_Index] = e_Factor_PT_2D * azimuthal_Integration(&F, abs(measurement.detector_Theta_Cos_Vec[point_Index]-cos_Theta_0));
									calculated_Values.S_p[point_Index] = calculated_Values.S_s[point_Index];
								}
							}
						}
					}
					if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
					{
						choose_PSD_2D_Function(point_Index, thread_Index);

						if( multilayer->imperfections_Model.vertical_Correlation == full_Correlation ||
						    multilayer->imperfections_Model.vertical_Correlation == zero_Correlation )
						{
							if( multilayer->imperfections_Model.use_Common_Roughness_Function )
							{
								double PSD_2D_Factor;

								// pure s-polarization
								if( (measurement.polarization - 1) > -POLARIZATION_TOLERANCE)
								{
									double field_Term_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										PSD_2D_Factor = PSD_2D_Func_Vec[thread_Index](substrate.PSD_ABC_2D_Factor,
										                                              substrate.roughness_Model.cor_radius.value,
										                                              substrate.roughness_Model.fractal_alpha.value,
										                                              measurement.k_Value,
										                                              measurement.detector_Theta_Cos_Vec[point_Index],
										                                              measurement.beam_Theta_0_Cos_Value,
										                                              measurement.detector_Phi_Cos_Vec[phi_Index]);

										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * field_Term_Sum_s * PSD_2D_Factor;
									}
								} else
								// pure p-polarization
								if( (measurement.polarization + 1) < POLARIZATION_TOLERANCE)
								{
									double field_Term_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										PSD_2D_Factor = PSD_2D_Func_Vec[thread_Index](substrate.PSD_ABC_2D_Factor,
										                                              substrate.roughness_Model.cor_radius.value,
										                                              substrate.roughness_Model.fractal_alpha.value,
										                                              measurement.k_Value,
										                                              measurement.detector_Theta_Cos_Vec[point_Index],
										                                              measurement.beam_Theta_0_Cos_Value,
										                                              measurement.detector_Phi_Cos_Vec[phi_Index]);

										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * field_Term_Sum_p * PSD_2D_Factor;
									}
								} else
								// mixed sp-polarization
								{
									double field_Term_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									double field_Term_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										PSD_2D_Factor = PSD_2D_Func_Vec[thread_Index](substrate.PSD_ABC_2D_Factor,
										                                              substrate.roughness_Model.cor_radius.value,
										                                              substrate.roughness_Model.fractal_alpha.value,
										                                              measurement.k_Value,
										                                              measurement.detector_Theta_Cos_Vec[point_Index],
										                                              measurement.beam_Theta_0_Cos_Value,
										                                              measurement.detector_Phi_Cos_Vec[phi_Index]);

										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = (s_Weight*field_Term_Sum_s + p_Weight*field_Term_Sum_p)*e_Factor_PT_2D*PSD_2D_Factor;
									}
								}
							} else
							/// individual PSD of items
							{
								// pure s-polarization
								if( (measurement.polarization - 1) > -POLARIZATION_TOLERANCE)
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										fill_Item_PSD_2D(thread_Index, point_Index, phi_Index);

										double field_With_PSD_2D_Factor = 0;
										for (int j = 0; j<num_Boundaries; j++)
										{
											int item_Index = boundary_Item_Vec[j];
											double PSD_2D_Factor = PSD_Factor_Item[thread_Index][item_Index];

											field_With_PSD_2D_Factor += intensity_Term_Boundary_s[thread_Index][j] * PSD_2D_Factor;
										}
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * field_With_PSD_2D_Factor;
									}
								} else
								// pure p-polarization
								if( (measurement.polarization + 1) < POLARIZATION_TOLERANCE)
								{
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										fill_Item_PSD_2D(thread_Index, point_Index, phi_Index);

										double field_With_PSD_2D_Factor = 0;
										for (int j = 0; j<num_Boundaries; j++)
										{
											int item_Index = boundary_Item_Vec[j];
											double PSD_2D_Factor = PSD_Factor_Item[thread_Index][item_Index];

											field_With_PSD_2D_Factor += intensity_Term_Boundary_p[thread_Index][j] * PSD_2D_Factor;
										}
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * field_With_PSD_2D_Factor;
									}
								} else
								// mixed sp-polarization
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										fill_Item_PSD_2D(thread_Index, point_Index, phi_Index);

										double field_With_PSD_2D_Factor_s = 0;
										double field_With_PSD_2D_Factor_p = 0;
										for (int j = 0; j<num_Boundaries; j++)
										{
											int item_Index = boundary_Item_Vec[j];
											double PSD_2D_Factor = PSD_Factor_Item[thread_Index][item_Index];

											field_With_PSD_2D_Factor_s += intensity_Term_Boundary_p[thread_Index][j] * PSD_2D_Factor;
											field_With_PSD_2D_Factor_p += intensity_Term_Boundary_p[thread_Index][j] * PSD_2D_Factor;
										}
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = (s_Weight*field_With_PSD_2D_Factor_s + p_Weight*field_With_PSD_2D_Factor_p)*e_Factor_PT_2D;
									}
								}
							}
						}
						if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation )
						{
							if(multilayer->imperfections_Model.common_Model == ABC_model)
							{
								// pure s-polarization
								if( (measurement.polarization - 1) > -POLARIZATION_TOLERANCE)
								{
									double incoherent_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, incoherent_Sum_s, 0, false };
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_ABC_2D_s(measurement.detector_Phi_Cos_Vec[phi_Index], &params);
									}
								} else
								// pure p-polarization
								if( (measurement.polarization + 1) < POLARIZATION_TOLERANCE)
								{
									double incoherent_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, 0, incoherent_Sum_p, false };
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_ABC_2D_p(measurement.detector_Phi_Cos_Vec[phi_Index], &params);
									}
								} else
								// mixed sp-polarization
								{
									double incoherent_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									double incoherent_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, incoherent_Sum_s, incoherent_Sum_p, false };
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_ABC_2D_sp(measurement.detector_Phi_Cos_Vec[phi_Index], &params);
									}
								}
							} else
							if(multilayer->imperfections_Model.common_Model == linear_Growth_and_ABC_Model)
							{
								// pure s-polarization
								if( (measurement.polarization - 1) > -POLARIZATION_TOLERANCE)
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, 0, 0, false };
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Linear_2D_s(measurement.detector_Phi_Cos_Vec[phi_Index], &params);
									}
								} else
								// pure p-polarization
								if( (measurement.polarization + 1) < POLARIZATION_TOLERANCE)
								{
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, 0, 0, false };
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Linear_2D_p(measurement.detector_Phi_Cos_Vec[phi_Index], &params);
									}
								} else
								// mixed sp-polarization
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, 0, 0, false };
										calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Linear_2D_sp(measurement.detector_Phi_Cos_Vec[phi_Index], &params);
									}
								}
							}
						}
					}
				}

				if( multilayer->imperfections_Model.approximation == DWBA_approximation ||
				    multilayer->imperfections_Model.approximation == SA_approximation   ||
				    multilayer->imperfections_Model.approximation == CSA_approximation  )
				{
					calc_k_Wavenumber_DWBA_SA_CSA(thread_Index, point_Index);
					if( (measurement.polarization + 1) >  POLARIZATION_TOLERANCE)
					{
						calc_Field_DWBA_SA_CSA(thread_Index, point_Index, "s");
						calc_K_Factor_DWBA_SA_CSA(thread_Index,           "s");
					}
					if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
					{
						calc_Field_DWBA_SA_CSA(thread_Index, point_Index, "p");
						calc_K_Factor_DWBA_SA_CSA(thread_Index,           "p");
					}

					if( multilayer->imperfections_Model.vertical_Correlation == full_Correlation ||
					    multilayer->imperfections_Model.vertical_Correlation == zero_Correlation )
					{
						if( multilayer->imperfections_Model.use_Common_Roughness_Function )
						{
							// s-polarization
							if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
							{
								double denominator = 1;
								for(int n_Power=1; n_Power<=n_Max_Series; n_Power++)
								{
									denominator = denominator * 2*n_Power;

//									double fourier_Factor = Global_Variables::splined_Value_1D(measurement.k_Value,
//																							   measurement.detector_Theta_Cos_Vec[point_Index],
//																							   cos_Theta_0,
//																							   substrate_Node->spline_n_Vec[n_Power-1],
//																							   substrate_Node->acc_n_Vec[n_Power-1]);
//									fourier_Factor = max(fourier_Factor,0.);
									double K_Factor_Term_Sum = calc_K_Factor_Term_Sum_DWBA_SA_CSA(thread_Index, "s", n_Power);
//									calculated_Values.S_s[point_Index] += e_Factor_DWBA_SA_CSA_1D * K_Factor_Term_Sum * fourier_Factor / denominator;

									pre_Fourier_Factor[thread_Index][n_Power-1] = K_Factor_Term_Sum / denominator;
								}

								double p = measurement.k_Value*abs(cos_Theta_0-measurement.detector_Theta_Cos_Vec[point_Index]);
								Params_DWBA_SA_CSA params { this, thread_Index, &substrate, p };
								gsl_function F = { &function_DWBA_SA_CSA_Batch_Integrand, &params };

								double result = 0, error;
								double abs_Err = 1e-4;
								double rel_Err = 1e-4;
								double current_Point = 0, sum_Result = 0;

								double alpha = substrate.roughness_Model.fractal_alpha.value;
								double sigma = substrate.roughness_Model.sigma.value;
								double xi = substrate.roughness_Model.cor_radius.value;


								if(p>1e-6 || multilayer->imperfections_Model.common_Model == ABC_model)
								{
									double interval = 0.5*xi;
									gsl_integration_workspace* w = gsl_integration_workspace_alloc(500);
									gsl_integration_workspace* wc = gsl_integration_workspace_alloc(500);

									gsl_integration_qawo_table* wf = gsl_integration_qawo_table_alloc(p, interval, GSL_INTEG_COSINE, size_t(10+n_Max_Series/3));

									if(multilayer->imperfections_Model.common_Model == fractal_Gauss_Model)
									{
										for(int i=0; i<50; i++)
										{
											gsl_integration_qawo(&F, current_Point, abs_Err, rel_Err, w->limit, w, wf, &result, &error); sum_Result += result; current_Point += interval;
										}
									}
									gsl_integration_qawf(&F, current_Point,         1e-1, w->limit, w, wc, wf, &result, &error); sum_Result += result; current_Point += interval;

									gsl_integration_qawo_table_free(wf);
									gsl_integration_workspace_free(wc);
									gsl_integration_workspace_free(w);
								} else
								{
									for(int n_Power=1; n_Power<=n_Max_Series; n_Power++)
									{
										sum_Result += sigma*sigma*pre_Fourier_Factor[thread_Index][n_Power-1] * xi*tgamma(1.+1/(2*alpha))/pow(n_Power,1/(2*alpha));
									}
								}

								calculated_Values.S_s[point_Index] = e_Factor_DWBA_SA_CSA_1D * sum_Result;
							}
							// p-polarization
							if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
							{

							}
						} else
						/// individual PSD of items
						{

						}
					}
				}
			}
		}
	}
}

double Unwrapped_Reflection::azimuthal_Integration(gsl_function* function, double delta)
{
	// TODO find more fast method

	double phi_Min = 0, phi_Max = 90;

	// faster, but less accurate
//	double phi_Min = 0, phi_Inter = 0.05, phi_Inter_2 = 1, phi_Inter_3 = 5, phi_Max = 20;
//	gsl_integration_glfixed_table* t = gsl_integration_glfixed_table_alloc(3);
//	double result;
//	double temp = gsl_integration_glfixed(function, phi_Min, phi_Inter, t);
//	temp   = temp+gsl_integration_glfixed(function, phi_Inter, phi_Inter_2, t);
//	temp   = temp+gsl_integration_glfixed(function, phi_Inter_2, phi_Inter_3, t);
//	result = temp+gsl_integration_glfixed(function, phi_Inter_3, phi_Max, t);
//	gsl_integration_glfixed_table_free(t);

	/// ----------------------------------------------------------------------------------------------------------------------

	// mixed
	double abserr, temp, result, epsabs = 1e0, epsrel = 1e0;
	gsl_integration_glfixed_table* t = gsl_integration_glfixed_table_alloc(3);
	size_t neval;
	if(delta<1e-4)
	{
		double phi_Inter_1 = 0.01, phi_Inter_2 = 0.05, phi_Inter_3 = 0.2, phi_Inter_4 = 20;
		gsl_integration_qng(function, phi_Min,     phi_Inter_1, epsabs, epsrel, &result, &abserr, &neval); temp=result;
		gsl_integration_qng(function, phi_Inter_1, phi_Inter_2, epsabs, epsrel, &result, &abserr, &neval); temp+=result;
		gsl_integration_qng(function, phi_Inter_2, phi_Inter_3, epsabs, epsrel, &result, &abserr, &neval); temp+=result;
		gsl_integration_qng(function, phi_Inter_3, phi_Inter_4, epsabs, epsrel, &result, &abserr, &neval); temp+=result;

		result = temp + gsl_integration_glfixed(function, phi_Inter_4, phi_Max, t);
	} else
	if(delta<5e-4)
	{
		double phi_Inter_1 = 0.5, phi_Inter_2 = 20;
		gsl_integration_qng(function, phi_Min,     phi_Inter_1, epsabs, epsrel, &result, &abserr, &neval); temp=result;
		gsl_integration_qng(function, phi_Inter_1, phi_Inter_2, epsabs, epsrel, &result, &abserr, &neval); temp+=result;

		result = temp + gsl_integration_glfixed(function, phi_Inter_2, phi_Max, t);
	} else
	{
		double phi_Inter_1 = 4, phi_Inter_2 = 20;
		gsl_integration_qng(function, phi_Min, phi_Inter_1, epsabs, epsrel, &result, &abserr, &neval); temp=result;

		temp   = temp + gsl_integration_glfixed(function, phi_Inter_1, phi_Inter_2, t);
		result = temp + gsl_integration_glfixed(function, phi_Inter_2, phi_Max,     t);
	}
	gsl_integration_glfixed_table_free(t);

	/// ----------------------------------------------------------------------------------------------------------------------

	// the slowest
//	gsl_integration_workspace* w = gsl_integration_workspace_alloc(100);
//	gsl_integration_qag(function, phi_Min, phi_Max, epsabs, epsrel, w->limit, GSL_INTEG_GAUSS61, w, &result, &abserr);
//	gsl_integration_workspace_free(w);

	return 2*result*(M_PI/180);
}

void Unwrapped_Reflection::fill_Specular_Values(int thread_Index, int point_Index)
{
	// reflectance
	if(	unwrapped_Structure->calc_Functions.check_Reflectance ||
	    unwrapped_Structure->calc_Functions.check_Absorptance )
	{
		complex<double> r_s = r_Local_s[thread_Index][0];
		complex<double> r_p = r_Local_p[thread_Index][0];

		calculated_Values.Phi_R_s[point_Index] = arg(r_s)/M_PI*180.;
		calculated_Values.Phi_R_p[point_Index] = arg(r_p)/M_PI*180.;
		calculated_Values.R_s	 [point_Index] = pow(abs(r_s),2);
		calculated_Values.R_p	 [point_Index] = pow(abs(r_p),2);
		calculated_Values.R		 [point_Index] = s_Weight * calculated_Values.R_s[point_Index] + p_Weight * calculated_Values.R_p[point_Index];
		calculated_Values.R_Instrumental[point_Index] = calculated_Values.R[point_Index];

		// NaN
		if(isnan(calculated_Values.R[point_Index]))
		{
			calculated_Values.R[point_Index]=10000;  // NaN to 10000. Be careful!
			qInfo() << "Unwrapped_Reflection::fill_Specular_Values  :  R = NaN at point" << point_Index << endl;
			QMessageBox::warning(nullptr, "Unwrapped_Reflection::fill_Specular_Values", "R = NaN");

			qInfo() << "r_Local_s_RE" << real(r_Local_s[thread_Index][0]) << "r_Local_s_IM" << imag(r_Local_s[thread_Index][0]) << endl;
			qInfo() << "r_Local_p_RE" << real(r_Local_p[thread_Index][0]) << "r_Local_p_IM" << imag(r_Local_p[thread_Index][0]) << endl  << endl;
		}
	}
	// transmittance
	if(	unwrapped_Structure->calc_Functions.check_Transmittance ||
	    unwrapped_Structure->calc_Functions.check_Absorptance )
	{
		calc_Environmental_Factor(thread_Index, point_Index);

		complex<double> t_s = t_Local_s[thread_Index][0];
		complex<double> t_p = t_Local_p[thread_Index][0];

		calculated_Values.Phi_T_s[point_Index] = arg(t_s)/M_PI*180.;
		calculated_Values.Phi_T_p[point_Index] = arg(t_p)/M_PI*180.;
		calculated_Values.T_s	 [point_Index] = pow(abs(t_s),2)*environment_Factor_s[thread_Index];
		calculated_Values.T_p	 [point_Index] = pow(abs(t_p),2)*environment_Factor_p[thread_Index];
		calculated_Values.T		 [point_Index] = s_Weight * calculated_Values.T_s[point_Index] + p_Weight * calculated_Values.T_p[point_Index];
		calculated_Values.T_Instrumental[point_Index] = calculated_Values.T[point_Index];

		// NaN
		if(isnan(calculated_Values.T[point_Index]))
		{
			calculated_Values.T[point_Index]=10000;  // NaN to 10000. Be careful!
			qInfo() << "Unwrapped_Reflection::fill_Specular_Values  :  T = NaN at point" << point_Index << endl;
			QMessageBox::warning(nullptr, "Unwrapped_Reflection::fill_Specular_Values", "T = NaN");

			qInfo() << "t_Local_s_RE" << real(t_Local_s[thread_Index][0]) << "t_Local_s_IM" << imag(t_Local_s[thread_Index][0]) << endl;
			qInfo() << "t_Local_p_RE" << real(t_Local_p[thread_Index][0]) << "t_Local_p_IM" << imag(t_Local_p[thread_Index][0]) << endl;
			qInfo() << "environment_Factor_s" << environment_Factor_s[thread_Index] << "environment_Factor_p" << environment_Factor_p[thread_Index]  << endl  << endl;
		}
	}
	// absorptance (simple, without scattering!)
	if(	unwrapped_Structure->calc_Functions.check_Absorptance )
	{
		calculated_Values.A_s[point_Index] = 1.-calculated_Values.T_s[point_Index]-calculated_Values.R_s[point_Index];
		calculated_Values.A_p[point_Index] = 1.-calculated_Values.T_p[point_Index]-calculated_Values.R_p[point_Index];
		calculated_Values.A  [point_Index] = s_Weight * calculated_Values.A_s[point_Index] + p_Weight * calculated_Values.A_p[point_Index];

		// crutch for non-displaying slightly negative values (?)
		if(calculated_Values.A_s[point_Index] < DBL_MIN) {calculated_Values.A_s[point_Index] = DBL_MIN;}
		if(calculated_Values.A_p[point_Index] < DBL_MIN) {calculated_Values.A_p[point_Index] = DBL_MIN;}
		if(calculated_Values.A  [point_Index] < DBL_MIN) {calculated_Values.A  [point_Index] = DBL_MIN;}
	}
	// field intensity in-depth
	if( unwrapped_Structure->calc_Functions.check_Field ||
	    unwrapped_Structure->calc_Functions.check_Joule )
	{
		// calculated_Values.field_Intensity	already calculated
		// calculated_Values.absorption_Map		already calculated
	}
	// scattering
	if(	unwrapped_Structure->calc_Functions.check_Scattering )
	{
		// calculated_Values.S_s		[point_Index] already calculated
		// calculated_Values.S_p		[point_Index] already calculated
		calculated_Values.S				[point_Index] = s_Weight * calculated_Values.S_s[point_Index] + p_Weight * calculated_Values.S_p[point_Index];
		calculated_Values.S_Instrumental[point_Index] = calculated_Values.S[point_Index];
	}
	// GISAS
	if(	unwrapped_Structure->calc_Functions.check_GISAS)
	{
		// interpolate the other half
		if(short_Phi_Points!=phi_Points)
		{
			for(size_t phi_Index = 0; phi_Index<short_Phi_Points; phi_Index++)
			{
				GISAS_Slice[thread_Index][phi_Index] = calculated_Values.GISAS_Instrumental[measurement.start_Phi_Index+phi_Index][point_Index];
				phi_Slice  [thread_Index][phi_Index] = measurement.detector_Phi_Angle_Vec  [measurement.start_Phi_Index+phi_Index];
			}
			gsl_spline_init(spline_Vec[thread_Index], phi_Slice[thread_Index].data(), GISAS_Slice[thread_Index].data(), GISAS_Slice[thread_Index].size());
			if(measurement.start_Phi_Index!=0)
			{
				for(size_t phi_Index = 1; phi_Index<measurement.start_Phi_Index; phi_Index++)
				{
					calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = gsl_spline_eval(spline_Vec[thread_Index], -measurement.detector_Phi_Angle_Vec[phi_Index], acc_Vec[thread_Index]);
				}
				size_t phi_Index = 0;
				if(abs(measurement.detector_Phi_Angle_Vec.back()+measurement.detector_Phi_Angle_Vec.front())<=1e-9)
				{
					calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = calculated_Values.GISAS_Instrumental[phi_Points-1][point_Index];
				} else
				{
					calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = gsl_spline_eval(spline_Vec[thread_Index], -measurement.detector_Phi_Angle_Vec[phi_Index], acc_Vec[thread_Index]);
				}
			} else
			if(measurement.end_Phi_Number!=phi_Points)
			{
				for(size_t phi_Index = measurement.end_Phi_Number; phi_Index<phi_Points-1; phi_Index++)
				{
					calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = gsl_spline_eval(spline_Vec[thread_Index], -measurement.detector_Phi_Angle_Vec[phi_Index], acc_Vec[thread_Index]);
				}
				size_t phi_Index = phi_Points-1;
				if(abs(measurement.detector_Phi_Angle_Vec.back()+measurement.detector_Phi_Angle_Vec.front())<=1e-9)
				{
					calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = calculated_Values.GISAS_Instrumental[0][point_Index];
				} else
				{
					calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = gsl_spline_eval(spline_Vec[thread_Index], -measurement.detector_Phi_Angle_Vec[phi_Index], acc_Vec[thread_Index]);
				}
			}
		}
		/// (too expensive for copying)
//		for(int phi_Index = 0; phi_Index<phi_Points; phi_Index++)
//		{
//			calculated_Values.GISAS_Map			[phi_Index][point_Index] = 0;
//		}
//		// s-polarization
//		if( (measurement.polarization + 1) > POLARIZATION_TOLERANCE)
//		{
//			for(int phi_Index = 0; phi_Index<phi_Points; phi_Index++)
//			{
//				// calculated_Values.GISAS_Map_s[phi_Index][point_Index] already calculated
//				calculated_Values.GISAS_Map		[phi_Index][point_Index] += s_Weight * calculated_Values.GISAS_Map_s[phi_Index][point_Index];
//			}
//		}
//		// p-polarization
//		if( (measurement.polarization - 1) < -POLARIZATION_TOLERANCE)
//		{
//			for(int phi_Index = 0; phi_Index<phi_Points; phi_Index++)
//			{
//				// calculated_Values.GISAS_Map_p[phi_Index][point_Index] already calculated
//				calculated_Values.GISAS_Map		[phi_Index][point_Index] += p_Weight * calculated_Values.GISAS_Map_p[phi_Index][point_Index];
//			}
//		}
//		for(int phi_Index = 0; phi_Index<phi_Points; phi_Index++)
//		{
//			calculated_Values.GISAS_Instrumental[phi_Index][point_Index] = calculated_Values.GISAS_Map[phi_Index][point_Index];
//		}
	}
}

void Unwrapped_Reflection::calc_Specular_nMin_nMax_1_Thread(int n_Min, int n_Max, int thread_Index)
{
	for(int point_Index = n_Min; point_Index<n_Max; ++point_Index)
	{
		calc_Specular_1_Point_1_Thread(thread_Index, point_Index);
		if(measurement.measurement_Type == measurement_Types[Specular_Scan] || spec_Scat_mode == SCATTERED_MODE)
		{
			fill_Specular_Values	  (thread_Index, point_Index);
		}
	}
}

void Unwrapped_Reflection::calc_Specular()
{
//	auto start = std::chrono::system_clock::now();

	/// ----------------------------------------------------------------------------------------------------------------------
	/// parallelization
	/// ----------------------------------------------------------------------------------------------------------------------
	int delta_N = num_Points / num_Threads;	// number of points per thread

	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		int n_Min = thread_Index*delta_N;
		int n_Max = (thread_Index + 1)*delta_N;
		if(thread_Index == (num_Threads-1))
		{
			n_Max = num_Points;
		}
		global_Workers[thread_Index] = thread(&Unwrapped_Reflection::calc_Specular_nMin_nMax_1_Thread, this, n_Min, n_Max, thread_Index);
	}
	// join threads
	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		if (global_Workers[thread_Index].joinable()) global_Workers[thread_Index].join();
	}

	// dont go deep in preliminary calculations
	if( measurement.measurement_Type != measurement_Types[Specular_Scan] && spec_Scat_mode != SCATTERED_MODE) return;

//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//	qInfo() << "	parallelization:    "<< elapsed.count()/1000000. << " seconds" << endl << endl;
	/// ----------------------------------------------------------------------------------------------------------------------
	// postprocessing
	{
//		auto start = std::chrono::system_clock::now();

		// interpolation

//		vector<double>* calculated_Curve = &calculated_Values.R;
//		vector<double>* working_Curve = &calculated_Values.R_Instrumental;
//		if( calc_Functions.check_Reflectance)
//		{	calculated_Curve = &calculated_Values.R; calculated_Values.R_Instrumental = calculated_Values.R; working_Curve = &calculated_Values.R_Instrumental;}
//		if( calc_Functions.check_Transmittance)
//		{	calculated_Curve = &calculated_Values.T; calculated_Values.T_Instrumental = calculated_Values.T; working_Curve = &calculated_Values.T_Instrumental;}
		// TODO GISAS Scattering

//		if( measurement.measurement_Type == measurement_Types[Specular_Scan] &&
//			measurement.argument_Type  == argument_Types[Beam_Grazing_Angle] )
//		{
//			if( (measurement.beam_Theta_0_Distribution.FWHM_distribution>0 || measurement.spectral_Distribution.FWHM_distribution>0) && measurement.beam_Theta_0_Angle_Vec.size()>=MIN_ANGULAR_RESOLUTION_POINTS)
//			{
//				// interpolation on dense mesh (tunable)
//				double min_Step_As_Is = find_Min_Mesh_Step(measurement.beam_Theta_0_Angle_Vec);
//				double preliminary_Delta = min_Step_As_Is;//min(min_Step_As_Is/1, min_Resolution/2);  // tunable

//				double first_Argument = measurement.beam_Theta_0_Angle_Vec.first();
//				double last_Argument  = measurement.beam_Theta_0_Angle_Vec.last();
//				unsigned long long num_Points_in_Dense_Mesh = ceil( abs(last_Argument - first_Argument)/preliminary_Delta );
//								   num_Points_in_Dense_Mesh = min(num_Points_in_Dense_Mesh, (unsigned long long)(202020));
//				double real_Delta = (last_Argument - first_Argument)/(num_Points_in_Dense_Mesh-1);  // interpolated curve is equidistant

//				// create dense mesh
//				vector<double> dense_Mesh(num_Points_in_Dense_Mesh);
//				vector<double> dense_Mesh_Interpolated_Curve(num_Points_in_Dense_Mesh);
//				condense_Curve      (measurement.beam_Theta_0_Angle_Vec, calculated_Curve, real_Delta, dense_Mesh_Interpolated_Curve, dense_Mesh);
//				wrap_Condensed_Curve(measurement.beam_Theta_0_Angle_Vec, calculated_Curve, dense_Mesh, dense_Mesh_Interpolated_Curve, measurement.angular_Resolution_Mixed, working_Curve);
//			}
//		}

//		if( measurement.measurement_Type == measurement_Types[Specular_Scan] &&
//			measurement.argument_Type  == argument_Types[Wavelength_Energy] )
//		{
//			if( (measurement.beam_Theta_0_Distribution.FWHM_distribution>0 || measurement.spectral_Distribution.FWHM_distribution>0) && measurement.lambda_Vec.size()>=MIN_SPECTRAL_RESOLUTION_POINTS)
//			{
//				// interpolation on dense mesh (tunable)
//				double min_Step_As_Is = find_Min_Mesh_Step(measurement.lambda_Vec);
//				double preliminary_Delta = min_Step_As_Is;//min(min_Step_As_Is/1, min_Resolution/2);  // tunable

//				double first_Argument = measurement.lambda_Vec.first();
//				double last_Argument  = measurement.lambda_Vec.last();
//				unsigned long long num_Points_in_Dense_Mesh = ceil( abs(last_Argument - first_Argument)/preliminary_Delta );
//								   num_Points_in_Dense_Mesh = min(num_Points_in_Dense_Mesh, (unsigned long long)(202020));
//				double real_Delta = (last_Argument - first_Argument)/(num_Points_in_Dense_Mesh-1);  // interpolated curve is equidistant

//				// create dense mesh
//				vector<double> dense_Mesh(num_Points_in_Dense_Mesh);
//				vector<double> dense_Mesh_Interpolated_Curve(num_Points_in_Dense_Mesh);
//				condense_Curve      (measurement.lambda_Vec, calculated_Curve, real_Delta, dense_Mesh_Interpolated_Curve, dense_Mesh);
//				wrap_Condensed_Curve(measurement.lambda_Vec, calculated_Curve, dense_Mesh, dense_Mesh_Interpolated_Curve, measurement.spectral_Resolution_Mixed, working_Curve);
//			}
//		}

//		// instrumental function (after interpolation!)
//		if(measurement.beam_Geometry.size>DBL_EPSILON)
//		for(size_t point_Index=0; point_Index<R.size(); ++point_Index)
//		{
//			R_Instrumental[point_Index] *= measurement.footprint_Factor_Vec[point_Index];
//		}

		// any way
		if( calc_Functions.check_Reflectance)	{
			for(size_t point_Index=0; point_Index<calculated_Values.R.size(); ++point_Index)	{
				calculated_Values.R_Instrumental[point_Index] += measurement.background;
			}
		}
		if( calc_Functions.check_Transmittance)	{
			for(size_t point_Index=0; point_Index<calculated_Values.T.size(); ++point_Index)	{
				calculated_Values.T_Instrumental[point_Index] += measurement.background;
			}
		}

//		auto end = std::chrono::system_clock::now();
//		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		qInfo() << "	interpolation:    "<< elapsed.count()/1000000. << " seconds" << endl << endl;
	}
}

double Unwrapped_Reflection::find_Min_Mesh_Step(const QVector<double>& argument)
{
	vector<double> steps;
	steps.resize(argument.size()-1);

	for(int i=0; i<argument.size()-1; i++)
	{
		steps[i] = abs(argument[i+1]-argument[i]);
	}

	std::sort(steps.begin(), steps.end());		// sort steps from min to max
	// minimal index
	size_t min_Index = std::min(int(steps.size()),7);// not the first from bottom. At least, 7th from bottom
	double min = max(steps[min_Index], DBL_EPSILON); // not extremely small step
	return min;
}

void Unwrapped_Reflection::wrap_Condensed_Curve(const QVector<double>& sparse_Argument, const vector<double>* sparse_Input_Curve, const vector<double>& dense_Argument, const vector<double>& dense_Curve, const vector<double>& resolution, vector<double>* output_Sparse_Curve)
{
#define Loop_Body 		{																																				\
	                        (*output_Sparse_Curve)[point_Index] = 0;																									\
	                        double weight_Accumulator = DBL_MIN;																										\
	                        double weight = DBL_MIN;																													\
	                        double distance;																															\
	                                                                                                                                                                    \
	                        int near_Central_Dense_Position = int(abs(sparse_Argument[point_Index]-sparse_Argument[0])/delta);											\
	                        int range = ceil(2*resolution[point_Index]/delta);																							\
	                                                                                                                                                                    \
	                        for(int i=max(0, near_Central_Dense_Position-range); i<=min(near_Central_Dense_Position+range, int(dense_Argument.size())-1); i++)			\
	                        {																																			\
	                            distance = abs(dense_Argument[i] - sparse_Argument[point_Index]);																		\
	                            weight = exp(-pow(distance/(resolution[point_Index]/2*1.18),2));																		\
	                                                                                                                                                                    \
	                            (*output_Sparse_Curve)[point_Index] += weight*dense_Curve[i];																			\
	                            weight_Accumulator += weight;																											\
                            }																																			\
	                        double self_weight = 0.2;																													\
	                        (*output_Sparse_Curve)[point_Index] += self_weight*(*sparse_Input_Curve)[point_Index];														\
	                        weight_Accumulator += self_weight;																											\
	                                                                                                                                                                    \
	                        (*output_Sparse_Curve)[point_Index] /= weight_Accumulator;																					\
                        }																																				\
// 0.2 is tunable

	// program
	double delta = abs(dense_Argument[1]-dense_Argument[0]);

	if(sparse_Argument.size()*resolution[0]>50) // tunable
	{
		Global_Variables::parallel_For(sparse_Argument.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
		{
			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
			{
				Loop_Body
			}
		});
	} else
	{
		for(int point_Index=0; point_Index<sparse_Argument.size(); ++point_Index)
		{
			Loop_Body
		}
	}
}

void Unwrapped_Reflection::condense_Curve(const QVector<double>& sparse_Argument_As_Is, const vector<double>* input_Sparse_Curve_As_Is, double real_Delta, vector<double>& output_Dense_Curve, vector<double>& output_Dense_Argument)
{
	// OPTIMIZE

	// make argument strictly increasing if necessary
	QVector<double> sparse_Argument = sparse_Argument_As_Is;
	vector<double> input_Sparse_Curve = (*input_Sparse_Curve_As_Is);
	bool was_Reversed = false;
	if(sparse_Argument.first() >= sparse_Argument.last())
	{
		reverse(sparse_Argument.begin(),sparse_Argument.end());
		reverse(input_Sparse_Curve.begin(),input_Sparse_Curve.end());
		was_Reversed = true;
	}

	// remove non-monotomic areas
	for(int i=0; i<sparse_Argument.size()-1; i++)
	{
		if(sparse_Argument[i+1]<=sparse_Argument[i])
		{
			sparse_Argument.remove(i+1);
			input_Sparse_Curve.erase(input_Sparse_Curve.begin()+i+1);
		}
	}

	const gsl_interp_type* interp_type = gsl_interp_steffen;
	gsl_interp_accel* Acc = gsl_interp_accel_alloc();
	gsl_spline* Spline = gsl_spline_alloc(interp_type, input_Sparse_Curve.size());
	gsl_spline_init(Spline, sparse_Argument.data(), input_Sparse_Curve.data(), input_Sparse_Curve.size());

	double minimum = min(sparse_Argument.first(),sparse_Argument.last());
	double maximum = max(sparse_Argument.first(),sparse_Argument.last());
	double new_Arg;
//	Global_Variables::parallel_For(output_Dense_Curve.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
//	{
//		for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
	    for(uint point_Index=0; point_Index<output_Dense_Curve.size(); ++point_Index)
		{
			new_Arg = sparse_Argument.first()+point_Index*abs(real_Delta);
			if(new_Arg>maximum) new_Arg = maximum; // control over machine precision
			if(new_Arg<minimum) new_Arg = minimum; // control over machine precision
			output_Dense_Argument[point_Index] = new_Arg;
			output_Dense_Curve   [point_Index] = gsl_spline_eval(Spline, new_Arg, Acc);
		}
//	});

	if(was_Reversed)
	{
		reverse(output_Dense_Argument.begin(),output_Dense_Argument.end());
		reverse(output_Dense_Curve.begin(),output_Dense_Curve.end());
	}

	gsl_spline_free(Spline);
	gsl_interp_accel_free(Acc);
}

void Unwrapped_Reflection::interpolate_Curve(int res_Points, const QVector<double> &argument, const vector<double>& resolution, const vector<double>& input_Curve, vector<double>& output_Curve)
{
	// OPTIMIZE
	const gsl_interp_type* interp_type = gsl_interp_steffen;
	gsl_interp_accel* Acc = gsl_interp_accel_alloc();
	gsl_spline* Spline = gsl_spline_alloc(interp_type, input_Curve.size());

	gsl_spline_init(Spline, argument.data(), input_Curve.data(), input_Curve.size());

	double minimum = min(argument.first(),argument.last());
	double maximum = max(argument.first(),argument.last());

//	Global_Variables::parallel_For(input_Curve.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
//	{
	    for(uint point_Index=0; point_Index<input_Curve.size(); ++point_Index)
//		for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
		{
			double delta = 2*resolution[point_Index]/res_Points; // spectral resolution is not constant in absolute values

			output_Curve[point_Index] = 0;
			double weight, x;
			double weight_Accumulator = 0;
			for(int i=-res_Points; i<res_Points; ++i)
			{
				x = i*delta;
				if((argument[point_Index]+x>=minimum)&&
				   (argument[point_Index]+x<=maximum))
				{
					weight = exp(-pow(x/(resolution[point_Index]/2),2));
					weight_Accumulator += weight;
					output_Curve[point_Index] += weight*gsl_spline_eval(Spline, argument[point_Index]+i*delta, Acc);
				}
			}
			output_Curve[point_Index] /= weight_Accumulator;
		}
//	});

	gsl_spline_free(Spline);
	gsl_interp_accel_free(Acc);
}
