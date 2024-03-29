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

double base_PSD(double cos_Phi, Params* params)
{
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	double value = u->PSD_Gauss_Peak_2D_Func_Vec[thread_Index]( u->substrate.PSD_Gauss_Peak_2D_Factor,
																u->substrate.roughness_Model.peak_Frequency.value,
																u->substrate.roughness_Model.peak_Frequency_Width.value,
																u->measurement.k_Value,
																params->cos_Theta,
																params->cos_Theta_0,
																cos_Phi,
																u->substrate_Node->spline_PSD_Peak,
																u->substrate_Node->acc_PSD_Peak);
	if(u->multilayer->imperfections_Model.add_Measured_PSD_2D && u->multilayer->imperfections_Model.PSD_2D.is_Loaded())
	{
		double nu = u->measurement.k_Value*sqrt(params->cos_Theta*params->cos_Theta + params->cos_Theta_0*params->cos_Theta_0 - 2*params->cos_Theta_0*params->cos_Theta*cos_Phi)/(2*M_PI); // here we need real frequency

		double nu_Start = u->multilayer->imperfections_Model.PSD_2D.argument.front();
		double nu_End   = u->multilayer->imperfections_Model.PSD_2D.argument.back();

		// without nu_Limit here
		if(nu<=nu_Start || nu>=nu_End )
		{
			value += u->PSD_2D_Func_Vec[thread_Index](  u->substrate.main_PSD_2D_factor,
														u->substrate.roughness_Model.cor_radius.value,
														u->substrate.roughness_Model.fractal_alpha.value,
														u->measurement.k_Value,
														params->cos_Theta,
														params->cos_Theta_0,
														cos_Phi,
														u->substrate_Node->spline_PSD_FG_2D,
														u->substrate_Node->acc_PSD_FG_2D);
		} else
		{
			value += Global_Variables::PSD_Measured_2D( u->substrate.main_PSD_2D_factor,
														u->substrate.roughness_Model.cor_radius.value,
														u->substrate.roughness_Model.fractal_alpha.value,
														u->measurement.k_Value,
														params->cos_Theta,
														params->cos_Theta_0,
														cos_Phi,
														u->substrate_Node->spline_PSD_Meas_2D,
														u->substrate_Node->acc_PSD_Meas_2D);
		}
	} else
	{
		value += u->PSD_2D_Func_Vec[thread_Index](  u->substrate.main_PSD_2D_factor,
													u->substrate.roughness_Model.cor_radius.value,
													u->substrate.roughness_Model.fractal_alpha.value,
													u->measurement.k_Value,
													params->cos_Theta,
													params->cos_Theta_0,
													cos_Phi,
													u->substrate_Node->spline_PSD_FG_2D,
													u->substrate_Node->acc_PSD_FG_2D);
	}
	return value;
}

double function_Scattering_Replication_Factor_2D_s (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	double common_PSD_2D_Factor = base_PSD(cos_Phi, params);

	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
	{
		double nu_Beta = Global_Variables::nu_Beta_2D(u->multilayer->imperfections_Model.vertical_Inheritance_Frequency,
													  u->unwrapped_Structure->beta_Threaded[thread_Index][i-1],
													  u->measurement.k_Value,
													  params->cos_Theta,
													  params->cos_Theta_0,
													  cos_Phi);

		u->cross_Exp_Factor_2D[thread_Index][i][i-1] = exp(-nu_Beta * u->unwrapped_Structure->PSD_h_mu_Threaded[thread_Index][i-1]);
		partially_Coherent_Sum += u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_s[thread_Index][i][i-1];

		for (int j = i-2; j>=0; j--)
		{
			u->cross_Exp_Factor_2D[thread_Index][i][j] = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->cross_Exp_Factor_2D[thread_Index][i-1][j];
			partially_Coherent_Sum += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_s[thread_Index][i][j];
		}
	}
	return (partially_Coherent_Sum + params->incoherent_Sum_s) * common_PSD_2D_Factor;
}
double function_Scattering_Replication_Factor_2D_p (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	double common_PSD_2D_Factor = base_PSD(cos_Phi, params);

	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
	{
		double nu_Beta = Global_Variables::nu_Beta_2D(u->multilayer->imperfections_Model.vertical_Inheritance_Frequency,
													  u->unwrapped_Structure->beta_Threaded[thread_Index][i-1],
													  u->measurement.k_Value,
													  params->cos_Theta,
													  params->cos_Theta_0,
													  cos_Phi);

		u->cross_Exp_Factor_2D[thread_Index][i][i-1] = exp(-nu_Beta * u->unwrapped_Structure->PSD_h_mu_Threaded[thread_Index][i-1]);
		partially_Coherent_Sum += u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->half_Sum_Field_Term_p[thread_Index][i][i-1];

		for (int j = i-2; j>=0; j--)
		{
			u->cross_Exp_Factor_2D[thread_Index][i][j] = u->cross_Exp_Factor_2D[thread_Index][i][i-1] * u->cross_Exp_Factor_2D[thread_Index][i-1][j];
			partially_Coherent_Sum += u->cross_Exp_Factor_2D[thread_Index][i][j] * u->half_Sum_Field_Term_p[thread_Index][i][j];
		}
	}
	return (partially_Coherent_Sum + params->incoherent_Sum_p) * common_PSD_2D_Factor;
}
double function_Scattering_Replication_Factor_2D_sp(double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	double common_PSD_2D_Factor = base_PSD(cos_Phi, params);

	double partially_Coherent_Sum_s = 0;
	double partially_Coherent_Sum_p = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
	{
		double nu_Beta = Global_Variables::nu_Beta_2D(u->multilayer->imperfections_Model.vertical_Inheritance_Frequency,
													  u->unwrapped_Structure->beta_Threaded[thread_Index][i-1],
													  u->measurement.k_Value,
													  params->cos_Theta,
													  params->cos_Theta_0,
													  cos_Phi);

		u->cross_Exp_Factor_2D[thread_Index][i][i-1] = exp(-nu_Beta * u->unwrapped_Structure->PSD_h_mu_Threaded[thread_Index][i-1]);
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

double function_Scattering_Linear_Growth_Alpha_2D_s (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1] = base_PSD(cos_Phi, params);

	// incoherent part
	double incoherent_Sum = u->intensity_Term_Boundary_s[thread_Index][u->num_Boundaries_Sharp-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1];
	for (int i = u->num_Boundaries_Sharp-2; i>=0; i--)
	{
		double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->multilayer->imperfections_Model.vertical_Inheritance_Frequency,
														  u->unwrapped_Structure->alpha_Threaded[thread_Index][i],
		                                                  u->measurement.k_Value,
		                                                  params->cos_Theta,
		                                                  params->cos_Theta_0,
		                                                  cos_Phi);
		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_h_mu_Threaded[thread_Index][i]);

		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
													+ Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
																							 nu_Alpha_2/u->unwrapped_Structure->mu_Threaded[thread_Index][i],
																							 u->unwrapped_Structure->omega_Threaded[thread_Index][i],
																							 u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum += u->intensity_Term_Boundary_s[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
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
double function_Scattering_Linear_Growth_Alpha_2D_p (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1] = base_PSD(cos_Phi, params);

	// incoherent part
	double incoherent_Sum = u->intensity_Term_Boundary_p[thread_Index][u->num_Boundaries_Sharp-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1];
	for (int i = u->num_Boundaries_Sharp-2; i>=0; i--)
	{
		double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->multilayer->imperfections_Model.vertical_Inheritance_Frequency,
														  u->unwrapped_Structure->alpha_Threaded[thread_Index][i],
		                                                  u->measurement.k_Value,
		                                                  params->cos_Theta,
		                                                  params->cos_Theta_0,
		                                                  cos_Phi);
		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_h_mu_Threaded[thread_Index][i]);

		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
													   + Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
																								nu_Alpha_2/u->unwrapped_Structure->mu_Threaded[thread_Index][i],
																								u->unwrapped_Structure->omega_Threaded[thread_Index][i],
																								u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum += u->intensity_Term_Boundary_p[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
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
double function_Scattering_Linear_Growth_Alpha_2D_sp(double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1] = base_PSD(cos_Phi, params);

	// incoherent part
	double incoherent_Sum_s = u->intensity_Term_Boundary_s[thread_Index][u->num_Boundaries_Sharp-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1];
	double incoherent_Sum_p = u->intensity_Term_Boundary_p[thread_Index][u->num_Boundaries_Sharp-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1];
	for (int i = u->num_Boundaries_Sharp-2; i>=0; i--)
	{
		double nu_Alpha_2 = Global_Variables::nu_Alpha_2D(u->multilayer->imperfections_Model.vertical_Inheritance_Frequency,
														  u->unwrapped_Structure->alpha_Threaded[thread_Index][i],
		                                                  u->measurement.k_Value,
		                                                  params->cos_Theta,
		                                                  params->cos_Theta_0,
		                                                  cos_Phi);
		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-nu_Alpha_2 * u->unwrapped_Structure->PSD_h_mu_Threaded[thread_Index][i]);

		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
												   + Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
																							nu_Alpha_2/u->unwrapped_Structure->mu_Threaded[thread_Index][i],
																							u->unwrapped_Structure->omega_Threaded[thread_Index][i],
																							u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum_s += u->intensity_Term_Boundary_s[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
		incoherent_Sum_p += u->intensity_Term_Boundary_p[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum_s = 0;
	double partially_Coherent_Sum_p = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
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

double function_Scattering_Linear_Growth_n_1_4_2D_s (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1] = base_PSD(cos_Phi, params);

	// nu
	double nu_2 = Global_Variables::nu2(u->measurement.k_Value, params->cos_Theta, params->cos_Theta_0, cos_Phi);
	double nu_1 = sqrt(nu_2);
	double nu_3 = nu_1*nu_2;
	double nu_4 = nu_2*nu_2;

	// incoherent part
	double incoherent_Sum = u->intensity_Term_Boundary_s[thread_Index][u->num_Boundaries_Sharp-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1];
	for (int i = u->num_Boundaries_Sharp-2; i>=0; i--)
	{
		double b_Function =  u->unwrapped_Structure->a1_Threaded[thread_Index][i]*nu_1 +
							 u->unwrapped_Structure->a2_Threaded[thread_Index][i]*nu_2 +
							 u->unwrapped_Structure->a3_Threaded[thread_Index][i]*nu_3 +
							 u->unwrapped_Structure->a4_Threaded[thread_Index][i]*nu_4;

		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-b_Function);

		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
											   + Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
																						b_Function,
																						u->unwrapped_Structure->omega_Threaded[thread_Index][i],
																						u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum += u->intensity_Term_Boundary_s[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
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
double function_Scattering_Linear_Growth_n_1_4_2D_p (double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1] = base_PSD(cos_Phi, params);

	// nu
	double nu_2 = Global_Variables::nu2(u->measurement.k_Value, params->cos_Theta, params->cos_Theta_0, cos_Phi);
	double nu_1 = sqrt(nu_2);
	double nu_3 = nu_1*nu_2;
	double nu_4 = nu_2*nu_2;

	// incoherent part
	double incoherent_Sum = u->intensity_Term_Boundary_p[thread_Index][u->num_Boundaries_Sharp-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1];
	for (int i = u->num_Boundaries_Sharp-2; i>=0; i--)
	{
		double b_Function =  u->unwrapped_Structure->a1_Threaded[thread_Index][i]*nu_1 +
							 u->unwrapped_Structure->a2_Threaded[thread_Index][i]*nu_2 +
							 u->unwrapped_Structure->a3_Threaded[thread_Index][i]*nu_3 +
							 u->unwrapped_Structure->a4_Threaded[thread_Index][i]*nu_4;

		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-b_Function);

		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
													   + Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
																								b_Function,
																								u->unwrapped_Structure->omega_Threaded[thread_Index][i],
																								u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum += u->intensity_Term_Boundary_p[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
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
double function_Scattering_Linear_Growth_n_1_4_2D_sp(double phi, void* p)
{
	Params* params = reinterpret_cast<Params*>(p);
	int thread_Index = params->thread_Index;
	Unwrapped_Reflection* u = params->unwrapped_Reflection;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// PSD
	u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1] = base_PSD(cos_Phi, params);

	// nu
	double nu_2 = Global_Variables::nu2(u->measurement.k_Value, params->cos_Theta, params->cos_Theta_0, cos_Phi);
	double nu_1 = sqrt(nu_2);
	double nu_3 = nu_1*nu_2;
	double nu_4 = nu_2*nu_2;

	// incoherent part
	double incoherent_Sum_s = u->intensity_Term_Boundary_s[thread_Index][u->num_Boundaries_Sharp-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1];
	double incoherent_Sum_p = u->intensity_Term_Boundary_p[thread_Index][u->num_Boundaries_Sharp-1] * u->PSD_Factor_Boundary[thread_Index][u->num_Boundaries_Sharp-1];
	for (int i = u->num_Boundaries_Sharp-2; i>=0; i--)
	{
		double b_Function =  u->unwrapped_Structure->a1_Threaded[thread_Index][i]*nu_1 +
							 u->unwrapped_Structure->a2_Threaded[thread_Index][i]*nu_2 +
							 u->unwrapped_Structure->a3_Threaded[thread_Index][i]*nu_3 +
							 u->unwrapped_Structure->a4_Threaded[thread_Index][i]*nu_4;

		u->cross_Exp_Factor_2D[thread_Index][i+1][i] = exp(-b_Function);

		u->PSD_Factor_Boundary[thread_Index][i] = u->cross_Exp_Factor_2D[thread_Index][i+1][i]*u->PSD_Factor_Boundary[thread_Index][i+1]
												   + Global_Variables::PSD_Linear_Growth_2D(u->cross_Exp_Factor_2D[thread_Index][i+1][i],
																							b_Function,
																							u->unwrapped_Structure->omega_Threaded[thread_Index][i],
																							u->unwrapped_Structure->thickness_Threaded[thread_Index][i]);
		incoherent_Sum_s += u->intensity_Term_Boundary_s[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
		incoherent_Sum_p += u->intensity_Term_Boundary_p[thread_Index][i] * u->PSD_Factor_Boundary[thread_Index][i];
	}

	// partially coherent part
	double partially_Coherent_Sum_s = 0;
	double partially_Coherent_Sum_p = 0;
	for (int i = 1; i<u->num_Boundaries_Sharp; i++)
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

struct Params_Particles
{
	Unwrapped_Reflection* unwrapped_Reflection;
	int thread_Index;
	bool phi_As_Angle;
	bool common;
	double cos_Theta;
	double cos_Theta_0;
	Data* last_Item;
	Node* last_Node;
	vector<complex<double>>& layer_Field_Factor_Vec; // used only in partial correlation
};
double function_Scattering_Particles_Partial_Correlation(double phi, void* p)
{
	Params_Particles* params = reinterpret_cast<Params_Particles*>(p);
	Unwrapped_Reflection* u = params->unwrapped_Reflection;
	int thread_Index = params->thread_Index;
	bool common = params->common;
	double cos_Theta = params->cos_Theta;
	double cos_Theta_0 = params->cos_Theta_0;
	Data* last_Item = params->last_Item;
	Node* last_Node = params->last_Node;
	vector<complex<double>>& layer_Field_Factor_Vec = params->layer_Field_Factor_Vec;
	double k = u->measurement.k_Value;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// main function for single phi
	double single_Point_Value = 0;

	double G1_Type_Value = last_Node->G1_Type_Outer();
	double G2_Type_Value = G1_Type_Value; // for disorder
	double G2_Type_Value_Sqrt = sqrt(G2_Type_Value); // for disorder

	for(int item_Index : u->used_Appropriate_Item_Index_Vec)
	{
		Node* node = u->short_Flat_Calc_Tree[item_Index];
		Data& item = u->appropriate_Item_Vec[item_Index];
		Data& order_Item = common ? (*last_Item) : item;

		bool is_Z_Deviation = order_Item.particles_Model.particle_Z_Position_Deviation.value > DBL_EPSILON;

		complex<double> d_Eps = node->delta_Epsilon_Contrast;
		double d_Eps_Norm = norm(d_Eps);

		if(is_Z_Deviation)
		{
			double incoherent_Field_Factor = u->body_Scattering_Particles_Partial_Correlation_Z_Deviation(layer_Field_Factor_Vec, thread_Index, item_Index, order_Item.particles_Model.particle_Interference_Function,
																										  last_Node, G1_Type_Value, G2_Type_Value, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
			single_Point_Value += d_Eps_Norm * incoherent_Field_Factor;
		} else
		{
			double incoherent_Field_Factor = u->body_Scattering_Particles_Partial_Correlation(layer_Field_Factor_Vec, thread_Index, item_Index, last_Item->particles_Model.particle_Interference_Function,
																							  last_Node, G2_Type_Value_Sqrt, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
			single_Point_Value += incoherent_Field_Factor;
		}
	}
	// partially coherent part summation
	double q2 = k*k*(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);
	double coherent_Sum = 0;
	for(int& l : u->unwrapped_Structure->particles_Index_Vec)
	{
		double inheritance_Factor = 1;
		for(int& j : u->unwrapped_Structure->particles_Index_Vec)
		{
			if(j<l)
			{
				double s_j = u->unwrapped_Structure->lateral_Sigma_Particles[j];
				double ex = q2*s_j*s_j/2.;
				if(ex<20)
				{
					inheritance_Factor *= exp(-ex);
					coherent_Sum += 2*real(      layer_Field_Factor_Vec[l] *
											conj(layer_Field_Factor_Vec[j])
										   ) *  inheritance_Factor;// * u->unwrapped_Structure->particles_Inheritance_Factor[l][j];
				}
			}
		}
	}
	single_Point_Value += coherent_Sum;

	return single_Point_Value;
}
double function_Scattering_Particles_Full_Correlation(double phi, void* p)
{
	Params_Particles* params = reinterpret_cast<Params_Particles*>(p);
	Unwrapped_Reflection* u = params->unwrapped_Reflection;
	int thread_Index = params->thread_Index;
	bool common = params->common;
	double cos_Theta = params->cos_Theta;
	double cos_Theta_0 = params->cos_Theta_0;
	Data* last_Item = params->last_Item;
	Node* last_Node = params->last_Node;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// main function for single phi
	double single_Point_Value = 0;

	double G1_Type_Value = last_Node->G1_Type_Outer();
	double G2_Type_Value_Sqrt = sqrt(G1_Type_Value); // for disorder
	complex<double> coherent_Field_Factor = 0;

	for(int item_Index : u->used_Appropriate_Item_Index_Vec)
	{
		Node* node = u->short_Flat_Calc_Tree[item_Index];
		Data& item = u->appropriate_Item_Vec[item_Index];
		Data& order_Item = common ? (*last_Item) : item;

		bool is_Z_Deviation = order_Item.particles_Model.particle_Z_Position_Deviation.value > DBL_EPSILON;

		complex<double> d_Eps = node->delta_Epsilon_Contrast;
		double d_Eps_Norm = norm(d_Eps);

		if(is_Z_Deviation)
		{
			double incoherent_Field_Factor = u->body_Scattering_Particles_Full_Correlation_Z_Deviation(coherent_Field_Factor, thread_Index, item_Index, last_Item->particles_Model.particle_Interference_Function,
																									   last_Node, G1_Type_Value, G2_Type_Value_Sqrt, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
			single_Point_Value += d_Eps_Norm * incoherent_Field_Factor;
		} else
		{
			u->body_Scattering_Particles_Full_Correlation(coherent_Field_Factor, thread_Index, item_Index, last_Item->particles_Model.particle_Interference_Function,
														  last_Node, G2_Type_Value_Sqrt, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
		}
	}
	single_Point_Value += norm(coherent_Field_Factor);

	return single_Point_Value;
}
double function_Scattering_Particles_Zero_Correlation(double phi, void* p)
{
	Params_Particles* params = reinterpret_cast<Params_Particles*>(p);
	Unwrapped_Reflection* u = params->unwrapped_Reflection;
	int thread_Index = params->thread_Index;
	bool common = params->common;
	double cos_Theta = params->cos_Theta;
	double cos_Theta_0 = params->cos_Theta_0;
	Data* last_Item = params->last_Item;
	Node* last_Node = params->last_Node;

	// passed argument: phi or cos(phi)
	double cos_Phi;
	if(params->phi_As_Angle)
		cos_Phi = cos(qDegreesToRadians(phi));
	else
		cos_Phi = phi;

	// main function for single phi
	double single_Point_Value = 0;
	for(int item_Index : u->used_Appropriate_Item_Index_Vec)
	{
		Data& item = u->appropriate_Item_Vec[item_Index];
		Node* node = u->short_Flat_Calc_Tree[item_Index];
		Node* G2_node    = common ?   last_Node  : node;
		Data& order_Item = common ? (*last_Item) : item;

		bool is_Z_Deviation = order_Item.particles_Model.particle_Z_Position_Deviation.value > DBL_EPSILON;

		complex<double> d_Eps = node->delta_Epsilon_Contrast;
		double d_Eps_Norm = norm(d_Eps);
		double G1_Type_Value = G2_node->G1_Type_Outer();
		double G2_Type_Value_Sqrt = sqrt(G1_Type_Value);

		if(is_Z_Deviation)
		{
			double field_With_G_2D_Factor = u->body_Scattering_Particles_Zero_Correlation_Z_Deviation(thread_Index, item_Index, order_Item.particles_Model.particle_Interference_Function,
																									  G2_node, G1_Type_Value, cos_Theta, cos_Theta_0, cos_Phi);
			single_Point_Value += d_Eps_Norm * field_With_G_2D_Factor;
		} else
		{
			double field_With_G_2D_Factor = u->body_Scattering_Particles_Zero_Correlation(thread_Index, item_Index,  order_Item.particles_Model.particle_Interference_Function,
																						  G2_node, G2_Type_Value_Sqrt, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
			single_Point_Value += field_With_G_2D_Factor;
		}
	}
	return single_Point_Value;
}

Unwrapped_Reflection::Unwrapped_Reflection(const vector<Node*>& short_Flat_Calc_Tree, Calculated_Values& calculated_Values, Unwrapped_Structure* unwrapped_Structure, QString spec_Scat_mode):
	unwrapped_Structure  (unwrapped_Structure),
	multilayer			 (unwrapped_Structure->multilayer),
	num_Threads			 (reflectivity_calc_threads),
	num_Media_Sharp		 (unwrapped_Structure->num_Media_Sharp),
	num_Layers_Sharp	 (unwrapped_Structure->num_Media_Sharp-2 ),
	num_Boundaries_Sharp (unwrapped_Structure->num_Media_Sharp-1 ),
	num_Media			 (unwrapped_Structure->multilayer->discretization_Parameters.enable_Discretization ? unwrapped_Structure->num_Discretized_Media   : unwrapped_Structure->num_Media_Sharp   ),
	num_Layers			 (unwrapped_Structure->multilayer->discretization_Parameters.enable_Discretization ? unwrapped_Structure->num_Discretized_Media-2 : unwrapped_Structure->num_Media_Sharp-2 ),
	num_Boundaries		 (unwrapped_Structure->multilayer->discretization_Parameters.enable_Discretization ? unwrapped_Structure->num_Discretized_Media-1 : unwrapped_Structure->num_Media_Sharp-1 ),
	calc_Functions		 (unwrapped_Structure->calc_Functions),
	calculated_Values	 (calculated_Values),
	spec_Scat_mode		 (spec_Scat_mode),
	measurement			 (unwrapped_Structure->measurement),
	DWBA_n_Max_Series	 (unwrapped_Structure->multilayer->imperfections_Model.DWBA_n_Max_Series),
	short_Flat_Calc_Tree (short_Flat_Calc_Tree),
	media_Node_Map_Vector(unwrapped_Structure->media_Node_Map_Vector),
	media_Data_Map_Vector(unwrapped_Structure->media_Data_Map_Vector),

	/// specular
	epsilon_Ambient	 (num_Threads),
	epsilon_Substrate(num_Threads),

	environment_Factor_s(num_Threads),
	environment_Factor_p(num_Threads),

	hi	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_Hi : calculated_Values.q_Hi ),

    r_Fresnel_s(num_Threads,vector<complex<double>>(num_Boundaries)),
    r_Fresnel_p(num_Threads,vector<complex<double>>(num_Boundaries)),
    r_Local_s  (num_Threads,vector<complex<double>>(num_Boundaries)),
    r_Local_p  (num_Threads,vector<complex<double>>(num_Boundaries)),
	r_Exp_s    (num_Threads,vector<complex<double>>(num_Boundaries)),
	r_Exp_p    (num_Threads,vector<complex<double>>(num_Boundaries)),

    t_Fresnel_s(num_Threads,vector<complex<double>>(num_Boundaries)),
    t_Fresnel_p(num_Threads,vector<complex<double>>(num_Boundaries)),
    t_Local_s  (num_Threads,vector<complex<double>>(num_Boundaries)),
    t_Local_p  (num_Threads,vector<complex<double>>(num_Boundaries)),

	exponenta  (spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_Exponenta : calculated_Values.q_Exponenta ),
	exponenta_2(num_Threads,vector<complex<double>>(num_Boundaries)),

//	weak_Factor_R(num_Threads,vector<complex<double>>(num_Boundaries_Sharp)),
//	weak_Factor_T(num_Threads,vector<complex<double>>(num_Boundaries_Sharp)),
	weak_Factor_R(num_Threads,vector<double>(num_Boundaries_Sharp)),
	weak_Factor_T(num_Threads,vector<double>(num_Boundaries_Sharp)),
	specular_Debye_Waller_Weak_Factor_R_Final(num_Threads),

	/// fields

	U_i_s	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_i_s : calculated_Values.q_U_i_s ),
    U_r_s	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_r_s : calculated_Values.q_U_r_s ),
    U_i_p	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_i_p : calculated_Values.q_U_i_p ),
    U_r_p	(spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_r_p : calculated_Values.q_U_r_p ),

    /// roughness

    PSD_1D_Func_Vec(num_Threads),
	PSD_Gauss_Peak_1D_Func_Vec(num_Threads),
    PSD_2D_Func_Vec(num_Threads),
	PSD_Gauss_Peak_2D_Func_Vec(num_Threads),
	Cor_Func_Vec(num_Threads),

	boundary_Item_Vec(num_Boundaries_Sharp),
    PSD_Factor_Item	 (num_Threads),
	cor_Func_Item	 (num_Threads),
	incoherent_Diagonal_Term(num_Threads),

    spline_Vec(num_Threads),
    acc_Vec(num_Threads),
    GISAS_Slice(num_Threads),
	phi_Slice(num_Threads),
	q_Spline_Points(800) // 63
{
	s_Weight = (1. + measurement.polarization) / 2.;
	p_Weight = (1. - measurement.polarization) / 2.;

	has_S_Pol = (measurement.polarization + 1) >  POLARIZATION_TOLERANCE;
	has_P_Pol = (measurement.polarization - 1) < -POLARIZATION_TOLERANCE;

	pure_S_Pol = !has_P_Pol;
	pure_P_Pol = !has_S_Pol;
	mixed_Pol = has_S_Pol && has_P_Pol;

	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type  == argument_Types[Beam_Grazing_Angle] )	num_Points = measurement.beam_Theta_0_Angle_Vec.size();
		if( measurement.argument_Type  == argument_Types[Wavelength_Energy] )	num_Points = measurement.lambda_Vec.size();
	}
	if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		if_Single_Beam_Value = true;
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
	// takes time only at first calculation or significant changes in points and boundaries
	{
		hi.resize(num_Points);
		exponenta.resize(num_Points);
		U_i_s.resize(num_Points);
		U_r_s.resize(num_Points);
		U_i_p.resize(num_Points);
		U_r_p.resize(num_Points);

		for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
		{
			hi[point_Index].resize(num_Media);
			exponenta[point_Index].resize(num_Boundaries);
			U_i_s[point_Index].resize(num_Media);
			U_r_s[point_Index].resize(num_Media);
			U_i_p[point_Index].resize(num_Media);
			U_r_p[point_Index].resize(num_Media);
		}
	}

	if( spec_Scat_mode == SPECULAR_MODE )
	{
		// reflectance
		if(	calc_Functions.check_Reflectance ||
			calc_Functions.check_Absorptance ||
			calc_Functions.check_Scattering ||
			calc_Functions.check_GISAS )
		{
            calculated_Values.Phi_R_s.resize(num_Points);
			calculated_Values.Phi_R_p.resize(num_Points);
			calculated_Values.R_s.resize(num_Points);
			calculated_Values.R_p.resize(num_Points);
			calculated_Values.R  .resize(num_Points);
			calculated_Values.R_Instrumental.resize(num_Points);
		}
		// transmittance
		if(	calc_Functions.check_Transmittance ||
			calc_Functions.check_Absorptance)
		{
            calculated_Values.Phi_T_s.resize(num_Points);
			calculated_Values.Phi_T_p.resize(num_Points);
			calculated_Values.T_s.resize(num_Points);
			calculated_Values.T_p.resize(num_Points);
			calculated_Values.T  .resize(num_Points);
			calculated_Values.T_Instrumental.resize(num_Points);
		}
		// absorptance
		if(	calc_Functions.check_Absorptance)
		{
			calculated_Values.A_s.resize(num_Points);
			calculated_Values.A_p.resize(num_Points);
			calculated_Values.A  .resize(num_Points);
		}
		// field intensity in-depth
		if( calc_Functions.check_Field || calc_Functions.check_Joule)
		{
			if(multilayer->discretization_Parameters.enable_Discretization)	boundaries_Enlarged = unwrapped_Structure->discretized_Slices_Boundaries;
			else															boundaries_Enlarged = unwrapped_Structure->boundaries_Position;
			// plus one element to end
			boundaries_Enlarged.push_back(boundaries_Enlarged.back());

			// if too much slices
			//unwrapped_Structure->num_Field_Slices = min(unwrapped_Structure->num_Field_Slices, 1000000/num_Points);
			//unwrapped_Structure->field_Z_Positions.resize(unwrapped_Structure->num_Field_Slices);

			calculated_Values.field_Intensity.resize(num_Points);
			calculated_Values.absorption_Map.resize(num_Points);
			for(size_t i=0; i<num_Points; i++)
			{
				calculated_Values.field_Intensity[i].resize(unwrapped_Structure->num_Field_Slices);
				calculated_Values.absorption_Map [i].resize(unwrapped_Structure->num_Field_Slices);
			}
		}
	}

	fill_Item_Id_Map();
	fill_Boundary_Item();

	if( spec_Scat_mode == SCATTERED_MODE )
	{
		// scattering
		if(	unwrapped_Structure->calc_Functions.check_Scattering )
		{
			calculated_Values.S_s.resize(num_Points);
			calculated_Values.S_p.resize(num_Points);
			calculated_Values.S  .resize(num_Points);
			calculated_Values.S_Instrumental.resize(num_Points);
		}
		// GISAS
		if(	unwrapped_Structure->calc_Functions.check_GISAS )
		{
			phi_Points = measurement.detector_Phi_Angle_Vec.size();
			short_Phi_Points = measurement.end_Phi_Number - measurement.start_Phi_Index,

			calculated_Values.GISAS_Map.resize(phi_Points);
			calculated_Values.GISAS_Instrumental.resize(phi_Points);
			for(size_t i=0; i<phi_Points; i++)
			{
				calculated_Values.GISAS_Map[i].resize(num_Points);
				calculated_Values.GISAS_Instrumental[i].resize(num_Points);
			}

			// interpolation to the minor half of phi values due to symmetry of PSD
			for(int thread_Index = 0; thread_Index<num_Threads; thread_Index++)
			{
				acc_Vec[thread_Index] = gsl_interp_accel_alloc();
				spline_Vec[thread_Index] = gsl_spline_alloc(gsl_interp_steffen, short_Phi_Points);
				GISAS_Slice[thread_Index].resize(short_Phi_Points);
				phi_Slice[thread_Index].resize(short_Phi_Points);
			}
			delete_GISAXS_Spline = true;
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
			for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
			{
				PSD_Factor_Item[thread_Index].resize(short_Flat_Calc_Tree.size());
				cor_Func_Item[thread_Index].resize(short_Flat_Calc_Tree.size());
				incoherent_Diagonal_Term[thread_Index].resize(short_Flat_Calc_Tree.size());

				for(size_t item_Index=0; item_Index<short_Flat_Calc_Tree.size(); item_Index++)
				{
					incoherent_Diagonal_Term[thread_Index][item_Index].resize(DWBA_n_Max_Series);
				}
			}

			/// PT DWBA SA CSA
			{
				// s-polarization
				if(has_S_Pol)
				{
					b1_Up_Boundary_s.resize(num_Threads);
					b2_Up_Boundary_s.resize(num_Threads);
					b3_Up_Boundary_s.resize(num_Threads);
					b4_Up_Boundary_s.resize(num_Threads);

					b1_Low_Boundary_s.resize(num_Threads);
					b2_Low_Boundary_s.resize(num_Threads);
					b3_Low_Boundary_s.resize(num_Threads);
					b4_Low_Boundary_s.resize(num_Threads);

					for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
					{
						b1_Up_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);
						b2_Up_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);
						b3_Up_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);
						b4_Up_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);

						b1_Low_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);
						b2_Low_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);
						b3_Low_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);
						b4_Low_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);
					}
				}

				// p-polarization
				if(has_P_Pol)
				{
					b1_Up_Boundary_p.resize(num_Threads);
					b2_Up_Boundary_p.resize(num_Threads);
					b3_Up_Boundary_p.resize(num_Threads);
					b4_Up_Boundary_p.resize(num_Threads);

					b1_Low_Boundary_p.resize(num_Threads);
					b2_Low_Boundary_p.resize(num_Threads);
					b3_Low_Boundary_p.resize(num_Threads);
					b4_Low_Boundary_p.resize(num_Threads);

					for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
					{
						b1_Up_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);
						b2_Up_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);
						b3_Up_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);
						b4_Up_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);

						b1_Low_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);
						b2_Low_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);
						b3_Low_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);
						b4_Low_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);
					}
				}
			}

			/// PT
			if( multilayer->imperfections_Model.approximation == PT_approximation )
			{
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
					intensity_Term_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);
					intensity_Term_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);
					field_Term_Boundary_s	 [thread_Index].resize(num_Boundaries_Sharp);
					field_Term_Boundary_p	 [thread_Index].resize(num_Boundaries_Sharp);
					half_Sum_Field_Term_s	 [thread_Index].resize(num_Boundaries_Sharp);
					half_Sum_Field_Term_p	 [thread_Index].resize(num_Boundaries_Sharp);
					cross_Exp_Factor_2D		 [thread_Index].resize(num_Boundaries_Sharp);
					PSD_Factor_Boundary		 [thread_Index].resize(num_Boundaries_Sharp);

					if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation)
					{
						for(int boundary_Index = 0; boundary_Index<num_Boundaries_Sharp; boundary_Index++)
						{
							half_Sum_Field_Term_s[thread_Index][boundary_Index].resize(num_Boundaries_Sharp-1);
							half_Sum_Field_Term_p[thread_Index][boundary_Index].resize(num_Boundaries_Sharp-1);
							cross_Exp_Factor_2D	 [thread_Index][boundary_Index].resize(num_Boundaries_Sharp-1);
						}
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
						hermites.resize(DWBA_n_Max_Series-1);
						for(int n=0; n<DWBA_n_Max_Series-1; n++)
						{
							hermites[n] = hermite(n, 0);
						}
					}

					// factorial tabulation
					factorial.resize(DWBA_n_Max_Series+1);
					factorial[0] = 1;
					for(int n=1; n<=DWBA_n_Max_Series; n++)
					{
						factorial[n] = factorial[n-1] * 2*n;
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

					integrator_Vec.resize(num_Threads);

					pre_Fourier_Factor.resize(num_Threads);
					incoherent_Diagonal_Term.resize(num_Threads);

					for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
					{
						k1_Up_Boundary[thread_Index].resize(num_Boundaries_Sharp);
						k2_Up_Boundary[thread_Index].resize(num_Boundaries_Sharp);
						k3_Up_Boundary[thread_Index].resize(num_Boundaries_Sharp);
						k4_Up_Boundary[thread_Index].resize(num_Boundaries_Sharp);

						k1_Low_Boundary[thread_Index].resize(num_Boundaries_Sharp);
						k2_Low_Boundary[thread_Index].resize(num_Boundaries_Sharp);
						k3_Low_Boundary[thread_Index].resize(num_Boundaries_Sharp);
						k4_Low_Boundary[thread_Index].resize(num_Boundaries_Sharp);

						D1_Up[thread_Index].resize(DWBA_n_Max_Series);
						D2_Up[thread_Index].resize(DWBA_n_Max_Series);
						D3_Up[thread_Index].resize(DWBA_n_Max_Series);
						D4_Up[thread_Index].resize(DWBA_n_Max_Series);

						D1_Low[thread_Index].resize(DWBA_n_Max_Series);
						D2_Low[thread_Index].resize(DWBA_n_Max_Series);
						D3_Low[thread_Index].resize(DWBA_n_Max_Series);
						D4_Low[thread_Index].resize(DWBA_n_Max_Series);

						pre_Fourier_Factor[thread_Index].resize(DWBA_n_Max_Series);
					}

					// s-polarization
					if(has_S_Pol)
					{
						K_Factor_Boundary_s.resize(num_Threads);

						for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
						{
							K_Factor_Boundary_s[thread_Index].resize(num_Boundaries_Sharp);

							for(int bound=0; bound<num_Boundaries_Sharp; bound++)
							{
								K_Factor_Boundary_s[thread_Index][bound].resize(DWBA_n_Max_Series);
							}
						}
					}

					// p-polarization
					if(has_P_Pol)
					{
						K_Factor_Boundary_p.resize(num_Threads);

						for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
						{
							K_Factor_Boundary_p[thread_Index].resize(num_Boundaries_Sharp);

							for(int bound=0; bound<num_Boundaries_Sharp; bound++)
							{
								K_Factor_Boundary_p[thread_Index][bound].resize(DWBA_n_Max_Series);
							}
						}
					}
				}

			// particles
			if(multilayer->imperfections_Model.use_Particles)
			{
				C_03.resize(num_Threads);
				C_03_03.resize(num_Threads);
				C_03_norm.resize(num_Threads);

				exp_03.resize(num_Threads);
				k_03.resize(num_Threads);

				prefactor_2D_Func_Vec.resize(num_Threads);
				q_Factor_2D_Func_Vec.resize(num_Threads);
				form_Factor_2D_Func_Vec.resize(num_Threads);

				F_03.resize(num_Threads);
				F_pre_03.resize(num_Threads);
				w_03.resize(num_Threads);
				g_03_03.resize(num_Threads);

				alfa_03_03.resize(num_Threads);
				alfa_nn_03.resize(num_Threads);

				coherent_Field_Factor.resize(num_Threads);
				complex_Coef.resize(num_Threads);
				layer_Field_Factor.resize(num_Threads);
				layer_Field_Factor_Short.resize(num_Threads);

				spline_F_03_Real.resize(num_Threads);
				spline_F_03_Imag.resize(num_Threads);
				acc_F_03_Real.resize(num_Threads);
				acc_F_03_Imag.resize(num_Threads);

				for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
				{
					C_03[thread_Index].resize(num_Layers_Sharp);
					C_03_03[thread_Index].resize(num_Layers_Sharp);
					C_03_norm[thread_Index].resize(num_Layers_Sharp);

					exp_03[thread_Index].resize(num_Layers_Sharp);

					for(int layer_Index=0; layer_Index<num_Layers_Sharp; layer_Index++)
					{
						C_03[thread_Index][layer_Index].resize(4);
						C_03_03[thread_Index][layer_Index].resize(4);
						C_03_norm[thread_Index][layer_Index].resize(4);

						for(int i=0; i<4; i++)
						{
							C_03_03[thread_Index][layer_Index][i].resize(4);
						}

						exp_03[thread_Index][layer_Index].resize(4);
					}

					int num_Layer_Items = short_Flat_Calc_Tree.size()-1;
					k_03[thread_Index].resize(num_Layer_Items);
					prefactor_2D_Func_Vec[thread_Index].resize(num_Layer_Items);
					q_Factor_2D_Func_Vec[thread_Index].resize(num_Layer_Items);
					form_Factor_2D_Func_Vec[thread_Index].resize(num_Layer_Items);
					F_03[thread_Index].resize(num_Layer_Items);
					F_pre_03[thread_Index].resize(num_Layer_Items);
					w_03[thread_Index].resize(num_Layer_Items);
					g_03_03[thread_Index].resize(num_Layer_Items);

					alfa_03_03[thread_Index].resize(num_Layer_Items);
					alfa_nn_03[thread_Index].resize(num_Layer_Items);

					complex_Coef[thread_Index].resize(4);
					if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
					{
						if(multilayer->imperfections_Model.particle_Vertical_Correlation == full_Correlation)
						{
							coherent_Field_Factor[thread_Index].resize(abs(int(measurement.end_Phi_Number)-int(measurement.start_Phi_Index)));
						}
						if(multilayer->imperfections_Model.particle_Vertical_Correlation == partial_Correlation )
						{
							layer_Field_Factor[thread_Index].resize(abs(int(measurement.end_Phi_Number)-int(measurement.start_Phi_Index)));
							for(int phi_Local_Index=0; phi_Local_Index<layer_Field_Factor[thread_Index].size(); phi_Local_Index++)
							{
								layer_Field_Factor[thread_Index][phi_Local_Index].resize(num_Layers);
							}
						}
					}
					if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
						measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
						measurement.measurement_Type == measurement_Types[Offset_Scan] )
					{
						if(multilayer->imperfections_Model.particle_Vertical_Correlation == partial_Correlation )
						{
							layer_Field_Factor_Short[thread_Index].resize(num_Layers);
						}
					}

					spline_F_03_Real[thread_Index].resize(num_Layer_Items);
					spline_F_03_Imag[thread_Index].resize(num_Layer_Items);
					acc_F_03_Real[thread_Index].resize(num_Layer_Items);
					acc_F_03_Imag[thread_Index].resize(num_Layer_Items);

					for(int item_Index=0; item_Index<num_Layer_Items; item_Index++)
					{
						k_03[thread_Index][item_Index].resize(4);
						F_03[thread_Index][item_Index].resize(4);
						F_pre_03[thread_Index][item_Index].resize(4);
						w_03[thread_Index][item_Index].resize(4);
						g_03_03[thread_Index][item_Index].resize(4);

						alfa_03_03[thread_Index][item_Index].resize(4);
						alfa_nn_03[thread_Index][item_Index].resize(4);

						spline_F_03_Real[thread_Index][item_Index].resize(4);
						spline_F_03_Imag[thread_Index][item_Index].resize(4);
						acc_F_03_Real[thread_Index][item_Index].resize(4);
						acc_F_03_Imag[thread_Index][item_Index].resize(4);

						for(int i=0; i<4; i++)
						{
							g_03_03[thread_Index][item_Index][i].resize(4);
							alfa_03_03[thread_Index][item_Index][i].resize(4);

							acc_F_03_Real[thread_Index][item_Index][i] = gsl_interp_accel_alloc();
							acc_F_03_Imag[thread_Index][item_Index][i] = gsl_interp_accel_alloc();

							spline_F_03_Real[thread_Index][item_Index][i] = gsl_spline_alloc(gsl_interp_steffen, q_Spline_Points); // plus zero + two last points
							spline_F_03_Imag[thread_Index][item_Index][i] = gsl_spline_alloc(gsl_interp_steffen, q_Spline_Points);
						}
					}
				}
				delete_Particles_F_Spline = true;

				// s-polarization
				if(has_S_Pol)
				{
					C_03_s.resize(num_Threads);
					C_03_03_s.resize(num_Threads);
					C_03_s_norm.resize(num_Threads);

					for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
					{
						C_03_s[thread_Index].resize(num_Layers_Sharp);
						C_03_03_s[thread_Index].resize(num_Layers_Sharp);
						C_03_s_norm[thread_Index].resize(num_Layers_Sharp);

						for(int layer_Index=0; layer_Index<num_Layers_Sharp; layer_Index++)
						{
							C_03_s[thread_Index][layer_Index].resize(4);
							C_03_03_s[thread_Index][layer_Index].resize(4);
							C_03_s_norm[thread_Index][layer_Index].resize(4);

							for(int i=0; i<4; i++)
							{
								C_03_03_s[thread_Index][layer_Index][i].resize(4);
							}
						}
					}
				}

				// p-polarization
				if(has_P_Pol)
				{
					C_03_p.resize(num_Threads);
					C_03_03_p.resize(num_Threads);
					C_03_p_norm.resize(num_Threads);

					for(int thread_Index=0; thread_Index<num_Threads; thread_Index++)
					{
						C_03_p[thread_Index].resize(num_Layers_Sharp);
						C_03_03_p[thread_Index].resize(num_Layers_Sharp);
						C_03_p_norm[thread_Index].resize(num_Layers_Sharp);

						for(int layer_Index=0; layer_Index<num_Layers_Sharp; layer_Index++)
						{
							C_03_p[thread_Index][layer_Index].resize(4);
							C_03_03_p[thread_Index][layer_Index].resize(4);
							C_03_p_norm[thread_Index][layer_Index].resize(4);

							for(int i=0; i<4; i++)
							{
								C_03_03_p[thread_Index][layer_Index][i].resize(4);
							}
						}
					}
				}
			}
		}

		// for getting common PSD parameters
		substrate_Node = media_Node_Map_Vector.back();
		substrate = substrate_Node->struct_Data;
	}
}

Unwrapped_Reflection::~Unwrapped_Reflection()
{
	if(	delete_GISAXS_Spline )
	{
		for(int thread_Index = 0; thread_Index<num_Threads; thread_Index++)
		{
			// GISAXS
			gsl_spline_free(spline_Vec[thread_Index]);
			gsl_interp_accel_free(acc_Vec[thread_Index]);
		}
	}
	if(	delete_Particles_F_Spline )
	{
		for(int thread_Index = 0; thread_Index<num_Threads; thread_Index++)
		{
			// particles
			for(int item_Index=0; item_Index<short_Flat_Calc_Tree.size()-1; item_Index++)
			{
				for(int i=0; i<4; i++)
				{
					gsl_spline_free(spline_F_03_Real[thread_Index][item_Index][i]);
					gsl_spline_free(spline_F_03_Imag[thread_Index][item_Index][i]);
					gsl_interp_accel_free(acc_F_03_Real[thread_Index][item_Index][i]);
					gsl_interp_accel_free(acc_F_03_Imag[thread_Index][item_Index][i]);
				}
			}
		}
	}
}

void Unwrapped_Reflection::fill_Components_From_Node_Vector(int thread_Index, int point_Index)
{
	// if have s-polarization
	if(has_S_Pol)
	{
		for(int boundary_Index = 0; boundary_Index<num_Media_Sharp-1; boundary_Index++)
		{
			Node* node = media_Node_Map_Vector[boundary_Index+1];
			r_Fresnel_s[thread_Index][boundary_Index] = node->Fresnel_R_s[point_Index] * node->weak_Factor_R[point_Index];
			t_Fresnel_s[thread_Index][boundary_Index] = node->Fresnel_T_s[point_Index] * node->weak_Factor_T[point_Index];
		}
	}

	// if have p-polarization
	if (has_P_Pol)
	{
		for(int boundary_Index = 0; boundary_Index<num_Media_Sharp-1; boundary_Index++)
		{
			Node* node = media_Node_Map_Vector[boundary_Index+1];
			r_Fresnel_p[thread_Index][boundary_Index] = node->Fresnel_R_p[point_Index] * node->weak_Factor_R[point_Index];
			t_Fresnel_p[thread_Index][boundary_Index] = node->Fresnel_T_p[point_Index] * node->weak_Factor_T[point_Index];
		}
	}

	// anyway

	/// Ambient
	hi[point_Index].front() = media_Node_Map_Vector.front()->hi[point_Index];

	/// Layers
	for(int media_Index = 1; media_Index<num_Media_Sharp-1; media_Index++)
	{
		Node* node = media_Node_Map_Vector[media_Index];
		hi         [point_Index ][media_Index  ] = node->hi         [point_Index];
		exponenta  [point_Index ][media_Index-1] = node->exponenta  [point_Index];
		exponenta_2[thread_Index][media_Index-1] = node->exponenta_2[point_Index];
	}
	exponenta  [point_Index ][num_Boundaries_Sharp-1] = 1.;
	exponenta_2[thread_Index][num_Boundaries_Sharp-1] = 1.;

	/// Substrate
	hi[point_Index].back() = media_Node_Map_Vector.back()->hi[point_Index];
}

void Unwrapped_Reflection::fill_DW_Factors_From_Node_Vector(int thread_Index, int point_Index)
{
	if(measurement.measurement_Type == measurement_Types[Specular_Scan])
	{	
		if(multilayer->imperfections_Model.use_Common_Roughness_Function || multilayer->imperfections_Model.vertical_Correlation == partial_Correlation)
		{
			Node* node_Substrate = short_Flat_Calc_Tree.back();
			specular_Debye_Waller_Weak_Factor_R_Final[thread_Index] = node_Substrate->specular_Debye_Waller_Weak_Factor_R[point_Index];
		} else
		{
			for (size_t item_Index = 0; item_Index<short_Flat_Calc_Tree.size(); item_Index++)
			{
				Node* node = short_Flat_Calc_Tree[item_Index];
				for(size_t similar_Boundaries=0; similar_Boundaries<boundaries_Of_Item_Vec[item_Index].size(); similar_Boundaries++)
				{
					specular_Debye_Waller_Weak_Factor_R_Final[thread_Index] += node->specular_Debye_Waller_Weak_Factor_R[point_Index];
				}
			}
			specular_Debye_Waller_Weak_Factor_R_Final[thread_Index] /= num_Boundaries_Sharp; // average DW
		}
	} else
	{
		if(spec_Scat_mode == SPECULAR_MODE)
		{
			double angle_Theta_0;
			double k = measurement.k_Value;

			if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				measurement.measurement_Type == measurement_Types[GISAS_Map] )
			{
				angle_Theta_0 = measurement.beam_Theta_0_Angle_Value;
			}
			if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				measurement.measurement_Type == measurement_Types[Offset_Scan] )
			{
				angle_Theta_0 = measurement.beam_Theta_0_Angle_Vec[point_Index];
			}
			double hi = k*sin(qDegreesToRadians(angle_Theta_0));

			if(multilayer->imperfections_Model.use_Common_Roughness_Function || multilayer->imperfections_Model.vertical_Correlation == partial_Correlation)
			{
				double sigma = unwrapped_Structure->sigma_Roughness_Threaded[thread_Index].back();				
				specular_Debye_Waller_Weak_Factor_R_Final[thread_Index] = exp( - 4. * hi*hi * sigma*sigma );
			} else
			{
				for(int boundary_Index = 0; boundary_Index<num_Boundaries_Sharp; boundary_Index++)
				{
					double sigma = unwrapped_Structure->sigma_Roughness_Threaded[thread_Index][boundary_Index];
					specular_Debye_Waller_Weak_Factor_R_Final[thread_Index] += exp( - 4. * hi*hi * sigma*sigma );
				}
				specular_Debye_Waller_Weak_Factor_R_Final[thread_Index] /= num_Boundaries_Sharp;
			}
		}
	}
}

void Unwrapped_Reflection::fill_Item_Id_Map()
{
	id_Item_Map.clear();
	appropriate_Item_Vec.clear();
	used_Appropriate_Item_Index_Vec.clear();
	for(int item_Index = 0; item_Index<short_Flat_Calc_Tree.size(); item_Index++)
	{
		const Data& item = short_Flat_Calc_Tree[item_Index]->struct_Data;
		id_Item_Map.insert(item.id, item_Index);
		appropriate_Item_Vec.push_back(item);
		if(item.particles_Model.is_Used)
		{
			used_Appropriate_Item_Index_Vec.insert(used_Appropriate_Item_Index_Vec.begin(), item_Index);
		}
	}
	boundaries_Of_Item_Vec.resize(short_Flat_Calc_Tree.size());
}

inline void Unwrapped_Reflection::fill_Item_PSD_1D(int thread_Index, int point_Index, double cos_Theta_0)
{
	for(size_t item_Index = 0; item_Index<short_Flat_Calc_Tree.size(); item_Index++)
	{
		Node* node = short_Flat_Calc_Tree[item_Index];
		Data& item = appropriate_Item_Vec[item_Index];
		choose_PSD_1D_Function(item, thread_Index);

		double value = PSD_Gauss_Peak_1D_Func_Vec[thread_Index](1,
																item.roughness_Model.peak_Frequency.value,
																item.roughness_Model.peak_Frequency_Width.value,
																measurement.k_Value,
																measurement.detector_Theta_Cos_Vec[point_Index],
																cos_Theta_0,
																node->spline_PSD_Peak,
																node->acc_PSD_Peak);

		if(substrate_Node->spline_PSD_Combined_1D_Condition(multilayer->imperfections_Model))
		{
			double p = measurement.k_Value*abs(measurement.detector_Theta_Cos_Vec[point_Index] - cos_Theta_0)/(2*M_PI);
			value += gsl_spline_eval(substrate_Node->spline_PSD_Combined_1D, p, substrate_Node->acc_PSD_Combined_1D);
		} else
		{
			if(multilayer->imperfections_Model.add_Measured_PSD_1D && multilayer->imperfections_Model.PSD_1D.is_Loaded())
			{
				double nu = measurement.k_Value*abs(measurement.detector_Theta_Cos_Vec[point_Index] - cos_Theta_0)/(2*M_PI);
				double nu_Start = multilayer->imperfections_Model.PSD_1D.argument.front();
				double nu_End   = multilayer->imperfections_Model.PSD_1D.argument.back();

				// without nu_Limit here
				if(nu<=nu_Start || nu>=nu_End )
				{
					value += PSD_1D_Func_Vec[thread_Index]( item.main_PSD_1D_factor,
															item.roughness_Model.cor_radius.value,
															item.roughness_Model.fractal_alpha.value,
															measurement.k_Value,
															measurement.detector_Theta_Cos_Vec[point_Index],
															cos_Theta_0,
															node->spline_PSD_FG_1D,
															node->acc_PSD_FG_1D);
				} else
				{
					value += Global_Variables::PSD_Measured_1D( substrate.main_PSD_1D_factor,
																substrate.roughness_Model.cor_radius.value,
																substrate.roughness_Model.fractal_alpha.value,
																measurement.k_Value,
																measurement.detector_Theta_Cos_Vec[point_Index],
																cos_Theta_0,
																substrate_Node->spline_PSD_Meas_1D,
																substrate_Node->acc_PSD_Meas_1D);
				}
			} else
			{
				value += PSD_1D_Func_Vec[thread_Index]( item.main_PSD_1D_factor,
														item.roughness_Model.cor_radius.value,
														item.roughness_Model.fractal_alpha.value,
														measurement.k_Value,
														measurement.detector_Theta_Cos_Vec[point_Index],
														cos_Theta_0,
														node->spline_PSD_FG_1D,
														node->acc_PSD_FG_1D);
			}
		}

		PSD_Factor_Item[thread_Index][item_Index] = max(value,0.);
	}
}

inline void Unwrapped_Reflection::fill_Item_PSD_2D(int thread_Index, int point_Index, int phi_Index)
{
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size(); item_Index++)
	{
		Node* node = short_Flat_Calc_Tree[item_Index];
		Data& item = appropriate_Item_Vec[item_Index];
		choose_PSD_2D_Function(item, thread_Index);
//		double value = PSD_2D_Func_Vec[thread_Index](item.main_PSD_2D_factor,
//		                                             item.roughness_Model.cor_radius.value,
//		                                             item.roughness_Model.fractal_alpha.value,
//		                                             measurement.k_Value,
//		                                             measurement.detector_Theta_Cos_Vec[point_Index],
//		                                             measurement.beam_Theta_0_Cos_Value,
//													 measurement.detector_Phi_Cos_Vec[phi_Index],
//													 node->spline_PSD_FG_2D,
//													 node->acc_PSD_FG_2D)

//		  + PSD_Gauss_Peak_2D_Func_Vec[thread_Index](item.PSD_Gauss_Peak_2D_Factor,
//													 item.roughness_Model.peak_Frequency.value,
//													 item.roughness_Model.peak_Frequency_Width.value,
//													 measurement.k_Value,
//													 measurement.detector_Theta_Cos_Vec[point_Index],
//													 measurement.beam_Theta_0_Cos_Value,
//													 measurement.detector_Phi_Cos_Vec[phi_Index],
//													 node->spline_PSD_Peak,
//													 node->acc_PSD_Peak);

		double value = PSD_Gauss_Peak_2D_Func_Vec[thread_Index](item.PSD_Gauss_Peak_2D_Factor,
																item.roughness_Model.peak_Frequency.value,
																item.roughness_Model.peak_Frequency_Width.value,
																measurement.k_Value,
																measurement.detector_Theta_Cos_Vec[point_Index],
																measurement.beam_Theta_0_Cos_Value,
																measurement.detector_Phi_Cos_Vec[phi_Index],
																node->spline_PSD_Peak,
																node->acc_PSD_Peak);

		if(multilayer->imperfections_Model.add_Measured_PSD_2D && multilayer->imperfections_Model.PSD_2D.is_Loaded())
		{
			double nu = measurement.k_Value*sqrt(measurement.detector_Theta_Cos_Vec[point_Index]*measurement.detector_Theta_Cos_Vec[point_Index] + measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value - 2*measurement.beam_Theta_0_Cos_Value*measurement.detector_Theta_Cos_Vec[point_Index]*measurement.detector_Phi_Cos_Vec[phi_Index])/(2*M_PI); // here we need real frequency

			double nu_Start = multilayer->imperfections_Model.PSD_2D.argument.front();
			double nu_End   = multilayer->imperfections_Model.PSD_2D.argument.back();

			// without nu_Limit here
			if(nu<=nu_Start || nu>=nu_End )
			{
				value += PSD_2D_Func_Vec[thread_Index]( substrate.main_PSD_2D_factor,
														substrate.roughness_Model.cor_radius.value,
														substrate.roughness_Model.fractal_alpha.value,
														measurement.k_Value,
														measurement.detector_Theta_Cos_Vec[point_Index],
														measurement.beam_Theta_0_Cos_Value,
														measurement.detector_Phi_Cos_Vec[phi_Index],
														substrate_Node->spline_PSD_FG_2D,
														substrate_Node->acc_PSD_FG_2D);
			} else
			{
				value += Global_Variables::PSD_Measured_2D( substrate.main_PSD_2D_factor,
															substrate.roughness_Model.cor_radius.value,
															substrate.roughness_Model.fractal_alpha.value,
															measurement.k_Value,
															measurement.detector_Theta_Cos_Vec[point_Index],
															measurement.beam_Theta_0_Cos_Value,
															measurement.detector_Phi_Cos_Vec[phi_Index],
															substrate_Node->spline_PSD_Meas_2D,
															substrate_Node->acc_PSD_Meas_2D);
			}
		} else
		{
			value += PSD_2D_Func_Vec[thread_Index]( substrate.main_PSD_2D_factor,
													substrate.roughness_Model.cor_radius.value,
													substrate.roughness_Model.fractal_alpha.value,
													measurement.k_Value,
													measurement.detector_Theta_Cos_Vec[point_Index],
													measurement.beam_Theta_0_Cos_Value,
													measurement.detector_Phi_Cos_Vec[phi_Index],
													substrate_Node->spline_PSD_FG_2D,
													substrate_Node->acc_PSD_FG_2D);
		}

		PSD_Factor_Item[thread_Index][item_Index] = value;
	}
}

void Unwrapped_Reflection::fill_Boundary_Item()
{	
	// media_Data_Map_Vector[0] is Ambient, so we skip it
	for(size_t boundary_Index=0; boundary_Index<media_Data_Map_Vector.size()-1; ++boundary_Index)
	{
		const Data& struct_Data = media_Data_Map_Vector[boundary_Index+1];

		int index_Of_Item = -2021;
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			index_Of_Item = id_Item_Map.value(struct_Data.aperiodic_Avatar_Id);
		} else
		{
			index_Of_Item = id_Item_Map.value(struct_Data.id);
		}
		boundary_Item_Vec[boundary_Index] = index_Of_Item;
		boundaries_Of_Item_Vec[index_Of_Item].push_back(boundary_Index);
	}
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
	double norm, a = M_PI/sqrt(M_PI*M_PI - 8.);
//	complex<double> s_r, factor_r, x_r, y_r, six_r, siy_r;
//	complex<double> s_t, factor_t, x_t, y_t, six_t, siy_t;
	double s_r, factor_r, x_r, y_r, six_r, siy_r;
	double s_t, factor_t, x_t, y_t, six_t, siy_t;
	double my_Sigma_Diffuse = 0;
	for (int i = 0; i < num_Boundaries_Sharp; ++i)
	{
		weak_Factor_R[thread_Index][i] = 0;
		weak_Factor_T[thread_Index][i] = 0;

		if(unwrapped_Structure->enabled_Interlayer[i] && (abs(unwrapped_Structure->sigma_Diffuse[i]) > DBL_MIN))
		{
			my_Sigma_Diffuse = unwrapped_Structure->sigma_Diffuse[i];	// by default, otherwise we change it
			norm = 0;
//			s_r = sqrt(hi[point_Index][i]*hi[point_Index][i+1]);
//			s_t =     (hi[point_Index][i]-hi[point_Index][i+1])/2.;
			s_r = sqrt(real(hi[point_Index][i])*real(hi[point_Index][i+1]));
			s_t =      real(hi[point_Index][i] -     hi[point_Index][i+1])/2.;

			//-------------------------------------------------------------------------------
			// erf interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Erf].enabled &&
			   unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Erf].interlayer.value > DBL_MIN)
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
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Lin].enabled &&
			   unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Lin].interlayer.value > DBL_MIN)
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
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Exp].enabled &&
			   unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Exp].interlayer.value > DBL_MIN)
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
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Tanh].enabled &&
			   unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Tanh].interlayer.value > DBL_MIN)
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
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Sin].enabled &&
			   unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Sin].interlayer.value > DBL_MIN)
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
			if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Step].enabled &&
			   unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][i][Step].interlayer.value > DBL_MIN)
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
				// NaN checking
				if(isnan(real(weak_Factor_R[thread_Index][i])) || isnan(imag(weak_Factor_R[thread_Index][i]))) {weak_Factor_R[thread_Index][i] = 0;}
				if(isnan(real(weak_Factor_T[thread_Index][i])) || isnan(imag(weak_Factor_T[thread_Index][i]))) {weak_Factor_T[thread_Index][i] = 1;}

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
	if(has_S_Pol)
	{
		// reflectance only
		if( calc_Functions.if_Reflectance_Only() )
		{
			for (int i = 0; i < num_Boundaries; ++i)
			{
				if ( norm(hi[point_Index][i] + hi[point_Index][i+1]) > DBL_MIN )
				{
					r_Fresnel_s[thread_Index][i] = (hi[point_Index][i] - hi[point_Index][i+1]) /
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
				if ( norm(hi[point_Index][i] + hi[point_Index][i+1]) > DBL_MIN )
				{
					// reflectance
					r_Fresnel_s[thread_Index][i] = (hi[point_Index][i] - hi[point_Index][i+1]) /
												   (hi[point_Index][i] + hi[point_Index][i+1]);
					// transmittance
					t_Fresnel_s[thread_Index][i] = 2.*hi[point_Index][i] / (hi[point_Index][i] + hi[point_Index][i+1]);
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
	if(has_P_Pol)
	{
		complex<double> hi_je, hi_j1e;
		// reflectance only
		if( calc_Functions.if_Reflectance_Only() )
		{
			for (int i = 0; i < num_Boundaries; ++i)
			{
				hi_je  = hi[point_Index][i  ]/epsilon_Vector[i];
				hi_j1e = hi[point_Index][i+1]/epsilon_Vector[i+1];

				if ( norm(hi_je+hi_j1e) > DBL_MIN )
				{
					r_Fresnel_p[thread_Index][i] = (hi_je-hi_j1e)/ (hi_je+hi_j1e);
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

				if ( norm(hi_je+hi_j1e) > DBL_MIN )
				{
					// reflectance
					r_Fresnel_p[thread_Index][i] = (hi_je-hi_j1e)/ (hi_je+hi_j1e);
					// transmittance
					t_Fresnel_p[thread_Index][i] = 2.*hi_je/ (hi_je+hi_j1e);
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
		exponenta_2[thread_Index][num_Layers] = 1.;

	} else
	// reflectance and transmittance
	{
		for (int i = 0; i < num_Layers; ++i)
		{
			exponenta[point_Index][i] = exp(I*hi[point_Index][i+1]*thickness[i]);
			exponenta_2[thread_Index][i] = pow(exponenta[point_Index][i],2);
		}
		exponenta  [point_Index ][num_Layers] = 1.;
		exponenta_2[thread_Index][num_Layers] = 1.;
	}
}

void Unwrapped_Reflection::calc_Local(int thread_Index, int point_Index)
{
	// s-polarization
	if(has_S_Pol)
	{
		// reflectance only
		if( calc_Functions.if_Reflectance_Only() )
		{
			r_Local_s[thread_Index].back() = r_Fresnel_s[thread_Index].back();	// last boundary
			for (int i = num_Layers-1; i >= 0; --i)
			{
				r_Exp_s[thread_Index][i] = r_Local_s[thread_Index][i+1]*exponenta_2[thread_Index][i]+I*10.*DBL_MIN;
				r_Local_s[thread_Index][i] = (r_Fresnel_s[thread_Index][i] + r_Exp_s[thread_Index][i]) / (1. + r_Fresnel_s[thread_Index][i]*r_Exp_s[thread_Index][i]);
			}
		} else
		// reflectance and transmittance
		{
			complex<double> denom;
			r_Local_s[thread_Index].back() = r_Fresnel_s[thread_Index].back();	// last boundary
			t_Local_s[thread_Index].back() = t_Fresnel_s[thread_Index].back();	// last boundary
			for (int i = num_Layers-1; i >= 0; --i)
			{
				r_Exp_s[thread_Index][i] = r_Local_s[thread_Index][i+1]*exponenta_2[thread_Index][i]+I*10.*DBL_MIN;
				denom = 1. + r_Fresnel_s[thread_Index][i]*r_Exp_s[thread_Index][i];
				r_Local_s[thread_Index][i] = (r_Fresnel_s[thread_Index][i] + r_Exp_s[thread_Index][i]) / denom;

				t_Local_s[thread_Index][i] = (t_Local_s[thread_Index][i+1]*t_Fresnel_s[thread_Index][i]*exponenta[point_Index][i]) / denom;
			}
		}
	}
	// p-polarization
	if(has_P_Pol)
	{
		// reflectance only
		if( calc_Functions.if_Reflectance_Only() )
		{
			r_Local_p[thread_Index].back() = r_Fresnel_p[thread_Index].back();	// last boundary
			for (int i = num_Layers-1; i >= 0; --i)
			{
				r_Exp_p[thread_Index][i] = r_Local_p[thread_Index][i+1]*exponenta_2[thread_Index][i];
				r_Local_p[thread_Index][i] = (r_Fresnel_p[thread_Index][i] + r_Exp_p[thread_Index][i]) / (1. + r_Fresnel_p[thread_Index][i]*r_Exp_p[thread_Index][i]);
			}
		} else
		// reflectance and transmittance
		{
			complex<double> denom;
			r_Local_p[thread_Index].back() = r_Fresnel_p[thread_Index].back();	// last boundary
			t_Local_p[thread_Index].back() = t_Fresnel_p[thread_Index].back();	// last boundary
			for (int i = num_Layers-1; i >= 0; --i)
			{
				r_Exp_p[thread_Index][i] = r_Local_p[thread_Index][i+1]*exponenta_2[thread_Index][i];
				denom = 1. + r_Fresnel_p[thread_Index][i]*r_Exp_p[thread_Index][i];
				r_Local_p[thread_Index][i] = (r_Fresnel_p[thread_Index][i] + r_Exp_p[thread_Index][i]) / denom;

				t_Local_p[thread_Index][i] = (t_Local_p[thread_Index][i+1]*t_Fresnel_p[thread_Index][i]*exponenta[point_Index][i]) / denom;
			}
		}
	}
}

void Unwrapped_Reflection::calc_Amplitudes_Field(int thread_Index, int point_Index, QString polarization)
{
	vector<complex<double>>& U_i_s = spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_i_s[point_Index] : calculated_Values.q_U_i_s[point_Index];
	vector<complex<double>>& U_r_s = spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_r_s[point_Index] : calculated_Values.q_U_r_s[point_Index];
	vector<complex<double>>& U_i_p = spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_i_p[point_Index] : calculated_Values.q_U_i_p[point_Index];
	vector<complex<double>>& U_r_p = spec_Scat_mode == SPECULAR_MODE ? calculated_Values.q0_U_r_p[point_Index] : calculated_Values.q_U_r_p[point_Index];

	vector<complex<double>>& U_i = polarization == "s" ? U_i_s : U_i_p;
	vector<complex<double>>& U_r = polarization == "s" ? U_r_s : U_r_p;
	vector<complex<double>>& r_Local   = polarization == "s" ? r_Local_s  [thread_Index] : r_Local_p  [thread_Index];
	vector<complex<double>>& t_Local   = polarization == "s" ? t_Local_s  [thread_Index] : t_Local_p  [thread_Index];
	vector<complex<double>>& r_Exp     = polarization == "s" ? r_Exp_s    [thread_Index] : r_Exp_p    [thread_Index];
	vector<complex<double>>& r_Fresnel = polarization == "s" ? r_Fresnel_s[thread_Index] : r_Fresnel_p[thread_Index];
	vector<complex<double>>& t_Fresnel = polarization == "s" ? t_Fresnel_s[thread_Index] : t_Fresnel_p[thread_Index];

	/// from surface to substrate
//	complex<double> factor = 1;
//	U_i[0] = 1;
//	U_r[0] = r_Local[0];
//	for (int j = 1; j<num_Boundaries; j++)
//	{
//		factor = exponenta[point_Index][j-1]/(1. + r_Exp[j-1]);
//		U_i[j] = (U_i[j-1] + U_r[j-1])*factor;
//		U_r[j] = U_i[j] * r_Local[j];
//	}
//	U_i[num_Boundaries] = U_i[num_Boundaries-1] * t_Local.back();
//	U_r[num_Boundaries] = 0;

	/// from substrate to surface
//	U_i[num_Boundaries] = t_Local.front();
//	U_r[num_Boundaries] = 0;
//	for (int j = 0; j<num_Boundaries; j++)
//	{
//		U_i[j] = U_i[num_Boundaries]/t_Local[j];
//		U_r[j] = U_i[j] * r_Local[j];
//	}

	/// mixed: normally from substrate to surface, but if t==0, from surface to substrate
	complex<double> factor = 1;
	U_i[0] = 1;
	U_r[0] = r_Local[0];
	U_i[num_Boundaries] = t_Local[0];
	U_r[num_Boundaries] = 0;
	for (int j = 1; j<num_Boundaries; j++)
	{
		if(norm(t_Local[j])>0)
		{
			U_i[j] = U_i[num_Boundaries]/t_Local[j];
		} else
		{
			factor = exponenta[point_Index][j-1]/(1. + r_Exp[j-1]);
			U_i[j] = (U_i[j-1] + U_r[j-1])*factor;

//			factor = t_Fresnel[j-1] * exponenta[point_Index][j-1]/(1. + r_Fresnel[j-1]*r_Exp[j-1]); // alternative
//			U_i[j] = U_i[j-1] * factor;																// alternative
		}
		U_r[j] = U_i[j] * r_Local[j];
	}
}

void Unwrapped_Reflection::calc_k_Wavenumber_Up_Low(int thread_Index, int point_Index)
{
	vector<complex<double>>& q0_Hi = if_Single_Beam_Value ? calculated_Values.q0_Hi.front() : calculated_Values.q0_Hi[point_Index];

	for (int boundary_Index = 0; boundary_Index<num_Boundaries_Sharp; boundary_Index++)
	{
		k1_Up_Boundary [thread_Index][boundary_Index] =  q0_Hi[boundary_Index] + calculated_Values.q_Hi[point_Index][boundary_Index];
		k2_Up_Boundary [thread_Index][boundary_Index] =  q0_Hi[boundary_Index] - calculated_Values.q_Hi[point_Index][boundary_Index];
		k3_Up_Boundary [thread_Index][boundary_Index] = -k2_Up_Boundary [thread_Index][boundary_Index];
		k4_Up_Boundary [thread_Index][boundary_Index] = -k1_Up_Boundary [thread_Index][boundary_Index];

		k1_Low_Boundary[thread_Index][boundary_Index] =  q0_Hi[boundary_Index+1] + calculated_Values.q_Hi[point_Index][boundary_Index+1];
		k2_Low_Boundary[thread_Index][boundary_Index] =  q0_Hi[boundary_Index+1] - calculated_Values.q_Hi[point_Index][boundary_Index+1];
		k3_Low_Boundary[thread_Index][boundary_Index] = -k2_Low_Boundary[thread_Index][boundary_Index];
		k4_Low_Boundary[thread_Index][boundary_Index] = -k1_Low_Boundary[thread_Index][boundary_Index];
	}
}

void Unwrapped_Reflection::calc_Field_Up_Low(int thread_Index, int point_Index, QString polarization)
{
	const vector<complex<double>>& q0_U_i_s = if_Single_Beam_Value ? calculated_Values.q0_U_i_s.front() : calculated_Values.q0_U_i_s[point_Index];
	const vector<complex<double>>& q0_U_r_s = if_Single_Beam_Value ? calculated_Values.q0_U_r_s.front() : calculated_Values.q0_U_r_s[point_Index];
	const vector<complex<double>>& q0_U_i_p = if_Single_Beam_Value ? calculated_Values.q0_U_i_p.front() : calculated_Values.q0_U_i_p[point_Index];
	const vector<complex<double>>& q0_U_r_p = if_Single_Beam_Value ? calculated_Values.q0_U_r_p.front() : calculated_Values.q0_U_r_p[point_Index];

	const vector<complex<double>>& q0_U_i = polarization == "s" ? q0_U_i_s : q0_U_i_p;
	const vector<complex<double>>& q0_U_r = polarization == "s" ? q0_U_r_s : q0_U_r_p;
	const vector<complex<double>>& q_U_i  = polarization == "s" ? calculated_Values.q_U_i_s[point_Index] : calculated_Values.q_U_i_p[point_Index];
	const vector<complex<double>>& q_U_r  = polarization == "s" ? calculated_Values.q_U_r_s[point_Index] : calculated_Values.q_U_r_p[point_Index];

	const vector<complex<double>>& q0_Exponenta = if_Single_Beam_Value ? calculated_Values.q0_Exponenta.front() : calculated_Values.q0_Exponenta[point_Index];
	const vector<complex<double>>& q_Exponenta = calculated_Values.q_Exponenta[point_Index];

	vector<complex<double>>& b1_Up_Boundary  = polarization == "s" ? b1_Up_Boundary_s[thread_Index] : b1_Up_Boundary_p[thread_Index];
	vector<complex<double>>& b2_Up_Boundary  = polarization == "s" ? b2_Up_Boundary_s[thread_Index] : b2_Up_Boundary_p[thread_Index];
	vector<complex<double>>& b3_Up_Boundary  = polarization == "s" ? b3_Up_Boundary_s[thread_Index] : b3_Up_Boundary_p[thread_Index];
	vector<complex<double>>& b4_Up_Boundary  = polarization == "s" ? b4_Up_Boundary_s[thread_Index] : b4_Up_Boundary_p[thread_Index];

	vector<complex<double>>& b1_Low_Boundary = polarization == "s" ? b1_Low_Boundary_s[thread_Index] : b1_Low_Boundary_p[thread_Index];
	vector<complex<double>>& b2_Low_Boundary = polarization == "s" ? b2_Low_Boundary_s[thread_Index] : b2_Low_Boundary_p[thread_Index];
	vector<complex<double>>& b3_Low_Boundary = polarization == "s" ? b3_Low_Boundary_s[thread_Index] : b3_Low_Boundary_p[thread_Index];
	vector<complex<double>>& b4_Low_Boundary = polarization == "s" ? b4_Low_Boundary_s[thread_Index] : b4_Low_Boundary_p[thread_Index];

	complex<double> exp_1, exp_2;

	for (int j = 0; j<num_Boundaries_Sharp; j++)
	{
		b1_Up_Boundary[j] = q0_U_i[j] * q_U_i[j];
		b2_Up_Boundary[j] = q0_U_i[j] * q_U_r[j];
		b3_Up_Boundary[j] = q0_U_r[j] * q_U_i[j];
		b4_Up_Boundary[j] = q0_U_r[j] * q_U_r[j];

		exp_1 = q0_Exponenta[j] * q_Exponenta[j];
		exp_2 = q0_Exponenta[j] / q_Exponenta[j];
		b1_Low_Boundary[j] = q0_U_i[j+1] * q_U_i[j+1] / exp_1;
		b2_Low_Boundary[j] = q0_U_i[j+1] * q_U_r[j+1] / exp_2;
		b3_Low_Boundary[j] = q0_U_r[j+1] * q_U_i[j+1] * exp_2;
		b4_Low_Boundary[j] = q0_U_r[j+1] * q_U_r[j+1] * exp_1;
	}
}

void Unwrapped_Reflection::calc_K_Factor_DWBA_SA_CSA(int point_Index, int thread_Index, QString polarization)
{
	Q_UNUSED(point_Index)
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
		for (int boundary_Index = 0; boundary_Index<num_Boundaries_Sharp; boundary_Index++)
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
			for(int n=2; n<=DWBA_n_Max_Series; n++)
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
		for (int boundary_Index = 0; boundary_Index<num_Boundaries_Sharp; boundary_Index++)
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
			for(int n=2; n<=DWBA_n_Max_Series; n++)
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
	const vector<vector<complex<double>>>& K_Factor_Boundary = polarization == "s" ? K_Factor_Boundary_s[thread_Index] : K_Factor_Boundary_p[thread_Index];

	if(multilayer->imperfections_Model.vertical_Correlation == full_Correlation)
	{
		complex<double> coherent_Diagonal_Sum = 0;
		for (int boundary_Index = 0; boundary_Index<num_Boundaries_Sharp; boundary_Index++)
		{
			coherent_Diagonal_Sum += K_Factor_Boundary[boundary_Index][n_Power-1];
		}
		return norm(coherent_Diagonal_Sum);
	}
	if(multilayer->imperfections_Model.vertical_Correlation == zero_Correlation)
	{
		double incoherent_Diagonal_Sum = 0;
		for (size_t item_Index = 0; item_Index<short_Flat_Calc_Tree.size(); item_Index++)
		{
			incoherent_Diagonal_Term[thread_Index][item_Index][n_Power-1] = 0;

			for(size_t similar_Boundaries=0; similar_Boundaries<boundaries_Of_Item_Vec[item_Index].size(); similar_Boundaries++)
			{
				int boundary_Index = boundaries_Of_Item_Vec[item_Index][similar_Boundaries];
				incoherent_Diagonal_Term[thread_Index][item_Index][n_Power-1] += norm(K_Factor_Boundary[boundary_Index][n_Power-1]);
			}
			incoherent_Diagonal_Sum += incoherent_Diagonal_Term[thread_Index][item_Index][n_Power-1];
		}
		return incoherent_Diagonal_Sum;
	}
	return -2020;
}

void Unwrapped_Reflection::choose_Cor_Function(int thread_Index)
{
	if(multilayer->imperfections_Model.PSD_Model == ABC_Model)
	{
		Cor_Func_Vec[thread_Index] = Global_Variables::Cor_ABC;
	} else
    if(multilayer->imperfections_Model.PSD_Model == stretched_exp_Model)
	{
		Cor_Func_Vec[thread_Index] = Global_Variables::Cor_Fractal_Gauss;
	}
}

double Unwrapped_Reflection::function_DWBA_SA_CSA_Batch_Common_Integrand(double r, int thread_Index)
{
	double sigma = substrate.roughness_Model.sigma.value;
	double alpha = substrate.roughness_Model.fractal_alpha.value;
	double xi    = substrate.roughness_Model.cor_radius.value;

	double cor = Cor_Func_Vec[thread_Index](xi, alpha, r);
	double integrand = 0, cor_n_Power = 1;
	for(int n_Power=1; n_Power<=DWBA_n_Max_Series; n_Power++)
	{
		cor_n_Power *= cor;
		integrand += pre_Fourier_Factor[thread_Index][n_Power-1]*cor_n_Power;
	}
	return sigma*sigma * integrand;
}

double Unwrapped_Reflection::function_DWBA_SA_CSA_Batch_Individual_Integrand(double r, int thread_Index)
{
	double integrand = 0, cor_n_Power = 1;
	for(size_t item_Index = 0; item_Index<short_Flat_Calc_Tree.size(); item_Index++)
	{
		const Data& struct_Data = short_Flat_Calc_Tree[item_Index]->struct_Data;
		double sigma = struct_Data.roughness_Model.sigma.value;
		double alpha = struct_Data.roughness_Model.fractal_alpha.value;
		double xi    = struct_Data.roughness_Model.cor_radius.value;

		double cor = Cor_Func_Vec[thread_Index](xi, alpha, r);

		cor_n_Power = 1;
		for(int n_Power=1; n_Power<=DWBA_n_Max_Series; n_Power++)
		{
			cor_n_Power *= cor;
			integrand += sigma*sigma * incoherent_Diagonal_Term[thread_Index][item_Index][n_Power-1]*cor_n_Power / factorial[n_Power];
		}
	}
	return integrand;
}

void Unwrapped_Reflection::calc_Sliced_Field(int thread_Index, int point_Index, const vector<complex<double>>& epsilon_Vector)
{
	if( calc_Functions.check_Field ||
	    calc_Functions.check_Joule )
	{
		if(has_S_Pol) calc_Amplitudes_Field(thread_Index, point_Index, "s");
		if(has_P_Pol) calc_Amplitudes_Field(thread_Index, point_Index, "p");

		// field intensity
		complex<double> U_s, U_p, iChi, e_i, e_r;
		for(int z_Index=0; z_Index<unwrapped_Structure->num_Field_Slices; z_Index++)
		{
			double z = unwrapped_Structure->field_Z_Positions[z_Index];
			int layer_Index = unwrapped_Structure->get_Layer_or_Slice_Index(z);
			int media_Index = layer_Index+1;

			iChi = I*hi[point_Index][media_Index];
			e_i = exp(+iChi*(z-boundaries_Enlarged[media_Index]));
			e_r = 1./e_i;//exp(-iChi*(z-boundaries_Enlarged[media_Index]));

			double field_Value = 0;
			// s-polarization
			if(has_S_Pol)
			{
				U_s = U_i_s[point_Index][media_Index] * e_i + U_r_s[point_Index][media_Index] * e_r;
				field_Value += s_Weight*norm(U_s);
			}
			// p-polarization
			if(has_P_Pol)
			{
				U_p = U_i_p[point_Index][media_Index] * e_i + U_r_p[point_Index][media_Index] * e_r;
				field_Value += p_Weight*norm(U_p);
			}

			calculated_Values.field_Intensity[point_Index][z_Index] = field_Value;
			calculated_Values.absorption_Map [point_Index][z_Index] = field_Value * imag(epsilon_Vector[media_Index]); // consider sigma?
//			Kossel[point_Index] += field_Value*unwrapped_Structure->calc_Functions.field_Step;
		}
	}
}

complex<double> Unwrapped_Reflection::calc_Field_Term_j_Discrete(int point_Index, int j,
																		const vector<complex<double>>& q0_U_i,
																		const vector<complex<double>>& q0_U_r,
																		const vector<complex<double>>& q_U_i,
																		const vector<complex<double>>& q_U_r)
{
	Q_UNUSED(point_Index)
	const vector<Subbounds>& boundary_Subboundary_Map_Vector = unwrapped_Structure->boundary_Subboundary_Map_Vector;
	const vector<complex<double>>& eps = unwrapped_Structure->discretized_Epsilon;

	complex<double> my_Field_Factor = 0;
	size_t b;

	if(boundary_Subboundary_Map_Vector[j].top_Half_Boundary)
	{
		b = boundary_Subboundary_Map_Vector[j].top_Boundary-1;
		my_Field_Factor += 0.5 * (q0_U_i[b]+q0_U_r[b])*(q_U_i[b]+q_U_r[b]) * (eps[b+1]-eps[b]);
	}
	for(size_t b=boundary_Subboundary_Map_Vector[j].top_Boundary; b<=boundary_Subboundary_Map_Vector[j].bottom_Boundary; b++)
	{
		my_Field_Factor += (q0_U_i[b]+q0_U_r[b])*(q_U_i[b]+q_U_r[b]) * (eps[b+1]-eps[b]);
	}
	if(boundary_Subboundary_Map_Vector[j].bottom_Half_Boundary)
	{
		b = boundary_Subboundary_Map_Vector[j].bottom_Boundary+1;
		my_Field_Factor += 0.5 * (q0_U_i[b]+q0_U_r[b])*(q_U_i[b]+q_U_r[b]) * (eps[b+1]-eps[b]);
	}
	return my_Field_Factor;
}

complex<double> Unwrapped_Reflection::calc_Field_Term_j(int point_Index, int j, int thread_Index,
																const vector<complex<double>>& q0_Hi,
																const vector<complex<double>>& q_Hi,

																const vector<complex<double>>& b1_Up_Boundary,
																const vector<complex<double>>& b2_Up_Boundary,
																const vector<complex<double>>& b3_Up_Boundary,
																const vector<complex<double>>& b4_Up_Boundary,

																const vector<complex<double>>& b1_Low_Boundary,
																const vector<complex<double>>& b2_Low_Boundary,
																const vector<complex<double>>& b3_Low_Boundary,
																const vector<complex<double>>& b4_Low_Boundary)
{
	complex<double> k1_Up = q0_Hi[j] + q_Hi[j];
	complex<double> k2_Up = q0_Hi[j] - q_Hi[j];
	complex<double> k3_Up = -k2_Up;
	complex<double> k4_Up = -k1_Up;

	complex<double> k1_Low = q0_Hi[j+1] + q_Hi[j+1];
	complex<double> k2_Low = q0_Hi[j+1] - q_Hi[j+1];
	complex<double> k3_Low = -k2_Low;
	complex<double> k4_Low = -k1_Low;

	const complex<double>& b1_Up = b1_Up_Boundary[j];
	const complex<double>& b2_Up = b2_Up_Boundary[j];
	const complex<double>& b3_Up = b3_Up_Boundary[j];
	const complex<double>& b4_Up = b4_Up_Boundary[j];

	const complex<double>& b1_Low = b1_Low_Boundary[j];
	const complex<double>& b2_Low = b2_Low_Boundary[j];
	const complex<double>& b3_Low = b3_Low_Boundary[j];
	const complex<double>& b4_Low = b4_Low_Boundary[j];

	double sigma_j = unwrapped_Structure->sigma_Diffuse[j];
	complex<double> result = 0, factor;
	double inter_Value, norma = 0, a = 1./sqrt(M_PI*M_PI - 8.);

	{
		// erf
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Erf].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Erf].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Erf].my_Sigma_Diffuse.value; }

			double sigma_j_s2 = sigma_j / M_SQRT2;
			complex<double> exp_Up_1  = exp(-sigma_j_s2*sigma_j_s2 * k1_Up *k1_Up);
			complex<double> exp_Up_2  = exp(-sigma_j_s2*sigma_j_s2 * k2_Up *k2_Up);
			complex<double> exp_Low_1 = exp(-sigma_j_s2*sigma_j_s2 * k1_Low*k1_Low);
			complex<double> exp_Low_2 = exp(-sigma_j_s2*sigma_j_s2 * k2_Low*k2_Low);

			complex<double> erf_Up_1  = Faddeeva::erf( I*sigma_j_s2*k1_Up );
			complex<double> erf_Up_2  = Faddeeva::erf( I*sigma_j_s2*k2_Up );
			complex<double> erf_Low_1 = Faddeeva::erf( I*sigma_j_s2*k1_Low);
			complex<double> erf_Low_2 = Faddeeva::erf( I*sigma_j_s2*k2_Low);

			factor = 0.5*(b1_Up*exp_Up_1*(1.-erf_Up_1) +
						  b2_Up*exp_Up_2*(1.-erf_Up_2) +
						  b3_Up*exp_Up_2*(1.+erf_Up_2) +
						  b4_Up*exp_Up_1*(1.+erf_Up_1) +

						  b1_Low*exp_Low_1*(1.+erf_Low_1) +
						  b2_Low*exp_Low_2*(1.+erf_Low_2) +
						  b3_Low*exp_Low_2*(1.-erf_Low_2) +
						  b4_Low*exp_Low_1*(1.-erf_Low_1)
						 );
			result += inter_Value*factor;
		}
		// lin
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Lin].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Lin].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Lin].my_Sigma_Diffuse.value + 10*DBL_EPSILON; }

			double sqrt_3 = sqrt(3);
			if(norm(k1_Up )<DBL_EPSILON) {k1_Up =DBL_EPSILON; k4_Up =-DBL_EPSILON;}
			if(norm(k2_Up )<DBL_EPSILON) {k2_Up =DBL_EPSILON; k3_Up =-DBL_EPSILON;}
			if(norm(k1_Low)<DBL_EPSILON) {k1_Low=DBL_EPSILON; k4_Low=-DBL_EPSILON;}
			if(norm(k2_Low)<DBL_EPSILON) {k2_Low=DBL_EPSILON; k3_Low=-DBL_EPSILON;}

			complex<double> exp_Up_1  = exp(-I*sqrt_3*sigma_j * k1_Up);
			complex<double> exp_Up_2  = exp(-I*sqrt_3*sigma_j * k2_Up);
			complex<double> exp_Low_1 = exp( I*sqrt_3*sigma_j * k1_Low);
			complex<double> exp_Low_2 = exp( I*sqrt_3*sigma_j * k2_Low);

			factor = I/(2*sqrt_3)*( b1_Up/(sigma_j * k1_Up)*(   exp_Up_1-1.) +
									b2_Up/(sigma_j * k2_Up)*(   exp_Up_2-1.) +
									b3_Up/(sigma_j * k3_Up)*(1./exp_Up_2-1.) +
									b4_Up/(sigma_j * k4_Up)*(1./exp_Up_1-1.) +

									b1_Low/(sigma_j * k1_Low)*(1.-   exp_Low_1) +
									b2_Low/(sigma_j * k2_Low)*(1.-   exp_Low_2) +
									b3_Low/(sigma_j * k3_Low)*(1.-1./exp_Low_2) +
									b4_Low/(sigma_j * k4_Low)*(1.-1./exp_Low_1)
									);
			result += inter_Value*factor;
		}
		// exp
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Exp].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Exp].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Exp].my_Sigma_Diffuse.value; }

			factor = M_SQRT1_2*(b1_Up/(M_SQRT2 + I*sigma_j*k1_Up) +
								b2_Up/(M_SQRT2 + I*sigma_j*k2_Up) +
								b3_Up/(M_SQRT2 + I*sigma_j*k3_Up) +
								b4_Up/(M_SQRT2 + I*sigma_j*k4_Up) +

								b1_Low/(M_SQRT2 - I*sigma_j*k1_Low) +
								b2_Low/(M_SQRT2 - I*sigma_j*k2_Low) +
								b3_Low/(M_SQRT2 - I*sigma_j*k3_Low) +
								b4_Low/(M_SQRT2 - I*sigma_j*k4_Low)
								);
			result += inter_Value*factor;
		}
		// tanh == erf
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Tanh].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Tanh].interlayer.value;
			norma += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Tanh].interlayer.value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Tanh].my_Sigma_Diffuse.value; }

			double sigma_j_s2 = sigma_j / M_SQRT2;
			complex<double> exp_Up_1  = exp(-sigma_j_s2*sigma_j_s2 * k1_Up *k1_Up);
			complex<double> exp_Up_2  = exp(-sigma_j_s2*sigma_j_s2 * k2_Up *k2_Up);
			complex<double> exp_Low_1 = exp(-sigma_j_s2*sigma_j_s2 * k1_Low*k1_Low);
			complex<double> exp_Low_2 = exp(-sigma_j_s2*sigma_j_s2 * k2_Low*k2_Low);

			complex<double> erf_Up_1  = Faddeeva::erf( I*sigma_j_s2*k1_Up );
			complex<double> erf_Up_2  = Faddeeva::erf( I*sigma_j_s2*k2_Up );
			complex<double> erf_Low_1 = Faddeeva::erf( I*sigma_j_s2*k1_Low);
			complex<double> erf_Low_2 = Faddeeva::erf( I*sigma_j_s2*k2_Low);

			factor = 0.5*(b1_Up*exp_Up_1*(1.-erf_Up_1) +
						  b2_Up*exp_Up_2*(1.-erf_Up_2) +
						  b3_Up*exp_Up_2*(1.+erf_Up_2) +
						  b4_Up*exp_Up_1*(1.+erf_Up_1) +

						  b1_Low*exp_Low_1*(1.+erf_Low_1) +
						  b2_Low*exp_Low_2*(1.+erf_Low_2) +
						  b3_Low*exp_Low_2*(1.-erf_Low_2) +
						  b4_Low*exp_Low_1*(1.-erf_Low_1)
						  );
			result += inter_Value*factor;
		}
		// sin
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Sin].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Sin].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Sin].my_Sigma_Diffuse.value; }

			complex<double> exp_Up_1  = exp(-I* M_PI*a * sigma_j * k1_Up);
			complex<double> exp_Up_2  = exp(-I* M_PI*a * sigma_j * k2_Up);
			complex<double> exp_Low_1 = exp( I* M_PI*a * sigma_j * k1_Low);
			complex<double> exp_Low_2 = exp( I* M_PI*a * sigma_j * k2_Low);

			factor = 0.5*(b1_Up/(1. - 4. * a*a * sigma_j*sigma_j * k1_Up*k1_Up)*(1.*exp_Up_1 + 2.*I*a*k1_Up*sigma_j) +
						  b2_Up/(1. - 4. * a*a * sigma_j*sigma_j * k2_Up*k2_Up)*(1.*exp_Up_2 + 2.*I*a*k2_Up*sigma_j) +
						  b3_Up/(1. - 4. * a*a * sigma_j*sigma_j * k3_Up*k3_Up)*(1./exp_Up_2 + 2.*I*a*k3_Up*sigma_j) +
						  b4_Up/(1. - 4. * a*a * sigma_j*sigma_j * k4_Up*k4_Up)*(1./exp_Up_1 + 2.*I*a*k4_Up*sigma_j) +

						  b1_Low/(1. - 4. * a*a * sigma_j*sigma_j * k1_Low*k1_Low)*(1.*exp_Low_1 - 2.*I*a*k1_Low*sigma_j) +
						  b2_Low/(1. - 4. * a*a * sigma_j*sigma_j * k2_Low*k2_Low)*(1.*exp_Low_2 - 2.*I*a*k2_Low*sigma_j) +
						  b3_Low/(1. - 4. * a*a * sigma_j*sigma_j * k3_Low*k3_Low)*(1./exp_Low_2 - 2.*I*a*k3_Low*sigma_j) +
						  b4_Low/(1. - 4. * a*a * sigma_j*sigma_j * k4_Low*k4_Low)*(1./exp_Low_1 - 2.*I*a*k4_Low*sigma_j)
						 );
			result += inter_Value*factor;
		}
		// step
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Step].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Step].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Step].my_Sigma_Diffuse.value; }

			complex<double> exp_Up_1  = exp(-I * sigma_j * k1_Up);
			complex<double> exp_Up_2  = exp(-I * sigma_j * k2_Up);
			complex<double> exp_Low_1 = exp( I * sigma_j * k1_Low);
			complex<double> exp_Low_2 = exp( I * sigma_j * k2_Low);

			factor = 0.5*(b1_Up*exp_Up_1 +
						  b2_Up*exp_Up_2 +
						  b3_Up/exp_Up_2 +
						  b4_Up/exp_Up_1 +

						  b1_Low*exp_Low_1 +
						  b2_Low*exp_Low_2 +
						  b3_Low/exp_Low_2 +
						  b4_Low/exp_Low_1
						 );
			result += inter_Value*factor;
		}

		if( abs(norma) > DBL_MIN )
		{
			result /= norma;
		} else
		{
			result = b1_Up + b2_Up + b3_Up + b4_Up; // up and down are equal if no interlayer
		}
	}
	return result*(unwrapped_Structure->epsilon[j+1]-unwrapped_Structure->epsilon[j]);
}

complex<double> Unwrapped_Reflection::calc_Field_Term_j_Simplified(int point_Index, int j, int thread_Index,
																   const vector<complex<double>>& q0_Hi,
																   const vector<complex<double>>& q_Hi,

																   const vector<complex<double>>& b1_Up_Boundary,
																   const vector<complex<double>>& b2_Up_Boundary,
																   const vector<complex<double>>& b3_Up_Boundary,
																   const vector<complex<double>>& b4_Up_Boundary,

																   const vector<complex<double>>& b1_Low_Boundary,
																   const vector<complex<double>>& b2_Low_Boundary,
																   const vector<complex<double>>& b3_Low_Boundary,
																   const vector<complex<double>>& b4_Low_Boundary)
{
	complex<double> k1 = sqrt((q0_Hi[j] + q_Hi[j])*(q0_Hi[j+1] + q_Hi[j+1]));
	complex<double> k2 = sqrt((q0_Hi[j] - q_Hi[j])*(q0_Hi[j+1] - q_Hi[j+1]));
	complex<double> k3 = -k2;
	complex<double> k4 = -k1;

	const complex<double> b1 = (b1_Up_Boundary[j] + b1_Low_Boundary[j])/2.;
	const complex<double> b2 = (b2_Up_Boundary[j] + b2_Low_Boundary[j])/2.;
	const complex<double> b3 = (b3_Up_Boundary[j] + b3_Low_Boundary[j])/2.;
	const complex<double> b4 = (b4_Up_Boundary[j] + b4_Low_Boundary[j])/2.;

	double sigma_j = unwrapped_Structure->sigma_Diffuse[j];
	complex<double> result = 0, factor;
	double inter_Value, norma = 0, a = 1./sqrt(M_PI*M_PI - 8.);
	{
		// erf
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Erf].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Erf].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Erf].my_Sigma_Diffuse.value; }

			double sigma_j_s2 = sigma_j / M_SQRT2;
			complex<double> exp_1 = exp(-sigma_j_s2*sigma_j_s2 * k1 *k1);
			complex<double> exp_2 = exp(-sigma_j_s2*sigma_j_s2 * k2 *k2);

			factor = (b1*exp_1 +
					  b2*exp_2 +
					  b3*exp_2 +
					  b4*exp_1);

			result += inter_Value*factor;
		}
		// lin
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Lin].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Lin].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Lin].my_Sigma_Diffuse.value + 10*DBL_EPSILON; }

			double sqrt_3 = sqrt(3);
			if(norm(k1)<DBL_EPSILON) {k1=DBL_EPSILON;k4=-DBL_EPSILON;}
			if(norm(k2)<DBL_EPSILON) {k2=DBL_EPSILON;k3=-DBL_EPSILON;}
			complex<double> sin_1 = sin(sqrt_3*sigma_j * k1);
			complex<double> sin_2 = sin(sqrt_3*sigma_j * k2);

			factor = 1./(sqrt_3)*(b1/(sigma_j * k1)*( sin_1) +
								  b2/(sigma_j * k2)*( sin_2) +
								  b3/(sigma_j * k3)*(-sin_2) +
								  b4/(sigma_j * k4)*(-sin_1));

			result += inter_Value*factor;
		}
		// exp
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Exp].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Exp].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Exp].my_Sigma_Diffuse.value; }

			factor = 2.*(b1/(2. + sigma_j*sigma_j*k1*k1) +
						 b2/(2. + sigma_j*sigma_j*k2*k2) +
						 b3/(2. + sigma_j*sigma_j*k3*k3) +
						 b4/(2. + sigma_j*sigma_j*k4*k4));
			result += inter_Value*factor;
		}
		// tanh == erf
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Tanh].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Tanh].interlayer.value;
			norma += unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Tanh].interlayer.value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Tanh].my_Sigma_Diffuse.value; }

			double sigma_j_s2 = sigma_j / M_SQRT2;
			complex<double> exp_1 = exp(-sigma_j_s2*sigma_j_s2 * k1 *k1);
			complex<double> exp_2 = exp(-sigma_j_s2*sigma_j_s2 * k2 *k2);

			factor = (b1*exp_1 +
					  b2*exp_2 +
					  b3*exp_2 +
					  b4*exp_1);

			result += inter_Value*factor;
		}
		// sin
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Sin].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Sin].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Sin].my_Sigma_Diffuse.value; }

			complex<double> cos_1 = cos( M_PI*a * sigma_j * k1);
			complex<double> cos_2 = cos( M_PI*a * sigma_j * k2);

			factor = 1. *(b1/(1. - 4. * a*a * sigma_j*sigma_j * k1*k1)*cos_1 +
						  b2/(1. - 4. * a*a * sigma_j*sigma_j * k2*k2)*cos_2 +
						  b3/(1. - 4. * a*a * sigma_j*sigma_j * k3*k3)*cos_2 +
						  b4/(1. - 4. * a*a * sigma_j*sigma_j * k4*k4)*cos_1);
			result += inter_Value*factor;
		}
		// step
		if(unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Step].enabled)
		{
			inter_Value = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Step].interlayer.value;
			norma += inter_Value;
			if(!unwrapped_Structure->common_Sigma_Diffuse[j]) {
				sigma_j = unwrapped_Structure->boundary_Interlayer_Composition_Threaded[thread_Index][j][Step].my_Sigma_Diffuse.value; }

			complex<double> cos_1  = cos(sigma_j * k1);
			complex<double> cos_2  = cos(sigma_j * k2);

			factor = (b1*cos_1 +
					  b2*cos_2 +
					  b3*cos_2 +
					  b4*cos_1);
			result += inter_Value*factor;
		}

		if( abs(norma) > DBL_MIN )
		{
			result /= norma;
		} else
		{
			result = b1 + b2 + b3 + b4;
		}
	}
	return result*(unwrapped_Structure->epsilon[j+1]-unwrapped_Structure->epsilon[j]);
}

double Unwrapped_Reflection::calc_Field_Term_Sum(QString polarization, int point_Index, int thread_Index)
{
	// used only without discretization
	const vector<complex<double>>& q0_Hi = if_Single_Beam_Value ? calculated_Values.q0_Hi.front() : calculated_Values.q0_Hi[point_Index];
	const vector<complex<double>>& q_Hi = calculated_Values.q_Hi[point_Index];

	vector<complex<double>>& b1_Up_Boundary = polarization == "s" ? b1_Up_Boundary_s[thread_Index] : b1_Up_Boundary_p[thread_Index];
	vector<complex<double>>& b2_Up_Boundary = polarization == "s" ? b2_Up_Boundary_s[thread_Index] : b2_Up_Boundary_p[thread_Index];
	vector<complex<double>>& b3_Up_Boundary = polarization == "s" ? b3_Up_Boundary_s[thread_Index] : b3_Up_Boundary_p[thread_Index];
	vector<complex<double>>& b4_Up_Boundary = polarization == "s" ? b4_Up_Boundary_s[thread_Index] : b4_Up_Boundary_p[thread_Index];

	vector<complex<double>>& b1_Low_Boundary = polarization == "s" ? b1_Low_Boundary_s[thread_Index] : b1_Low_Boundary_p[thread_Index];
	vector<complex<double>>& b2_Low_Boundary = polarization == "s" ? b2_Low_Boundary_s[thread_Index] : b2_Low_Boundary_p[thread_Index];
	vector<complex<double>>& b3_Low_Boundary = polarization == "s" ? b3_Low_Boundary_s[thread_Index] : b3_Low_Boundary_p[thread_Index];
	vector<complex<double>>& b4_Low_Boundary = polarization == "s" ? b4_Low_Boundary_s[thread_Index] : b4_Low_Boundary_p[thread_Index];

	// common
	const vector<complex<double>>& q0_U_i_s = if_Single_Beam_Value ? calculated_Values.q0_U_i_s.front() : calculated_Values.q0_U_i_s[point_Index];
	const vector<complex<double>>& q0_U_r_s = if_Single_Beam_Value ? calculated_Values.q0_U_r_s.front() : calculated_Values.q0_U_r_s[point_Index];
	const vector<complex<double>>& q0_U_i_p = if_Single_Beam_Value ? calculated_Values.q0_U_i_p.front() : calculated_Values.q0_U_i_p[point_Index];
	const vector<complex<double>>& q0_U_r_p = if_Single_Beam_Value ? calculated_Values.q0_U_r_p.front() : calculated_Values.q0_U_r_p[point_Index];

	const vector<complex<double>>& q0_U_i = polarization == "s" ? q0_U_i_s : q0_U_i_p;
	const vector<complex<double>>& q0_U_r = polarization == "s" ? q0_U_r_s : q0_U_r_p;
	const vector<complex<double>>& q_U_i  = polarization == "s" ? calculated_Values.q_U_i_s [point_Index] : calculated_Values.q_U_i_p [point_Index];
	const vector<complex<double>>& q_U_r  = polarization == "s" ? calculated_Values.q_U_r_s [point_Index] : calculated_Values.q_U_r_p [point_Index];

	vector<complex<double>>& field_Term_Boundary     = polarization == "s" ? field_Term_Boundary_s    [thread_Index] : field_Term_Boundary_p    [thread_Index];
	vector<        double> & intensity_Term_Boundary = polarization == "s" ? intensity_Term_Boundary_s[thread_Index] : intensity_Term_Boundary_p[thread_Index];
	vector<vector<double>> & half_Sum_Field_Term	 = polarization == "s" ? half_Sum_Field_Term_s    [thread_Index] : half_Sum_Field_Term_p    [thread_Index];

	/// ------------------------------------------------------
	/// common
	/// ------------------------------------------------------
	double incoherent_Diagonal_Sum = 0;
	complex<double> coherent_Diagonal_Sum = 0;

	if(!multilayer->discretization_Parameters.enable_Discretization)
	{
		for (int j = 0; j<num_Boundaries_Sharp; j++)
		{
			// all fields
			if (unwrapped_Structure->enabled_Interlayer[j] && unwrapped_Structure->sigma_Diffuse[j]>0.01)
			{
//				if(use_Simplified_Scattering_Weak_Factor)
//				{
					field_Term_Boundary[j] = calc_Field_Term_j(point_Index, j, thread_Index, q0_Hi, q_Hi,
																		   b1_Up_Boundary, b2_Up_Boundary, b3_Up_Boundary, b4_Up_Boundary,
																		   b1_Low_Boundary,b2_Low_Boundary,b3_Low_Boundary,b4_Low_Boundary);
//				} else
//				{
//					field_Term_Boundary[j] = calc_Field_Term_j_Simplified(point_Index, j, thread_Index, q0_Hi, q_Hi,
//																		   b1_Up_Boundary, b2_Up_Boundary, b3_Up_Boundary, b4_Up_Boundary,
//																		   b1_Low_Boundary,b2_Low_Boundary,b3_Low_Boundary,b4_Low_Boundary);
//				}
			} else
			{
				field_Term_Boundary[j] = (b1_Up_Boundary[j] + b2_Up_Boundary[j] + b3_Up_Boundary[j] + b4_Up_Boundary[j])*(unwrapped_Structure->epsilon[j+1]-unwrapped_Structure->epsilon[j]);
			}
			// diagonal intensities
			intensity_Term_Boundary[j] = norm(field_Term_Boundary[j]);

			// incoherent diagonal sum
			incoherent_Diagonal_Sum += intensity_Term_Boundary[j];

			// coherent diagonal sum
			coherent_Diagonal_Sum += field_Term_Boundary[j];
		}
	} else
	{
		for (int j = 0; j<num_Boundaries_Sharp; j++)
		{
			// all fields
			field_Term_Boundary[j] = calc_Field_Term_j_Discrete(point_Index, j, q0_U_i, q0_U_r, q_U_i, q_U_r);

			// diagonal intensities
			intensity_Term_Boundary[j] = norm(field_Term_Boundary[j]);

			// incoherent diagonal sum
			incoherent_Diagonal_Sum += intensity_Term_Boundary[j];

			// coherent diagonal sum
			coherent_Diagonal_Sum += field_Term_Boundary[j];
		}
	}
	if(multilayer->imperfections_Model.vertical_Correlation == full_Correlation) return norm(coherent_Diagonal_Sum);
	if(multilayer->imperfections_Model.vertical_Correlation == zero_Correlation) return incoherent_Diagonal_Sum;


	if(multilayer->imperfections_Model.vertical_Correlation == partial_Correlation)
	{
		// start from substrate, i>j ( i lies deeper than j )
		for (int i = num_Boundaries_Sharp-1; i>=1; i--)
		{
			for (int j = i-1; j>=0; j--)
			{
				half_Sum_Field_Term[i][j] = 2*real(field_Term_Boundary[i]*conj(field_Term_Boundary[j]));
			}
		}
		return incoherent_Diagonal_Sum;
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
	// main PSD
	if(set_PSD_to_1)
	{
		PSD_1D_Func_Vec[thread_Index] = Global_Variables::no_PSD_at_all_1D;
	} else
	{
		if(multilayer->imperfections_Model.PSD_Model == ABC_Model)
		{
			PSD_1D_Func_Vec[thread_Index] = Global_Variables::PSD_ABC_1D;
		} else
        if(multilayer->imperfections_Model.PSD_Model == stretched_exp_Model)
		{
			if(abs(struct_Data.roughness_Model.fractal_alpha.value-1)>DBL_EPSILON)
			{
//				if(abs(struct_Data.roughness_Model.fractal_alpha.value - 0.5) < DBL_EPSILON)
//				{
//					PSD_1D_Func_Vec[thread_Index] = Global_Variables::PSD_ABC_1D; // don't use, because of different factors main_PSD_1D_factor and main_PSD_2D_factor in Node, used with different functions
//				} else
//				{
					PSD_1D_Func_Vec[thread_Index] = Global_Variables::PSD_Fractal_Gauss_1D;
//				}
			} else
			{
				PSD_1D_Func_Vec[thread_Index] = Global_Variables::PSD_Real_Gauss_1D;
			}
		}
	}

	// gauss peak for PSD
	if(!multilayer->imperfections_Model.add_Gauss_Peak || set_PSD_to_1)
	{
		PSD_Gauss_Peak_1D_Func_Vec[thread_Index] = Global_Variables::zero_PSD_1D;
	} else
	{
		PSD_Gauss_Peak_1D_Func_Vec[thread_Index] = Global_Variables::PSD_Gauss_Peak_1D;
	}
}

void Unwrapped_Reflection::choose_PSD_2D_Function(const Data& struct_Data, int thread_Index)
{
	// main PSD
	if(set_PSD_to_1)
	{
		PSD_2D_Func_Vec[thread_Index] = Global_Variables::no_PSD_at_all_2D;
	} else
	{
		if(multilayer->imperfections_Model.PSD_Model == ABC_Model)
		{
			PSD_2D_Func_Vec[thread_Index] = Global_Variables::PSD_ABC_2D;
		} else
        if(multilayer->imperfections_Model.PSD_Model == stretched_exp_Model)
		{
			if(abs(struct_Data.roughness_Model.fractal_alpha.value-1)>DBL_EPSILON)
			{
//				if(abs(struct_Data.roughness_Model.fractal_alpha.value - 0.5) < DBL_EPSILON)
//				{
//					PSD_2D_Func_Vec[thread_Index] = Global_Variables::PSD_ABC_2D; // don't use, because of different factors main_PSD_1D_factor and main_PSD_2D_factor in Node, used with different functions
//				} else
//				{
					PSD_2D_Func_Vec[thread_Index] = Global_Variables::PSD_Fractal_Gauss_2D;
//				}
			} else
			{
				PSD_2D_Func_Vec[thread_Index] = Global_Variables::PSD_Real_Gauss_2D;
			}
		}
	}

	// gauss peak for PSD
	if(!multilayer->imperfections_Model.add_Gauss_Peak || set_PSD_to_1)
	{
		PSD_Gauss_Peak_2D_Func_Vec[thread_Index] = Global_Variables::zero_PSD_2D;
	} else
	{
		PSD_Gauss_Peak_2D_Func_Vec[thread_Index] = Global_Variables::PSD_Gauss_Peak_2D;
	}
}

void Unwrapped_Reflection::multifly_Fresnel_And_Weak_Factor(int thread_Index)
{
	/// used only in if Fresnel was taken from Node
	// s-polarization
	if(has_S_Pol)
	{
		for (int i = 0; i < num_Boundaries_Sharp; ++i)
		{
			// reflectance
			r_Fresnel_s[thread_Index][i] *= weak_Factor_R[thread_Index][i];

			// transmittance
			t_Fresnel_s[thread_Index][i] *= weak_Factor_T[thread_Index][i];
		}
	}

	// p-polarization
	if(has_P_Pol)
	{
		for (int i = 0; i < num_Boundaries_Sharp; ++i)
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
	// anyway
	epsilon_Ambient  [thread_Index] = media_Node_Map_Vector.front()->epsilon[point_Index];
	epsilon_Substrate[thread_Index] = media_Node_Map_Vector.back() ->epsilon[point_Index];
	fill_DW_Factors_From_Node_Vector(thread_Index, point_Index);

	// if sharp structure
	if(!multilayer->discretization_Parameters.enable_Discretization)
	{
		// in case of grading or regular aperiodic, some of these values are temporary and will be recalculated
		fill_Components_From_Node_Vector(thread_Index, point_Index);

		// if we have some grading
		if( unwrapped_Structure->depth_Grading )
		{
			calc_Exponenta(thread_Index, point_Index,unwrapped_Structure->thickness);
		}
		if( unwrapped_Structure->sigma_Grading &&
			multilayer->imperfections_Model.approximation != DWBA_approximation &&
			multilayer->imperfections_Model.approximation != SA_approximation   &&
			multilayer->imperfections_Model.approximation != CSA_approximation  )
		{
			calc_Weak_Factor(thread_Index, point_Index);
			multifly_Fresnel_And_Weak_Factor(thread_Index);
		}
	} else
	// discretization
	{
		if( measurement.argument_Type  == argument_Types[Wavelength_Energy] ) // specular only
		{			
			calc_Hi(point_Index, measurement.k_Vec[point_Index], measurement.beam_Theta_0_Cos2_Value, unwrapped_Structure->discretized_Epsilon_Dependent[point_Index]);
			calc_Exponenta(thread_Index, point_Index,unwrapped_Structure->discretized_Thickness);
			calc_Fresnel(thread_Index, point_Index, unwrapped_Structure->discretized_Epsilon_Dependent[point_Index]);
		} else
		{
			double cos2 = 1;
			if( measurement.measurement_Type == measurement_Types[Specular_Scan] )		cos2 = max(measurement.beam_Theta_0_Cos2_Vec[point_Index], DBL_EPSILON);
			if(spec_Scat_mode == SPECULAR_MODE)
			{
				if( measurement.measurement_Type == measurement_Types[Offset_Scan] ||
					measurement.measurement_Type == measurement_Types[Rocking_Curve])	cos2 = max(measurement.beam_Theta_0_Cos2_Vec[point_Index], DBL_EPSILON);
				if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
					measurement.measurement_Type == measurement_Types[GISAS_Map])		cos2 = max(measurement.beam_Theta_0_Cos2_Value, DBL_EPSILON);
			}
			if(spec_Scat_mode == SCATTERED_MODE)
			{
				cos2 = max(measurement.detector_Theta_Cos2_Vec[point_Index], DBL_EPSILON);
			}
			calc_Hi(point_Index, measurement.k_Value, cos2, unwrapped_Structure->discretized_Epsilon);
			calc_Exponenta(thread_Index, point_Index,unwrapped_Structure->discretized_Thickness);
			calc_Fresnel(thread_Index, point_Index, unwrapped_Structure->discretized_Epsilon);
		}
	}
	calc_Local(thread_Index, point_Index);

	// if we need to calculate detailed field distribution
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			if(!multilayer->discretization_Parameters.enable_Discretization)	{
				calc_Sliced_Field(thread_Index, point_Index, unwrapped_Structure->epsilon);
			} else	{
				calc_Sliced_Field(thread_Index, point_Index, unwrapped_Structure->discretized_Epsilon);
			}
		}
		if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			if(!multilayer->discretization_Parameters.enable_Discretization)	{
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
		if(has_S_Pol) calc_Amplitudes_Field(thread_Index, point_Index, "s");
		if(has_P_Pol) calc_Amplitudes_Field(thread_Index, point_Index, "p");
		// in specular mode we stop here
		// in scattered mode we go further and use calculated q and q0 fields
		if(spec_Scat_mode == SCATTERED_MODE)
		{
			double sin_Theta_0 = max(measurement.beam_Theta_0_Sin_Value, DBL_EPSILON);
			double cos_Theta_0 = max(measurement.beam_Theta_0_Cos_Value, DBL_EPSILON);
			if( measurement.measurement_Type == measurement_Types[Offset_Scan] ||
				measurement.measurement_Type == measurement_Types[Rocking_Curve] )
			{
				sin_Theta_0 = max(measurement.beam_Theta_0_Sin_Vec[point_Index], DBL_EPSILON);
				cos_Theta_0 = max(measurement.beam_Theta_0_Cos_Vec[point_Index], DBL_EPSILON);
			}
			double e_Factor_PT_1D		   = pow(measurement.k_Value,3)/(16*M_PI     *sin_Theta_0*sqrt(abs(cos_Theta_0*measurement.detector_Theta_Cos_Vec[point_Index])));
			double e_Factor_PT_2D		   = pow(measurement.k_Value,4)/(16*M_PI*M_PI*sin_Theta_0);
			double e_Factor_DWBA_SA_CSA_1D = pow(measurement.k_Value,3)/(2 *M_PI*M_PI*sin_Theta_0*sqrt(abs(cos_Theta_0*measurement.detector_Theta_Cos_Vec[point_Index])));

			// common
			if(!multilayer->discretization_Parameters.enable_Discretization)
			{
				if(has_S_Pol)	calc_Field_Up_Low(thread_Index, point_Index, "s");
				if(has_P_Pol)	calc_Field_Up_Low(thread_Index, point_Index, "p");
			}

			// roughness
			if(multilayer->imperfections_Model.use_Roughness)
			{
				if(multilayer->imperfections_Model.approximation == PT_approximation)
				{
					// common for 1D and 2D linear growth PSD
					double (*function_Scattering_Linear_Growth_s)(double, void*);
					double (*function_Scattering_Linear_Growth_p)(double, void*);
					double (*function_Scattering_Linear_Growth_sp)(double, void*);
					if( multilayer->imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model)	{
						function_Scattering_Linear_Growth_s  = &function_Scattering_Linear_Growth_Alpha_2D_s;
						function_Scattering_Linear_Growth_p  = &function_Scattering_Linear_Growth_Alpha_2D_p;
						function_Scattering_Linear_Growth_sp = &function_Scattering_Linear_Growth_Alpha_2D_sp;
					}
					if( multilayer->imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model) {
						function_Scattering_Linear_Growth_s  = &function_Scattering_Linear_Growth_n_1_4_2D_s;
						function_Scattering_Linear_Growth_p  = &function_Scattering_Linear_Growth_n_1_4_2D_p;
						function_Scattering_Linear_Growth_sp = &function_Scattering_Linear_Growth_n_1_4_2D_sp;
					}

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
								double PSD_Factor = PSD_Gauss_Peak_1D_Func_Vec[thread_Index](1,
																							substrate.roughness_Model.peak_Frequency.value,
																							substrate.roughness_Model.peak_Frequency_Width.value,
																							measurement.k_Value,
																							measurement.detector_Theta_Cos_Vec[point_Index],
																							cos_Theta_0,
																							substrate_Node->spline_PSD_Peak,
																							substrate_Node->acc_PSD_Peak);
								if(substrate_Node->spline_PSD_Combined_1D_Condition(multilayer->imperfections_Model))
								{
									double p = measurement.k_Value*abs(measurement.detector_Theta_Cos_Vec[point_Index] - cos_Theta_0)/(2*M_PI);
									PSD_Factor += gsl_spline_eval(substrate_Node->spline_PSD_Combined_1D, p, substrate_Node->acc_PSD_Combined_1D);
								} else
								{
									if(multilayer->imperfections_Model.add_Measured_PSD_1D && multilayer->imperfections_Model.PSD_1D.is_Loaded())
									{
										double nu = measurement.k_Value*abs(measurement.detector_Theta_Cos_Vec[point_Index] - cos_Theta_0)/(2*M_PI);
										double nu_Start = multilayer->imperfections_Model.PSD_1D.argument.front();
										double nu_End   = multilayer->imperfections_Model.PSD_1D.argument.back();

										// without nu_Limit here
										if(nu<=nu_Start || nu>=nu_End )
										{
											PSD_Factor += PSD_1D_Func_Vec[thread_Index](substrate.main_PSD_1D_factor,
																						substrate.roughness_Model.cor_radius.value,
																						substrate.roughness_Model.fractal_alpha.value,
																						measurement.k_Value,
																						measurement.detector_Theta_Cos_Vec[point_Index],
																						cos_Theta_0,
																						substrate_Node->spline_PSD_FG_1D,
																						substrate_Node->acc_PSD_FG_1D);
										} else
										{
											PSD_Factor += Global_Variables::PSD_Measured_1D(substrate.main_PSD_1D_factor,
																							substrate.roughness_Model.cor_radius.value,
																							substrate.roughness_Model.fractal_alpha.value,
																							measurement.k_Value,
																							measurement.detector_Theta_Cos_Vec[point_Index],
																							cos_Theta_0,
																							substrate_Node->spline_PSD_Meas_1D,
																							substrate_Node->acc_PSD_Meas_1D);
										}
									} else
									{
										PSD_Factor += PSD_1D_Func_Vec[thread_Index](substrate.main_PSD_1D_factor,
																					substrate.roughness_Model.cor_radius.value,
																					substrate.roughness_Model.fractal_alpha.value,
																					measurement.k_Value,
																					measurement.detector_Theta_Cos_Vec[point_Index],
																					cos_Theta_0,
																					substrate_Node->spline_PSD_FG_1D,
																					substrate_Node->acc_PSD_FG_1D);
									}
								}

								PSD_Factor = max(PSD_Factor,0.);

								// s-polarization
								if(has_S_Pol)
								{
									double field_Term_Sum = calc_Field_Term_Sum("s", point_Index, thread_Index);
									calculated_Values.S_s[point_Index] = e_Factor_PT_1D * field_Term_Sum * PSD_Factor;
								}
								// p-polarization
								if(has_P_Pol)
								{
									double field_Term_Sum = calc_Field_Term_Sum("p", point_Index, thread_Index);
									calculated_Values.S_p[point_Index] = e_Factor_PT_1D * field_Term_Sum * PSD_Factor;
								}
							} else
							/// individual PSD of items
							{
								fill_Item_PSD_1D(thread_Index, point_Index, cos_Theta_0);

								// s-polarization
								if(has_S_Pol)
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									double field_Term_Sum_PSD = 0;
									for (int j = 0; j<num_Boundaries_Sharp; j++)
									{
										int item_Index = boundary_Item_Vec[j];
										double PSD_Factor = PSD_Factor_Item[thread_Index][item_Index];
										field_Term_Sum_PSD += intensity_Term_Boundary_s[thread_Index][j] * PSD_Factor;
									}
									calculated_Values.S_s[point_Index] = e_Factor_PT_1D * field_Term_Sum_PSD;
								}
								// p-polarization
								if(has_P_Pol)
								{
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									double field_Term_Sum_PSD = 0;
									for (int j = 0; j<num_Boundaries_Sharp; j++)
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
							// here we need 2D PSD
							choose_PSD_2D_Function(substrate, thread_Index);
							if( multilayer->imperfections_Model.inheritance_Model == replication_Factor_Inheritance_Model)
							{
								// pure s-polarization
								if(pure_S_Pol)
								{
									double incoherent_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, incoherent_Sum_s, 0, true };
									gsl_function F = { &function_Scattering_Replication_Factor_2D_s, &params };

									calculated_Values.S_s[point_Index] = e_Factor_PT_2D * roughness_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
								} else
								// pure p-polarization
								if(pure_P_Pol)
								{
									double incoherent_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, 0, incoherent_Sum_p, true };
									gsl_function F = { &function_Scattering_Replication_Factor_2D_p, &params };

									calculated_Values.S_p[point_Index] = e_Factor_PT_2D * roughness_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
								} else
								// mixed sp-polarization
								{
									double incoherent_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									double incoherent_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, incoherent_Sum_s, incoherent_Sum_p, true };
									gsl_function F = { &function_Scattering_Replication_Factor_2D_sp, &params };

									// crutch: s and p only on mixed state. no different s and p calculations
									calculated_Values.S_s[point_Index] = e_Factor_PT_2D * roughness_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
									calculated_Values.S_p[point_Index] = calculated_Values.S_s[point_Index];
								}
							} else
							if( multilayer->imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
								multilayer->imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model )
							{
								Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], cos_Theta_0, 0, 0, true };
								// pure s-polarization
								if(pure_S_Pol)
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									gsl_function F = { function_Scattering_Linear_Growth_s, &params };
									calculated_Values.S_s[point_Index] = e_Factor_PT_2D * roughness_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
								} else
								// pure p-polarization
								if(pure_P_Pol)
								{
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									gsl_function F = { function_Scattering_Linear_Growth_p, &params };
									calculated_Values.S_p[point_Index] = e_Factor_PT_2D * roughness_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
								} else
								// mixed sp-polarization
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									gsl_function F = { function_Scattering_Linear_Growth_sp, &params };

									// crutch: s and p only on mixed state. no different s and p calculations
									calculated_Values.S_s[point_Index] = e_Factor_PT_2D * roughness_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
									calculated_Values.S_p[point_Index] = calculated_Values.S_s[point_Index];
								}
							}
						}
					}
					if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
					{
						choose_PSD_2D_Function(substrate, thread_Index);
						if( multilayer->imperfections_Model.vertical_Correlation == full_Correlation ||
							multilayer->imperfections_Model.vertical_Correlation == zero_Correlation )
						{
							if( multilayer->imperfections_Model.use_Common_Roughness_Function )
							{
								double PSD_2D_Factor;

								// pure s-polarization
								if(pure_S_Pol)
								{
									double field_Term_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										PSD_2D_Factor =PSD_Gauss_Peak_2D_Func_Vec[thread_Index](substrate.PSD_Gauss_Peak_2D_Factor,
																								substrate.roughness_Model.peak_Frequency.value,
																								substrate.roughness_Model.peak_Frequency_Width.value,
																								measurement.k_Value,
																								measurement.detector_Theta_Cos_Vec[point_Index],
																								measurement.beam_Theta_0_Cos_Value,
																								measurement.detector_Phi_Cos_Vec[phi_Index],
																								substrate_Node->spline_PSD_Peak,
																								substrate_Node->acc_PSD_Peak);

										if(multilayer->imperfections_Model.add_Measured_PSD_2D && multilayer->imperfections_Model.PSD_2D.is_Loaded())
										{
											double nu = measurement.k_Value*sqrt(measurement.detector_Theta_Cos_Vec[point_Index]*measurement.detector_Theta_Cos_Vec[point_Index] + measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value - 2*measurement.beam_Theta_0_Cos_Value*measurement.detector_Theta_Cos_Vec[point_Index]*measurement.detector_Phi_Cos_Vec[phi_Index])/(2*M_PI); // here we need real frequency

											double nu_Start = multilayer->imperfections_Model.PSD_2D.argument.front();
											double nu_End   = multilayer->imperfections_Model.PSD_2D.argument.back();

											// without nu_Limit here
											if(nu<=nu_Start || nu>=nu_End )
											{
												PSD_2D_Factor += PSD_2D_Func_Vec[thread_Index]( substrate.main_PSD_2D_factor,
																								substrate.roughness_Model.cor_radius.value,
																								substrate.roughness_Model.fractal_alpha.value,
																								measurement.k_Value,
																								measurement.detector_Theta_Cos_Vec[point_Index],
																								measurement.beam_Theta_0_Cos_Value,
																								measurement.detector_Phi_Cos_Vec[phi_Index],
																								substrate_Node->spline_PSD_FG_2D,
																								substrate_Node->acc_PSD_FG_2D);
											} else
											{
												PSD_2D_Factor += Global_Variables::PSD_Measured_2D( substrate.main_PSD_2D_factor,
																									substrate.roughness_Model.cor_radius.value,
																									substrate.roughness_Model.fractal_alpha.value,
																									measurement.k_Value,
																									measurement.detector_Theta_Cos_Vec[point_Index],
																									measurement.beam_Theta_0_Cos_Value,
																									measurement.detector_Phi_Cos_Vec[phi_Index],
																									substrate_Node->spline_PSD_Meas_2D,
																									substrate_Node->acc_PSD_Meas_2D);
											}
										} else
										{
											PSD_2D_Factor += PSD_2D_Func_Vec[thread_Index]( substrate.main_PSD_2D_factor,
																							substrate.roughness_Model.cor_radius.value,
																							substrate.roughness_Model.fractal_alpha.value,
																							measurement.k_Value,
																							measurement.detector_Theta_Cos_Vec[point_Index],
																							measurement.beam_Theta_0_Cos_Value,
																							measurement.detector_Phi_Cos_Vec[phi_Index],
																							substrate_Node->spline_PSD_FG_2D,
																							substrate_Node->acc_PSD_FG_2D);
										}
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * field_Term_Sum_s * PSD_2D_Factor * measurement.footprint_Factor_Vec[point_Index];
									}

								} else
								// pure p-polarization
								if(pure_P_Pol)
								{
									double field_Term_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										PSD_2D_Factor =PSD_Gauss_Peak_2D_Func_Vec[thread_Index](substrate.PSD_Gauss_Peak_2D_Factor,
																								substrate.roughness_Model.peak_Frequency.value,
																								substrate.roughness_Model.peak_Frequency_Width.value,
																								measurement.k_Value,
																								measurement.detector_Theta_Cos_Vec[point_Index],
																								measurement.beam_Theta_0_Cos_Value,
																								measurement.detector_Phi_Cos_Vec[phi_Index],
																								substrate_Node->spline_PSD_Peak,
																								substrate_Node->acc_PSD_Peak);

										if(multilayer->imperfections_Model.add_Measured_PSD_2D && multilayer->imperfections_Model.PSD_2D.is_Loaded())
										{
											double nu = measurement.k_Value*sqrt(measurement.detector_Theta_Cos_Vec[point_Index]*measurement.detector_Theta_Cos_Vec[point_Index] + measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value - 2*measurement.beam_Theta_0_Cos_Value*measurement.detector_Theta_Cos_Vec[point_Index]*measurement.detector_Phi_Cos_Vec[phi_Index])/(2*M_PI); // here we need real frequency

											double nu_Start = multilayer->imperfections_Model.PSD_2D.argument.front();
											double nu_End   = multilayer->imperfections_Model.PSD_2D.argument.back();

											// without nu_Limit here
											if(nu<=nu_Start || nu>=nu_End )
											{
												PSD_2D_Factor += PSD_2D_Func_Vec[thread_Index]( substrate.main_PSD_2D_factor,
																								substrate.roughness_Model.cor_radius.value,
																								substrate.roughness_Model.fractal_alpha.value,
																								measurement.k_Value,
																								measurement.detector_Theta_Cos_Vec[point_Index],
																								measurement.beam_Theta_0_Cos_Value,
																								measurement.detector_Phi_Cos_Vec[phi_Index],
																								substrate_Node->spline_PSD_FG_2D,
																								substrate_Node->acc_PSD_FG_2D);
											} else
											{
												PSD_2D_Factor += Global_Variables::PSD_Measured_2D( substrate.main_PSD_2D_factor,
																									substrate.roughness_Model.cor_radius.value,
																									substrate.roughness_Model.fractal_alpha.value,
																									measurement.k_Value,
																									measurement.detector_Theta_Cos_Vec[point_Index],
																									measurement.beam_Theta_0_Cos_Value,
																									measurement.detector_Phi_Cos_Vec[phi_Index],
																									substrate_Node->spline_PSD_Meas_2D,
																									substrate_Node->acc_PSD_Meas_2D);
											}
										} else
										{
											PSD_2D_Factor += PSD_2D_Func_Vec[thread_Index]( substrate.main_PSD_2D_factor,
																							substrate.roughness_Model.cor_radius.value,
																							substrate.roughness_Model.fractal_alpha.value,
																							measurement.k_Value,
																							measurement.detector_Theta_Cos_Vec[point_Index],
																							measurement.beam_Theta_0_Cos_Value,
																							measurement.detector_Phi_Cos_Vec[phi_Index],
																							substrate_Node->spline_PSD_FG_2D,
																							substrate_Node->acc_PSD_FG_2D);
										}

										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * field_Term_Sum_p * PSD_2D_Factor * measurement.footprint_Factor_Vec[point_Index];
									}
								} else
								// mixed sp-polarization
								{
									double field_Term_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									double field_Term_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										PSD_2D_Factor =PSD_Gauss_Peak_2D_Func_Vec[thread_Index](substrate.PSD_Gauss_Peak_2D_Factor,
																								substrate.roughness_Model.peak_Frequency.value,
																								substrate.roughness_Model.peak_Frequency_Width.value,
																								measurement.k_Value,
																								measurement.detector_Theta_Cos_Vec[point_Index],
																								measurement.beam_Theta_0_Cos_Value,
																								measurement.detector_Phi_Cos_Vec[phi_Index],
																								substrate_Node->spline_PSD_Peak,
																								substrate_Node->acc_PSD_Peak);

										if(multilayer->imperfections_Model.add_Measured_PSD_2D && multilayer->imperfections_Model.PSD_2D.is_Loaded())
										{
											double nu = measurement.k_Value*sqrt(measurement.detector_Theta_Cos_Vec[point_Index]*measurement.detector_Theta_Cos_Vec[point_Index] + measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value - 2*measurement.beam_Theta_0_Cos_Value*measurement.detector_Theta_Cos_Vec[point_Index]*measurement.detector_Phi_Cos_Vec[phi_Index])/(2*M_PI); // here we need real frequency

											double nu_Start = multilayer->imperfections_Model.PSD_2D.argument.front();
											double nu_End   = multilayer->imperfections_Model.PSD_2D.argument.back();

											// without nu_Limit here
											if(nu<=nu_Start || nu>=nu_End )
											{
												PSD_2D_Factor += PSD_2D_Func_Vec[thread_Index]( substrate.main_PSD_2D_factor,
																								substrate.roughness_Model.cor_radius.value,
																								substrate.roughness_Model.fractal_alpha.value,
																								measurement.k_Value,
																								measurement.detector_Theta_Cos_Vec[point_Index],
																								measurement.beam_Theta_0_Cos_Value,
																								measurement.detector_Phi_Cos_Vec[phi_Index],
																								substrate_Node->spline_PSD_FG_2D,
																								substrate_Node->acc_PSD_FG_2D);
											} else
											{
												PSD_2D_Factor += Global_Variables::PSD_Measured_2D( substrate.main_PSD_2D_factor,
																									substrate.roughness_Model.cor_radius.value,
																									substrate.roughness_Model.fractal_alpha.value,
																									measurement.k_Value,
																									measurement.detector_Theta_Cos_Vec[point_Index],
																									measurement.beam_Theta_0_Cos_Value,
																									measurement.detector_Phi_Cos_Vec[phi_Index],
																									substrate_Node->spline_PSD_Meas_2D,
																									substrate_Node->acc_PSD_Meas_2D);
											}
										} else
										{
											PSD_2D_Factor += PSD_2D_Func_Vec[thread_Index]( substrate.main_PSD_2D_factor,
																							substrate.roughness_Model.cor_radius.value,
																							substrate.roughness_Model.fractal_alpha.value,
																							measurement.k_Value,
																							measurement.detector_Theta_Cos_Vec[point_Index],
																							measurement.beam_Theta_0_Cos_Value,
																							measurement.detector_Phi_Cos_Vec[phi_Index],
																							substrate_Node->spline_PSD_FG_2D,
																							substrate_Node->acc_PSD_FG_2D);
										}

										calculated_Values.GISAS_Map[phi_Index][point_Index] = (s_Weight*field_Term_Sum_s + p_Weight*field_Term_Sum_p)*e_Factor_PT_2D*PSD_2D_Factor * measurement.footprint_Factor_Vec[point_Index];
									}
								}
							} else
							/// individual PSD of items
							{
								// pure s-polarization
								if(pure_S_Pol)
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										fill_Item_PSD_2D(thread_Index, point_Index, phi_Index);

										double field_With_PSD_2D_Factor = 0;
										for(int j = 0; j<num_Boundaries_Sharp; j++)
										{
											int item_Index = boundary_Item_Vec[j];
											double PSD_2D_Factor = PSD_Factor_Item[thread_Index][item_Index];

											field_With_PSD_2D_Factor += intensity_Term_Boundary_s[thread_Index][j] * PSD_2D_Factor;
										}
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * field_With_PSD_2D_Factor * measurement.footprint_Factor_Vec[point_Index];
									}
								} else
								// pure p-polarization
								if(pure_P_Pol)
								{
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										fill_Item_PSD_2D(thread_Index, point_Index, phi_Index);

										double field_With_PSD_2D_Factor = 0;
										for (int j = 0; j<num_Boundaries_Sharp; j++)
										{
											int item_Index = boundary_Item_Vec[j];
											double PSD_2D_Factor = PSD_Factor_Item[thread_Index][item_Index];

											field_With_PSD_2D_Factor += intensity_Term_Boundary_p[thread_Index][j] * PSD_2D_Factor;
										}
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * field_With_PSD_2D_Factor * measurement.footprint_Factor_Vec[point_Index];
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
										for (int j = 0; j<num_Boundaries_Sharp; j++)
										{
											int item_Index = boundary_Item_Vec[j];
											double PSD_2D_Factor = PSD_Factor_Item[thread_Index][item_Index];

											field_With_PSD_2D_Factor_s += intensity_Term_Boundary_p[thread_Index][j] * PSD_2D_Factor;
											field_With_PSD_2D_Factor_p += intensity_Term_Boundary_p[thread_Index][j] * PSD_2D_Factor;
										}
										calculated_Values.GISAS_Map[phi_Index][point_Index] = (s_Weight*field_With_PSD_2D_Factor_s + p_Weight*field_With_PSD_2D_Factor_p)*e_Factor_PT_2D * measurement.footprint_Factor_Vec[point_Index];
									}
								}
							}
						}
						if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation )
						{
							if( multilayer->imperfections_Model.inheritance_Model == replication_Factor_Inheritance_Model )
							{
								// pure s-polarization
								if(pure_S_Pol)
								{
									double incoherent_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, incoherent_Sum_s, 0, false };
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Replication_Factor_2D_s(measurement.detector_Phi_Cos_Vec[phi_Index], &params) * measurement.footprint_Factor_Vec[point_Index];
									}
								} else
								// pure p-polarization
								if(pure_P_Pol)
								{
									double incoherent_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, 0, incoherent_Sum_p, false };
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Replication_Factor_2D_p(measurement.detector_Phi_Cos_Vec[phi_Index], &params) * measurement.footprint_Factor_Vec[point_Index];
									}
								} else
								// mixed sp-polarization
								{
									double incoherent_Sum_s = calc_Field_Term_Sum("s", point_Index, thread_Index);
									double incoherent_Sum_p = calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, incoherent_Sum_s, incoherent_Sum_p, false };
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Replication_Factor_2D_sp(measurement.detector_Phi_Cos_Vec[phi_Index], &params) * measurement.footprint_Factor_Vec[point_Index];
									}
								}
							} else
							if( multilayer->imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
								multilayer->imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model )
							{
								Params params { this, thread_Index, measurement.detector_Theta_Cos_Vec[point_Index], measurement.beam_Theta_0_Cos_Value, 0, 0, false };

								// pure s-polarization
								if(pure_S_Pol)
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Linear_Growth_s(measurement.detector_Phi_Cos_Vec[phi_Index], &params) * measurement.footprint_Factor_Vec[point_Index];
									}
								} else
								// pure p-polarization
								if(pure_P_Pol)
								{
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Linear_Growth_p(measurement.detector_Phi_Cos_Vec[phi_Index], &params) * measurement.footprint_Factor_Vec[point_Index];
									}
								} else
								// mixed sp-polarization
								{
									calc_Field_Term_Sum("s", point_Index, thread_Index);
									calc_Field_Term_Sum("p", point_Index, thread_Index);
									for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
									{
										calculated_Values.GISAS_Map[phi_Index][point_Index] = e_Factor_PT_2D * function_Scattering_Linear_Growth_sp(measurement.detector_Phi_Cos_Vec[phi_Index], &params) * measurement.footprint_Factor_Vec[point_Index];
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
					if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
						measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
						measurement.measurement_Type == measurement_Types[Offset_Scan] )
					{
						calc_k_Wavenumber_Up_Low(thread_Index, point_Index);
						if(has_S_Pol) calc_K_Factor_DWBA_SA_CSA(point_Index, thread_Index, "s");
						if(has_P_Pol) calc_K_Factor_DWBA_SA_CSA(point_Index, thread_Index, "p");
						choose_Cor_Function(thread_Index);

						if( multilayer->imperfections_Model.vertical_Correlation == full_Correlation ||
							multilayer->imperfections_Model.vertical_Correlation == zero_Correlation )
						{
							if( multilayer->imperfections_Model.use_Common_Roughness_Function )
							{
								// s-polarization
								if(has_S_Pol)
								{
									for(int n_Power=1; n_Power<=DWBA_n_Max_Series; n_Power++)
									{
										pre_Fourier_Factor[thread_Index][n_Power-1] = calc_K_Factor_Term_Sum_DWBA_SA_CSA(thread_Index, "s", n_Power) / factorial[n_Power];
									}
									calculated_Values.S_s[point_Index] = e_Factor_DWBA_SA_CSA_1D * cor_Function_Integration(point_Index, thread_Index, cos_Theta_0);
								}
								// p-polarization
								if(has_P_Pol)
								{
									for(int n_Power=1; n_Power<=DWBA_n_Max_Series; n_Power++)
									{
										pre_Fourier_Factor[thread_Index][n_Power-1] = calc_K_Factor_Term_Sum_DWBA_SA_CSA(thread_Index, "p", n_Power) / factorial[n_Power];
									}
									calculated_Values.S_p[point_Index] = e_Factor_DWBA_SA_CSA_1D * cor_Function_Integration(point_Index, thread_Index, cos_Theta_0);
								}
							} else
							/// individual PSD of items
							{
								// s-polarization
								if(has_S_Pol)
								{
									for(int n_Power=1; n_Power<=DWBA_n_Max_Series; n_Power++)
									{
										calc_K_Factor_Term_Sum_DWBA_SA_CSA(thread_Index, "s", n_Power);
									}
									calculated_Values.S_s[point_Index] = e_Factor_DWBA_SA_CSA_1D * cor_Function_Integration(point_Index, thread_Index, cos_Theta_0);
								}
								// p-polarization
								if(has_P_Pol)
								{
									for(int n_Power=1; n_Power<=DWBA_n_Max_Series; n_Power++)
									{
										calc_K_Factor_Term_Sum_DWBA_SA_CSA(thread_Index, "p", n_Power);
									}
									calculated_Values.S_p[point_Index] = e_Factor_DWBA_SA_CSA_1D * cor_Function_Integration(point_Index, thread_Index, cos_Theta_0);
								}
							}
						}
						if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation)
						{
							//if(multilayer->imperfections_Model.use_Common_Roughness_Function || multilayer->imperfections_Model.vertical_Correlation == partial_Correlation)
							// TODO
						}
					}
				}
			} else
			// if no roughness set scattering to 0
			{
				if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
					measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
					measurement.measurement_Type == measurement_Types[Offset_Scan] )
				{
					if(multilayer->imperfections_Model.use_Particles)
					{
						calculated_Values.S_s[point_Index] = 0;
						calculated_Values.S_p[point_Index] = 0;
					}
				}
				if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
				{
					if(multilayer->imperfections_Model.use_Particles)
					{
						for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
						{
							calculated_Values.GISAS_Map[phi_Index][point_Index] = 0;
						}
					}
				}
			}

			// particles
			if(multilayer->imperfections_Model.use_Particles)
			{
				if(short_Flat_Calc_Tree.size()<2) return;

				if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
					measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
					measurement.measurement_Type == measurement_Types[Offset_Scan] )
				{
					if(ignore_1D_particles_scattering) return;
				}

				/// all polarization cases are here
				calc_k_Wavenumber_Layer(thread_Index, point_Index);
				calc_Half_Layer_Exponents(thread_Index);
				// factors from field amplitudes
				if(has_S_Pol)
				{
					calc_C_Factor("s", thread_Index, point_Index);
					// pure s-polarization
					if(pure_S_Pol)
					{
						fill_C_Factor_Mixed("s", thread_Index);
					}
				}
				if(has_P_Pol)
				{
					calc_C_Factor("p", thread_Index, point_Index);
					// pure p-polarization
					if(pure_P_Pol)
					{
						fill_C_Factor_Mixed("p", thread_Index);
					}
				}
				// mixed sp-polarization
				if( mixed_Pol )
				{
					calc_C_Factor_Mixed(thread_Index);
				}

				// factors from particle z-deviations
				calc_Omega_Factor(thread_Index);
				calc_Gamma_Factor(thread_Index);

				bool common = multilayer->imperfections_Model.use_Common_Particle_Function;

				choose_Form_Factor_2D_Function(thread_Index);
				int last_Item_Index = short_Flat_Calc_Tree.size()-2;
				Data& last_Item = appropriate_Item_Vec[last_Item_Index];
				Node* last_Node = short_Flat_Calc_Tree[last_Item_Index];
				double cos_Theta = measurement.detector_Theta_Cos_Vec[point_Index];

				// splines for form factors (only for layers with used particles)
				for(size_t item_Index : used_Appropriate_Item_Index_Vec)
				{
					Data& item = appropriate_Item_Vec[item_Index];
					Data& order_Item = common ? last_Item : item;
					size_t order_Item_Index = common ? last_Item_Index : item_Index;
					calc_Item_Form_Factor_Splines(thread_Index, item_Index, order_Item_Index, order_Item, cos_Theta_0, cos_Theta);
				}

				if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
					measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
					measurement.measurement_Type == measurement_Types[Offset_Scan] )
				{
					double scattering = 0;
					if(multilayer->imperfections_Model.particle_Vertical_Correlation == partial_Correlation)
					{
						Params_Particles params_Particles = {this, thread_Index, true, common, cos_Theta, cos_Theta_0, &last_Item, last_Node, layer_Field_Factor_Short[thread_Index]};
						gsl_function F = { function_Scattering_Particles_Partial_Correlation, &params_Particles };
						scattering = e_Factor_PT_2D * particles_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
					}
					if(multilayer->imperfections_Model.particle_Vertical_Correlation == full_Correlation)
					{
						Params_Particles params_Particles = {this, thread_Index, true, common, cos_Theta, cos_Theta_0, &last_Item, last_Node, layer_Field_Factor_Short[thread_Index]};
						gsl_function F = { function_Scattering_Particles_Full_Correlation, &params_Particles };
						scattering = e_Factor_PT_2D * particles_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
					}
					if(multilayer->imperfections_Model.particle_Vertical_Correlation == zero_Correlation)
					{
						Params_Particles params_Particles = {this, thread_Index, true, common, cos_Theta, cos_Theta_0, &last_Item, last_Node, layer_Field_Factor_Short[thread_Index]};
						gsl_function F = { function_Scattering_Particles_Zero_Correlation, &params_Particles };
						scattering = e_Factor_PT_2D * particles_Azimuthal_Integration(&F, cos_Theta_0, point_Index);
					}
					// pure s-polarization
					if(pure_S_Pol)		{
						calculated_Values.S_s[point_Index] += scattering;
					} else
					// pure p-polarization
					if(pure_P_Pol)		{
						calculated_Values.S_p[point_Index] += scattering;
					} else {
					// mixed sp-polarization
						// crutch: s and p only on mixed state. no different s and p calculations
						calculated_Values.S_s[point_Index] += scattering;
						calculated_Values.S_p[point_Index] = calculated_Values.S_s[point_Index];
					}
				}
				if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
				{
					if(multilayer->imperfections_Model.particle_Vertical_Correlation == partial_Correlation)
					{
						double G1_Type_Value = last_Node->G1_Type_Outer();
						double G2_Type_Value = G1_Type_Value; // for disorder
						double G2_Type_Value_Sqrt = sqrt(G2_Type_Value); // for disorder

						for(int item_Index : used_Appropriate_Item_Index_Vec)
						{
							Node* node = short_Flat_Calc_Tree[item_Index];
							Data& item = appropriate_Item_Vec[item_Index];
							Data& order_Item = common ? last_Item : item;

							bool is_Z_Deviation = order_Item.particles_Model.particle_Z_Position_Deviation.value > DBL_EPSILON;

							complex<double> d_Eps = node->delta_Epsilon_Contrast;
							double d_Eps_Norm = norm(d_Eps);

							if(is_Z_Deviation)
							{
								for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
								{
									size_t phi_Index_Local = phi_Index - measurement.start_Phi_Index;
									double cos_Phi = measurement.detector_Phi_Cos_Vec[phi_Index];
									double incoherent_Field_Factor = body_Scattering_Particles_Partial_Correlation_Z_Deviation(layer_Field_Factor[thread_Index][phi_Index_Local], thread_Index, item_Index, order_Item.particles_Model.particle_Interference_Function,
																															   last_Node, G1_Type_Value, G2_Type_Value, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
									calculated_Values.GISAS_Map[phi_Index][point_Index] += e_Factor_PT_2D * d_Eps_Norm * incoherent_Field_Factor * measurement.footprint_Factor_Vec[point_Index];
								}
							} else
							{
								for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
								{
									size_t phi_Index_Local = phi_Index - measurement.start_Phi_Index;
									double cos_Phi = measurement.detector_Phi_Cos_Vec[phi_Index];
									double incoherent_Field_Factor = body_Scattering_Particles_Partial_Correlation(layer_Field_Factor[thread_Index][phi_Index_Local], thread_Index, item_Index, last_Item.particles_Model.particle_Interference_Function,
																												   last_Node, G2_Type_Value_Sqrt, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
									calculated_Values.GISAS_Map[phi_Index][point_Index] += e_Factor_PT_2D * incoherent_Field_Factor * measurement.footprint_Factor_Vec[point_Index];
								}
							}
						}
						// partially coherent part summation
						for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
						{
							double cos_Phi = measurement.detector_Phi_Cos_Vec[phi_Index];
							double q2 = measurement.k_Value*measurement.k_Value*(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);

							size_t phi_Index_Local = phi_Index - measurement.start_Phi_Index;
							double coherent_Sum = 0;
							for(int& l : unwrapped_Structure->particles_Index_Vec)
							{
								double inheritance_Factor = 1;
								for(int& j : unwrapped_Structure->particles_Index_Vec)
								{
									if(j<l)
									{
										double s_j = unwrapped_Structure->lateral_Sigma_Particles[j];
										double ex = q2*s_j*s_j/2.;
										if(ex<20)
										{
											inheritance_Factor *= exp(-ex);
											coherent_Sum += 2*real(      layer_Field_Factor[thread_Index][phi_Index_Local][l] *
																	conj(layer_Field_Factor[thread_Index][phi_Index_Local][j])
																   ) *  inheritance_Factor; //unwrapped_Structure->particles_Inheritance_Factor[l][j];
										}
									}
								}
							}
							calculated_Values.GISAS_Map[phi_Index][point_Index] += e_Factor_PT_2D * coherent_Sum * measurement.footprint_Factor_Vec[point_Index];
						}
					}
					if(multilayer->imperfections_Model.particle_Vertical_Correlation == full_Correlation)
					{
						double G1_Type_Value = last_Node->G1_Type_Outer();
						double G2_Type_Value_Sqrt = sqrt(G1_Type_Value); // for disorder

						for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
						{
							size_t phi_Index_Local = phi_Index - measurement.start_Phi_Index;
							coherent_Field_Factor[thread_Index][phi_Index_Local] = 0;
						}
						for(int item_Index : used_Appropriate_Item_Index_Vec)
						{
							Node* node = short_Flat_Calc_Tree[item_Index];
							Data& item = appropriate_Item_Vec[item_Index];
							Data& order_Item = common ? last_Item : item;

							bool is_Z_Deviation = order_Item.particles_Model.particle_Z_Position_Deviation.value > DBL_EPSILON;

							complex<double> d_Eps = node->delta_Epsilon_Contrast;
							double d_Eps_Norm = norm(d_Eps);

							if(is_Z_Deviation)
							{
								for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
								{
									size_t phi_Index_Local = phi_Index - measurement.start_Phi_Index;
									double cos_Phi = measurement.detector_Phi_Cos_Vec[phi_Index];
									double incoherent_Field_Factor = body_Scattering_Particles_Full_Correlation_Z_Deviation(coherent_Field_Factor[thread_Index][phi_Index_Local], thread_Index, item_Index, last_Item.particles_Model.particle_Interference_Function,
																															last_Node, G1_Type_Value, G2_Type_Value_Sqrt, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
									calculated_Values.GISAS_Map[phi_Index][point_Index] += e_Factor_PT_2D * d_Eps_Norm * incoherent_Field_Factor * measurement.footprint_Factor_Vec[point_Index];
								}
							} else
							{
								for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
								{
									size_t phi_Index_Local = phi_Index - measurement.start_Phi_Index;
									double cos_Phi = measurement.detector_Phi_Cos_Vec[phi_Index];
									body_Scattering_Particles_Full_Correlation(coherent_Field_Factor[thread_Index][phi_Index_Local], thread_Index, item_Index, last_Item.particles_Model.particle_Interference_Function,
																			   last_Node, G2_Type_Value_Sqrt, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
								}
							}
						}
						for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
						{
							size_t phi_Index_Local = phi_Index - measurement.start_Phi_Index;
							calculated_Values.GISAS_Map[phi_Index][point_Index] += e_Factor_PT_2D * norm(coherent_Field_Factor[thread_Index][phi_Index_Local]) * measurement.footprint_Factor_Vec[point_Index];
						}
					}
					if(multilayer->imperfections_Model.particle_Vertical_Correlation == zero_Correlation)
					{
						for(int item_Index : used_Appropriate_Item_Index_Vec)
						{
							Data& item = appropriate_Item_Vec[item_Index];
							Node* node = short_Flat_Calc_Tree[item_Index];
							Node* G2_node    = common ? last_Node : node;
							Data& order_Item = common ? last_Item : item;

							bool is_Z_Deviation = order_Item.particles_Model.particle_Z_Position_Deviation.value > DBL_EPSILON;

							complex<double> d_Eps = node->delta_Epsilon_Contrast;
							double d_Eps_Norm = norm(d_Eps);
							double G1_Type_Value = G2_node->G1_Type_Outer();
							double G2_Type_Value_Sqrt = sqrt(G1_Type_Value);

							if(is_Z_Deviation)
							{
								for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
								{
									double cos_Phi = measurement.detector_Phi_Cos_Vec[phi_Index];
									double field_With_G_2D_Factor = body_Scattering_Particles_Zero_Correlation_Z_Deviation(thread_Index, item_Index, order_Item.particles_Model.particle_Interference_Function,
																														   G2_node, G1_Type_Value, cos_Theta, cos_Theta_0, cos_Phi);
									calculated_Values.GISAS_Map[phi_Index][point_Index] += e_Factor_PT_2D * d_Eps_Norm * field_With_G_2D_Factor * measurement.footprint_Factor_Vec[point_Index];
								}
							} else
							{
								for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
								{
									double cos_Phi = measurement.detector_Phi_Cos_Vec[phi_Index];
									double field_With_G_2D_Factor = body_Scattering_Particles_Zero_Correlation(thread_Index, item_Index,  order_Item.particles_Model.particle_Interference_Function,
																											   G2_node, G2_Type_Value_Sqrt, cos_Theta, cos_Theta_0, cos_Phi, d_Eps);
									calculated_Values.GISAS_Map[phi_Index][point_Index] += e_Factor_PT_2D * field_With_G_2D_Factor * measurement.footprint_Factor_Vec[point_Index];
								}
							}
						}
						// 10% slower
//						for(size_t phi_Index = measurement.start_Phi_Index; phi_Index<measurement.end_Phi_Number; phi_Index++)
//						{
//							double cos_Phi = measurement.detector_Phi_Cos_Vec[phi_Index];
//							Params_Particles params_Particles = {this, thread_Index, false, common, cos_Theta, cos_Theta_0, &last_Item, last_Node};
//							calculated_Values.GISAS_Map[phi_Index][point_Index] += e_Factor_PT_2D * function_Scattering_Particles_Zero_Correlation(cos_Phi, &params_Particles) * measurement.footprint_Factor_Vec[point_Index];
//						}
					}				
				}
			}
		}
	}
}

double Unwrapped_Reflection::body_Scattering_Particles_Partial_Correlation_Z_Deviation(vector<complex<double>>& layer_Field_Factor, int thread_Index, int item_Index, QString particle_Interference_Function,
																					   Node *last_Node, double G1_Type_Value, double G2_Type_Value, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps)
{
	double q = measurement.k_Value*sqrt(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);

	calc_Item_Form_Factor_From_Spline(thread_Index, item_Index, q);
	if(particle_Interference_Function == radial_Paracrystal)
	{
		G2_Type_Value = last_Node->G2_Type_Outer(q);
		calc_Item_Alfa_Factor_With_G2(thread_Index, item_Index, G2_Type_Value, G1_Type_Value);
	} else
	if(particle_Interference_Function == disorder)
	{
		calc_Item_Alfa_Factor_No_G2(thread_Index, item_Index, G1_Type_Value);
	}
	calc_Coherent_Coef_G2(thread_Index, item_Index, sqrt(G2_Type_Value), d_Eps);

	double incoherent_Field_Factor = 0;

	// for all polarizations
	for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
	{
		layer_Field_Factor[layer_Index] = 0.;
		for (int i=0; i<4; i++)
		{
			// incoherent part
			incoherent_Field_Factor += C_03_norm[thread_Index][layer_Index][i] * alfa_nn_03[thread_Index][item_Index][i];
			for (int j = 0; j<i; j++)
			{
				incoherent_Field_Factor += 2*real(   C_03_03[thread_Index][layer_Index][i][j] *
												   alfa_03_03[thread_Index][item_Index][i][j]);
			}
			// for partially coherent part
			layer_Field_Factor[layer_Index] += C_03[thread_Index][layer_Index][i] *
											   complex_Coef[thread_Index][i];
		}
	}
	return incoherent_Field_Factor;
}
double Unwrapped_Reflection::body_Scattering_Particles_Partial_Correlation(vector<complex<double>>& layer_Field_Factor, int thread_Index, int item_Index, QString particle_Interference_Function,
																		   Node* last_Node, double G2_Type_Value_Sqrt, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps)
{
	double q = measurement.k_Value*sqrt(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);

	calc_Item_Form_Factor_From_Spline(thread_Index, item_Index, q);
	if(particle_Interference_Function == radial_Paracrystal)
	{
		G2_Type_Value_Sqrt = sqrt(last_Node->G2_Type_Outer(q));
	}
	calc_Coherent_Coef_G2(thread_Index, item_Index, G2_Type_Value_Sqrt, d_Eps);

	double incoherent_Field_Factor = 0;
	// for all polarizations
	for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
	{
		complex<double> complex_Incoherent_Field_Factor = 0;
		layer_Field_Factor[layer_Index] = 0;
		for (int i=0; i<4; i++)
		{
			// "incoherent" part (really, self-coherent)
			complex_Incoherent_Field_Factor += C_03[thread_Index][layer_Index][i] *
											   complex_Coef[thread_Index][i];
			// for partially coherent part
			layer_Field_Factor[layer_Index] += C_03[thread_Index][layer_Index][i] *
											   complex_Coef[thread_Index][i];
		}
		incoherent_Field_Factor += norm(complex_Incoherent_Field_Factor);
	}
	return incoherent_Field_Factor;
}

double Unwrapped_Reflection::body_Scattering_Particles_Full_Correlation_Z_Deviation(complex<double>& coherent_Field_Factor, int thread_Index, int item_Index, QString particle_Interference_Function,
																					Node* last_Node, double G1_Type_Value, double G2_Type_Value_Sqrt, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps)
{
	double q = measurement.k_Value*sqrt(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);

	// radial PC/disorder here
	if(particle_Interference_Function == radial_Paracrystal)
	{
		G2_Type_Value_Sqrt = sqrt(last_Node->G2_Type_Outer(q));
	}
	calc_Item_Form_Factor_From_Spline(thread_Index, item_Index, q);
	calc_Item_Alfa_Factor_G1		 (thread_Index, item_Index, G1_Type_Value);
	calc_Coherent_Coef_G2            (thread_Index, item_Index, G2_Type_Value_Sqrt, d_Eps);

	double incoherent_Field_Factor = 0;

	// for all polarizations
	for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
	{
		for (int i=0; i<4; i++)
		{
			// incoherent part
			incoherent_Field_Factor += C_03_norm[thread_Index][layer_Index][i] * alfa_nn_03[thread_Index][item_Index][i];
			for (int j = 0; j<i; j++)
			{
				incoherent_Field_Factor += 2*real( C_03_03[thread_Index][layer_Index][i][j] *
												   alfa_03_03[thread_Index][item_Index][i][j]
												  );
			}
			// coherent part
			coherent_Field_Factor += C_03[thread_Index][layer_Index][i] *
									 complex_Coef[thread_Index][i];
		}
	}
	return incoherent_Field_Factor;
}
void Unwrapped_Reflection::body_Scattering_Particles_Full_Correlation(complex<double>& coherent_Field_Factor, int thread_Index, int item_Index, QString particle_Interference_Function,
																	  Node* last_Node, double G2_Type_Value_Sqrt, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps)
{
	double q = measurement.k_Value*sqrt(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);

	// radial PC/disorder here
	if(particle_Interference_Function == radial_Paracrystal)
	{
		G2_Type_Value_Sqrt = sqrt(last_Node->G2_Type_Outer(q));
	}
	calc_Item_Form_Factor_From_Spline(thread_Index, item_Index, q);
	calc_Coherent_Coef_G2            (thread_Index, item_Index, G2_Type_Value_Sqrt, d_Eps);

	// for all polarizations
	for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
	{
		for (int i=0; i<4; i++)
		{
			// coherent part
			coherent_Field_Factor += C_03[thread_Index][layer_Index][i] *
									 complex_Coef[thread_Index][i];
		}
	}
}

double Unwrapped_Reflection::body_Scattering_Particles_Zero_Correlation_Z_Deviation(int thread_Index, int item_Index, QString particle_Interference_Function, Node* G2_node,
																					double G1_Type_Value, double cos_Theta, double cos_Theta_0, double cos_Phi)
{
	double q = measurement.k_Value*sqrt(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);

	calc_Item_Form_Factor_From_Spline(thread_Index, item_Index, q);
	if(particle_Interference_Function == radial_Paracrystal)
	{
		double G2_Type_Value = G2_node->G2_Type_Outer(q);
		calc_Item_Alfa_Factor_With_G2(thread_Index, item_Index, G2_Type_Value, G1_Type_Value);
	} else
	if(particle_Interference_Function == disorder)
	{
		calc_Item_Alfa_Factor_No_G2(thread_Index, item_Index, G1_Type_Value);
	}

	double field_With_G_2D_Factor = 0;

	// for all polarizations
	for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
	{
		for (int i = 0; i<4; i++)
		{
			field_With_G_2D_Factor += C_03_norm[thread_Index][layer_Index][i] * alfa_nn_03[thread_Index][item_Index][i];
			for (int j = 0; j<i; j++)
			{
				field_With_G_2D_Factor += 2*real(   C_03_03[thread_Index][layer_Index][i][j] *
												 alfa_03_03[thread_Index][item_Index ][i][j] );
			}
		}
	}
	return field_With_G_2D_Factor;
}
double Unwrapped_Reflection::body_Scattering_Particles_Zero_Correlation(int thread_Index, int item_Index, QString particle_Interference_Function, Node* G2_node,
																		double G2_Type_Value_Sqrt, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps)
{
	double q = measurement.k_Value*sqrt(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);

	if(particle_Interference_Function == radial_Paracrystal)
	{
		G2_Type_Value_Sqrt = sqrt(G2_node->G2_Type_Outer(q));		
	}
	calc_Item_Form_Factor_From_Spline(thread_Index, item_Index, q);
	calc_Coherent_Coef_G2            (thread_Index, item_Index, G2_Type_Value_Sqrt, d_Eps);

	double field_With_G_2D_Factor = 0;
	complex<double> sum;

	// for all polarizations
	for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
	{
		sum = 0.;
		for (int i=0; i<4; i++)
		{
			sum += C_03[thread_Index][layer_Index][i] *	complex_Coef[thread_Index][i];
		}
		field_With_G_2D_Factor += norm(sum);
	}
	return field_With_G_2D_Factor;
}
double Unwrapped_Reflection::cor_Function_Integration(int point_Index, int thread_Index, double cos_Theta_0)
{
	function<double(double)> f;
	// TODO partial correlation
	if(multilayer->imperfections_Model.use_Common_Roughness_Function) f = [&](double r){return function_DWBA_SA_CSA_Batch_Common_Integrand    (r, thread_Index);};
	else															  f = [&](double r){return function_DWBA_SA_CSA_Batch_Individual_Integrand(r, thread_Index);};
	double p = measurement.k_Value*abs(cos_Theta_0-measurement.detector_Theta_Cos_Vec[point_Index]);

	double integral = -2020, error;
	if( p > DBL_EPSILON )
	{
		std::pair<double, double> result_Boost = integrator_Vec[thread_Index].integrate(f, p);
		integral = result_Boost.first;
	} else
	{
		integral = gauss_kronrod<double, 61>::integrate(f, 0, std::numeric_limits<double>::infinity(), 5, 1e-7, &error);
	}
	return integral;
}

void Unwrapped_Reflection::calc_k_Wavenumber_Layer(int thread_Index, int point_Index)
{	
	vector<complex<double>>& q0_Hi = if_Single_Beam_Value ? calculated_Values.q0_Hi.front() : calculated_Values.q0_Hi[point_Index];

	// only layers
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size()-1; item_Index++)
	{
		const Data& item = appropriate_Item_Vec[item_Index];
		if(item.particles_Model.is_Used)
		{
			int first_Layer_Of_Item = boundaries_Of_Item_Vec[item_Index].front();
			int boundary_Index = first_Layer_Of_Item + 1;

			k_03[thread_Index][item_Index][0] =  q0_Hi[boundary_Index] + calculated_Values.q_Hi[point_Index][boundary_Index];
			k_03[thread_Index][item_Index][1] =  q0_Hi[boundary_Index] - calculated_Values.q_Hi[point_Index][boundary_Index];
			k_03[thread_Index][item_Index][2] = -k_03[thread_Index][item_Index][1];
			k_03[thread_Index][item_Index][3] = -k_03[thread_Index][item_Index][0];
		}
	}
}

void Unwrapped_Reflection::calc_Half_Layer_Exponents(int thread_Index)
{
	// only layers
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size()-1; item_Index++)
	{
		const Data& item = appropriate_Item_Vec[item_Index];

		if(item.particles_Model.is_Used)
		{
			if( unwrapped_Structure->depth_Grading )
			{
				// calculate for each layer thickness
				for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
				{
					exp_03[thread_Index][layer_Index][0] = exp(-I*k_03[thread_Index][item_Index][0]*unwrapped_Structure->thickness[layer_Index]/2.);
					exp_03[thread_Index][layer_Index][1] = exp(-I*k_03[thread_Index][item_Index][1]*unwrapped_Structure->thickness[layer_Index]/2.);
					exp_03[thread_Index][layer_Index][2] = exp(-I*k_03[thread_Index][item_Index][2]*unwrapped_Structure->thickness[layer_Index]/2.);
					exp_03[thread_Index][layer_Index][3] = exp(-I*k_03[thread_Index][item_Index][3]*unwrapped_Structure->thickness[layer_Index]/2.);
				}
			} else
			{
				// calculate only for unique layers and then copy
				int first_Layer_Of_Item = boundaries_Of_Item_Vec[item_Index].front();

				complex<double> exp_0 = exp(-I*k_03[thread_Index][item_Index][0]*unwrapped_Structure->thickness[first_Layer_Of_Item]/2.);
				complex<double> exp_1 = exp(-I*k_03[thread_Index][item_Index][1]*unwrapped_Structure->thickness[first_Layer_Of_Item]/2.);
				complex<double> exp_2 = exp(-I*k_03[thread_Index][item_Index][2]*unwrapped_Structure->thickness[first_Layer_Of_Item]/2.);
				complex<double> exp_3 = exp(-I*k_03[thread_Index][item_Index][3]*unwrapped_Structure->thickness[first_Layer_Of_Item]/2.);

				for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
				{
					exp_03[thread_Index][layer_Index][0] = exp_0;
					exp_03[thread_Index][layer_Index][1] = exp_1;
					exp_03[thread_Index][layer_Index][2] = exp_2;
					exp_03[thread_Index][layer_Index][3] = exp_3;
				}
			}
		}
	}
}

void Unwrapped_Reflection::calc_C_Factor(QString polarization, int thread_Index, int point_Index)
{
	const vector<complex<double>>& q0_U_i_s = if_Single_Beam_Value ? calculated_Values.q0_U_i_s.front() : calculated_Values.q0_U_i_s[point_Index];
	const vector<complex<double>>& q0_U_r_s = if_Single_Beam_Value ? calculated_Values.q0_U_r_s.front() : calculated_Values.q0_U_r_s[point_Index];
	const vector<complex<double>>& q0_U_i_p = if_Single_Beam_Value ? calculated_Values.q0_U_i_p.front() : calculated_Values.q0_U_i_p[point_Index];
	const vector<complex<double>>& q0_U_r_p = if_Single_Beam_Value ? calculated_Values.q0_U_r_p.front() : calculated_Values.q0_U_r_p[point_Index];

	const vector<complex<double>>& q0_U_i = polarization == "s" ? q0_U_i_s : q0_U_i_p;
	const vector<complex<double>>& q0_U_r = polarization == "s" ? q0_U_r_s : q0_U_r_p;
	const vector<complex<double>>& q_U_i  = polarization == "s" ? calculated_Values.q_U_i_s [point_Index] : calculated_Values.q_U_i_p [point_Index];
	const vector<complex<double>>& q_U_r  = polarization == "s" ? calculated_Values.q_U_r_s [point_Index] : calculated_Values.q_U_r_p [point_Index];

	vector<vector<complex<double>>>&		C_03_pol		= polarization == "s" ? C_03_s			[thread_Index] : C_03_p			[thread_Index];
	vector<vector<vector<complex<double>>>>&C_03_03_pol		= polarization == "s" ? C_03_03_s		[thread_Index] : C_03_03_p		[thread_Index];
//	vector<vector<vector<double>>>&			C_03_03_real= polarization == "s" ? C_03_03_s_real  [thread_Index] : C_03_03_p_real [thread_Index];
//	vector<vector<vector<double>>>&			C_03_03_imag= polarization == "s" ? C_03_03_s_imag  [thread_Index] : C_03_03_p_imag [thread_Index];
	vector<vector<double>>&					C_03_norm_pol	= polarization == "s" ? C_03_s_norm		[thread_Index] : C_03_p_norm	[thread_Index];

	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size()-1; item_Index++)
	{
		const Data& item = appropriate_Item_Vec[item_Index];

		if(item.particles_Model.is_Used)
		{
			for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
			{
				int boundary_Index = layer_Index + 1;

//				C_03[layer_Index][0] = q0_U_i[boundary_Index]*q_U_i[boundary_Index]*exp(-I*k_03[thread_Index][item_Index][0]*unwrapped_Structure->thickness[layer_Index]/2.);
//				C_03[layer_Index][1] = q0_U_i[boundary_Index]*q_U_r[boundary_Index]*exp(-I*k_03[thread_Index][item_Index][1]*unwrapped_Structure->thickness[layer_Index]/2.);
//				C_03[layer_Index][2] = q0_U_r[boundary_Index]*q_U_i[boundary_Index]*exp(-I*k_03[thread_Index][item_Index][2]*unwrapped_Structure->thickness[layer_Index]/2.);
//				C_03[layer_Index][3] = q0_U_r[boundary_Index]*q_U_r[boundary_Index]*exp(-I*k_03[thread_Index][item_Index][3]*unwrapped_Structure->thickness[layer_Index]/2.);

				C_03_pol[layer_Index][0] = q0_U_i[boundary_Index]*q_U_i[boundary_Index]*exp_03[thread_Index][layer_Index][0];
				C_03_pol[layer_Index][1] = q0_U_i[boundary_Index]*q_U_r[boundary_Index]*exp_03[thread_Index][layer_Index][1];
				C_03_pol[layer_Index][2] = q0_U_r[boundary_Index]*q_U_i[boundary_Index]*exp_03[thread_Index][layer_Index][2];
				C_03_pol[layer_Index][3] = q0_U_r[boundary_Index]*q_U_r[boundary_Index]*exp_03[thread_Index][layer_Index][3];

				for (int i = 0; i<4; i++)
				{
					C_03_norm_pol[layer_Index][i] = norm(C_03_pol[layer_Index][i]);
					for (int j = 0; j<i; j++)
					{
						C_03_03_pol[layer_Index][i][j] = C_03_pol[layer_Index][i] * conj(C_03_pol[layer_Index][j]);
					}
				}
			}
		}
	}
}

void Unwrapped_Reflection::fill_C_Factor_Mixed(QString polarization, int thread_Index)
{
	vector<vector<complex<double>>>&		C_03_pol		= polarization == "s" ? C_03_s			[thread_Index] : C_03_p			[thread_Index];
	vector<vector<vector<complex<double>>>>&C_03_03_pol		= polarization == "s" ? C_03_03_s		[thread_Index] : C_03_03_p		[thread_Index];
	vector<vector<double>>&					C_03_norm_pol	= polarization == "s" ? C_03_s_norm		[thread_Index] : C_03_p_norm	[thread_Index];

	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size()-1; item_Index++)
	{
		const Data& item = appropriate_Item_Vec[item_Index];

		if(item.particles_Model.is_Used)
		{
			for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
			{
				for (int i = 0; i<4; i++)
				{
					C_03[thread_Index][layer_Index][i] = C_03_pol[layer_Index][i];
					C_03_norm[thread_Index][layer_Index][i] = C_03_norm_pol[layer_Index][i];
					for (int j = 0; j<i; j++)
					{
						C_03_03[thread_Index][layer_Index][i][j] = C_03_03_pol[layer_Index][i][j];
					}
				}
			}
		}
	}
}

void Unwrapped_Reflection::calc_C_Factor_Mixed(int thread_Index)
{
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size()-1; item_Index++)
	{
		const Data& item = appropriate_Item_Vec[item_Index];

		if(item.particles_Model.is_Used)
		{
			for(int& layer_Index : boundaries_Of_Item_Vec[item_Index])
			{
				for (int i = 0; i<4; i++)
				{
					C_03[thread_Index][layer_Index][i] = s_Weight * C_03_s[thread_Index][layer_Index][i] + p_Weight * C_03_p[thread_Index][layer_Index][i];
					C_03_norm[thread_Index][layer_Index][i] = norm(C_03[thread_Index][layer_Index][i]);
					for (int j = 0; j<i; j++)
					{
						C_03_03[thread_Index][layer_Index][i][j] = C_03[thread_Index][layer_Index][i] * conj(C_03[thread_Index][layer_Index][j]);
					}
				}
			}
		}
	}
}

void Unwrapped_Reflection::calc_Omega_Factor(int thread_Index)
{
	bool common = multilayer->imperfections_Model.use_Common_Particle_Function;
	Data& last_Item = appropriate_Item_Vec[appropriate_Item_Vec.size()-2];

	// only layers
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size()-1; item_Index++)
	{
		Data& item = appropriate_Item_Vec[item_Index];
		Data& order_Item = common ? last_Item : item;

		if(item.particles_Model.is_Used)
		{
			for(int i=0; i<4; i++)
			{
				w_03[thread_Index][item_Index][i] = Global_Variables::omega_Factor(k_03[thread_Index][item_Index][i],
																				   order_Item.particles_Model.particle_Z_Position_Deviation.value);
			}
		}
	}
}

void Unwrapped_Reflection::calc_Gamma_Factor(int thread_Index)
{
	bool common = multilayer->imperfections_Model.use_Common_Particle_Function;
	Data& last_Item = appropriate_Item_Vec[appropriate_Item_Vec.size()-2];

	// only layers
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size()-1; item_Index++)
	{
		Data& item = appropriate_Item_Vec[item_Index];
		Data& order_Item = common ? last_Item : item;

		if(item.particles_Model.is_Used)
		{
			for(int i=0; i<4; i++)
			{
				for(int j=0; j<4; j++)
				{
					g_03_03[thread_Index][item_Index][i][j] = Global_Variables::gamma_Factor(k_03[thread_Index][item_Index][i],
																							 k_03[thread_Index][item_Index][j],
																							 order_Item.particles_Model.particle_Z_Position_Deviation.value);
				}
			}
		}
	}
}

void Unwrapped_Reflection::choose_Form_Factor_2D_Function(int thread_Index)
{
	// only layers, without substrate
	for(size_t item_Index = 0; item_Index<appropriate_Item_Vec.size()-1; item_Index++)
	{
		Data& item = appropriate_Item_Vec[item_Index];

		if(item.particles_Model.particle_Shape == full_Sphere)
		{
			prefactor_2D_Func_Vec  [thread_Index][item_Index] = Global_Variables::full_Sphere_FF_Prefactor;
			q_Factor_2D_Func_Vec   [thread_Index][item_Index] = Global_Variables::full_Sphere_FF_q_Factor;
			form_Factor_2D_Func_Vec[thread_Index][item_Index] = Global_Variables::full_Sphere_FF;
		} else
		if(item.particles_Model.particle_Shape == full_Spheroid)
		{
			prefactor_2D_Func_Vec  [thread_Index][item_Index] = Global_Variables::full_Spheroid_FF_Prefactor;
			q_Factor_2D_Func_Vec   [thread_Index][item_Index] = Global_Variables::full_Spheroid_FF_q_Factor;
			form_Factor_2D_Func_Vec[thread_Index][item_Index] = Global_Variables::full_Spheroid_FF;
		} else
		if(item.particles_Model.particle_Shape == cylinder)
		{
			prefactor_2D_Func_Vec  [thread_Index][item_Index] = Global_Variables::cylinder_FF_Prefactor;
			q_Factor_2D_Func_Vec   [thread_Index][item_Index] = Global_Variables::cylinder_FF_q_Factor;
			form_Factor_2D_Func_Vec[thread_Index][item_Index] = Global_Variables::cylinder_FF;
		}
	}
}

void Unwrapped_Reflection::calc_Item_Pre_Form_Factor(int thread_Index, int item_Index, Data& item)
{
	for(int i=0; i<4; i++)
	{
		F_pre_03[thread_Index][item_Index][i] = prefactor_2D_Func_Vec[thread_Index][item_Index]
													(k_03[thread_Index][item_Index][i],
													 item.particles_Model.particle_Radius.value,
													 item.particles_Model.particle_Height.value,
													 item.particles_Model.particle_Z_Position.value);
	}
}

void Unwrapped_Reflection::calc_Item_Form_Factor(int thread_Index, int item_Index, double q, Data& item)
{
	for(int i=0; i<4; i++)
	{
		/// slower
//		F_03[thread_Index][item_Index][i] = form_Factor_2D_Func_Vec[thread_Index][item_Index]
//													(q,
//													 k_03[thread_Index][item_Index][i],
//													 item.particles_Model.particle_Radius.value,
//													 item.particles_Model.particle_Height.value,
//													 item.particles_Model.particle_Z_Position.value);
		/// faster
		F_03[thread_Index][item_Index][i] = F_pre_03[thread_Index][item_Index][i] *
											q_Factor_2D_Func_Vec[thread_Index][item_Index]
													(q,
													 k_03[thread_Index][item_Index][i],
													 item.particles_Model.particle_Radius.value,
													 item.particles_Model.particle_Height.value,
													 item.particles_Model.particle_Z_Position.value);
	}
}

void Unwrapped_Reflection::calc_Item_Form_Factor_From_Spline(int thread_Index, int item_Index, double q)
{
	for(int i=0; i<4; i++)
	{
		double F_Real = gsl_spline_eval(spline_F_03_Real[thread_Index][item_Index][i], q,
										   acc_F_03_Real[thread_Index][item_Index][i]);
		double F_Imag = gsl_spline_eval(spline_F_03_Imag[thread_Index][item_Index][i], q,
										   acc_F_03_Imag[thread_Index][item_Index][i]);
		F_03[thread_Index][item_Index][i] = complex<double>(F_Real, F_Imag);
	}
}

void Unwrapped_Reflection::calc_Item_Form_Factor_Splines(int thread_Index, int item_Index, int order_Item_Index, Data& order_Item, double cos_Theta_0, double cos_Theta)
{
	double cos_Phi_Min = 1; // start from zero phi
	double q_Min = measurement.k_Value*sqrt(abs(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi_Min));
	q_Min = max(q_Min, DBL_EPSILON); // not zero, zero added additionally
//	q_Min = max(q_Min, 0.);
//	double q_Min = 0;

	double cos_Phi_Max = 0;
	if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		double phi_Max_Radians = qDegreesToRadians(max_Phi_Azimuthal_Integration);

		// finite slit length: set phi_Max
		if(measurement.detector_1D.finite_Slit)
		{
			phi_Max_Radians = Global_Variables::get_Phi_Max_From_Finite_Slit(measurement, cos_Theta);
		}
		cos_Phi_Max = cos(phi_Max_Radians);
	}
	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		cos_Phi_Max = min(measurement.detector_Phi_Cos_Vec.front(), measurement.detector_Phi_Cos_Vec.back());
	}
	double q_Max = measurement.k_Value*sqrt(abs(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi_Max));
	q_Max = max(q_Max, (q_Min+DBL_EPSILON)*1.01);

	double R = order_Item.particles_Model.particle_Radius.value;
	double H = order_Item.particles_Model.particle_Height.value;
	double z = order_Item.particles_Model.particle_Z_Position.value;

	// splining points
	int max_Arg_Length = q_Spline_Points-3;//-2;
	int q_Spline_Points_No_Edge = min(max_Arg_Length, int((q_Max-q_Min)*R*1.2)); // -3
	q_Spline_Points_No_Edge = max(q_Spline_Points_No_Edge, 21); // at least 10 effective points

	double step = (q_Max-q_Min) / (q_Spline_Points_No_Edge-1);
	vector<double> arg(max_Arg_Length);
	vector<double> val_Real(max_Arg_Length, 0); // reuse for each i = 0..3
	vector<double> val_Imag(max_Arg_Length, 0); // reuse for each i = 0..3
	// main part
	for(int j=0; j<q_Spline_Points_No_Edge; j++) {
		arg[j] = q_Min + j*step;
	}
	// empty rest
	for(int j=q_Spline_Points_No_Edge; j<max_Arg_Length; j++) {
		arg[j] = arg[j-1] + j*step;
	}
	{
		arg.insert(arg.begin(), 0);
		val_Real.insert(val_Real.begin(), 0); // will be recalculated
		val_Imag.insert(val_Imag.begin(), 0); // will be recalculated
	}
	{
		arg.push_back(arg.back()*(1+1E-5));
		arg.push_back(DBL_MAX);
		val_Real.push_back(0);
		val_Real.push_back(0);
		val_Imag.push_back(0);
		val_Imag.push_back(0);
	}

	for(int i=0; i<4; i++)
	{
		// values
		complex<double> prefactor = prefactor_2D_Func_Vec[thread_Index][order_Item_Index](k_03[thread_Index][item_Index][i], R, H, z);

		// main part
		for(int j=0; j<q_Spline_Points_No_Edge+2; j++) // with zero point, but except last DBL_MAX points
		{
			complex<double> val_Complex = prefactor*q_Factor_2D_Func_Vec[thread_Index][order_Item_Index](arg[j], k_03[thread_Index][item_Index][i], R, H, z);
			val_Real[j] = real(val_Complex);
			val_Imag[j] = imag(val_Complex);
		}
		// empty rest: for j=q_Spline_Points_No_Edge; j<max_Arg_Length do nothing

		// splines
		gsl_spline_init(spline_F_03_Real[thread_Index][item_Index][i], arg.data(), val_Real.data(), arg.size());
		gsl_spline_init(spline_F_03_Imag[thread_Index][item_Index][i], arg.data(), val_Imag.data(), arg.size());
	}
}

void Unwrapped_Reflection::calc_Item_Alfa_Factor_With_G2(int thread_Index, int item_Index, double G2_Type_Value, double G1_Type_Value)
{
	double g_nn, w_n2, F_n2;
	complex<double> g_ij, w_i, w_ijc, F_i, F_ijc;
	for(int i=0; i<4; i++)
	{
		// diagonal part
		g_nn = real(g_03_03[thread_Index][item_Index][i][i]);
		w_n2 = norm(   w_03[thread_Index][item_Index][i]);
		F_n2 = norm(   F_03[thread_Index][item_Index][i]);
		alfa_nn_03[thread_Index][item_Index][i] = F_n2 * (G1_Type_Value * g_nn + w_n2 * (G2_Type_Value - G1_Type_Value));

		// non-diagonal part
		w_i = w_03[thread_Index][item_Index][i];
		F_i = F_03[thread_Index][item_Index][i];
		for(int j=0; j<i; j++)
		{
			g_ij  =       g_03_03[thread_Index][item_Index][i][j];
			w_ijc = w_i*conj(w_03[thread_Index][item_Index][j]);
			F_ijc = F_i*conj(F_03[thread_Index][item_Index][j]);			
			alfa_03_03[thread_Index][item_Index][i][j] = F_ijc * (G1_Type_Value * g_ij + w_ijc * (G2_Type_Value - G1_Type_Value));
		}
	}
}

void Unwrapped_Reflection::calc_Item_Alfa_Factor_No_G2(int thread_Index, int item_Index, double G1_Type_Value)
{
	double g_nn, w_n2, F_n2;
	complex<double> g_ij, w_i, w_ijc, F_i, F_ijc;

	for(int i=0; i<4; i++)
	{
		// diagonal part
		g_nn = real(g_03_03[thread_Index][item_Index][i][i]);
//		w_n2 = norm(   w_03[thread_Index][item_Index][i]);
		F_n2 = norm(   F_03[thread_Index][item_Index][i]);
		alfa_nn_03[thread_Index][item_Index][i] = F_n2 * (G1_Type_Value * g_nn);

		// non-diagonal part
//		w_i = w_03[thread_Index][item_Index][i];
		F_i = F_03[thread_Index][item_Index][i];
		for(int j=0; j<i; j++)
		{
			g_ij  =       g_03_03[thread_Index][item_Index][i][j];
//			w_ijc = w_i*conj(w_03[thread_Index][item_Index][j]);
			F_ijc = F_i*conj(F_03[thread_Index][item_Index][j]);
			alfa_03_03	   [thread_Index][item_Index][i][j] = F_ijc * (G1_Type_Value * g_ij);
		}
	}
}

void Unwrapped_Reflection::calc_Item_Alfa_Factor_G1(int thread_Index, int item_Index, double G1_Type_Value)
{
	double g_nn, w_n2, F_n2;
	complex<double> g_ij, w_i, w_ijc, F_i, F_ijc;
	for(int i=0; i<4; i++)
	{
		// diagonal part
		g_nn = real(g_03_03[thread_Index][item_Index][i][i]);
		w_n2 = norm(   w_03[thread_Index][item_Index][i]);
		F_n2 = norm(   F_03[thread_Index][item_Index][i]);
		alfa_nn_03[thread_Index][item_Index][i] = F_n2 * (G1_Type_Value * (g_nn - w_n2));

		// non-diagonal part
		w_i = w_03[thread_Index][item_Index][i];
		F_i = F_03[thread_Index][item_Index][i];
		for(int j=0; j<i; j++)
		{
			g_ij  =       g_03_03[thread_Index][item_Index][i][j];
			w_ijc = w_i*conj(w_03[thread_Index][item_Index][j]);
			F_ijc = F_i*conj(F_03[thread_Index][item_Index][j]);
			alfa_03_03	   [thread_Index][item_Index][i][j] = F_ijc * (G1_Type_Value * (g_ij - w_ijc));
		}
	}
}

void Unwrapped_Reflection::calc_Coherent_Coef_G2(int thread_Index, int item_Index, double G2_Type_Value_Sqrt, complex<double> d_Eps)
{
	for(int i=0; i<4; i++)
	{
		complex_Coef[thread_Index][i] = F_03[thread_Index][item_Index][i] *
										w_03[thread_Index][item_Index][i] *
										d_Eps * G2_Type_Value_Sqrt;
	}
}

double Unwrapped_Reflection::particles_Azimuthal_Integration(gsl_function* function, double cos_Theta_0, int point_Index)
{
	double cos_Theta = measurement.detector_Theta_Cos_Vec[point_Index];
	double phi_Min = 0;
	double phi_Max = max_Phi_Azimuthal_Integration;

	// finite slit length: set phi_Max
	if(measurement.detector_1D.finite_Slit)
	{
		phi_Max = qRadiansToDegrees(Global_Variables::get_Phi_Max_From_Finite_Slit(measurement, cos_Theta));
	}
//	double cos_Phi_Max = cos(qDegreesToRadians(min(5., phi_Max)));
//	double q_Max = measurement.k_Value*sqrt(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi_Max);

	/// ----------------------------------------------------------------------------------------------------------------------

	// partition
	vector<double> q_Peak, hw_Peak;
	bool common = multilayer->imperfections_Model.use_Common_Particle_Function;
	bool disorder_Model = true;
	double max_R = 0;
	if(common)
	{
		Node* G2_Last_Node = short_Flat_Calc_Tree[short_Flat_Calc_Tree.size()-2];
		q_Peak = G2_Last_Node->q_Peak;
		hw_Peak = G2_Last_Node->hw_Peak;

		Data& order_Item = appropriate_Item_Vec[short_Flat_Calc_Tree.size()-2];
		disorder_Model = disorder_Model && order_Item.particles_Model.particle_Interference_Function == disorder;
		max_R = max(max_R, order_Item.particles_Model.particle_Radius.value);
	} else
	{
		for(int item_Index : used_Appropriate_Item_Index_Vec)
		{
			Node* G2_node = short_Flat_Calc_Tree[item_Index];
			vector<double>& q_Peak_Node  = G2_node->q_Peak;
			vector<double>& hw_Peak_Node = G2_node->hw_Peak;

			if(q_Peak_Node.size()>0)
			{
				 q_Peak.insert(  q_Peak.end(),  q_Peak_Node.begin(),  q_Peak_Node.end() );
				hw_Peak.insert( hw_Peak.end(), hw_Peak_Node.begin(), hw_Peak_Node.end() );
			}

			Data& order_Item = appropriate_Item_Vec[item_Index];
			disorder_Model = disorder_Model && order_Item.particles_Model.particle_Interference_Function == disorder;
			max_R = max(max_R, order_Item.particles_Model.particle_Radius.value);
		}
	}

	// min phi
	if(!disorder_Model)
	{
		double q_Fail = 0.002; // empirical value. defends from non-physical values at phi=0 at low theta
		double fraction_Fail = (pow(cos_Theta,2) + pow(cos_Theta_0,2) - pow(q_Fail/measurement.k_Value,2))/(2*cos_Theta*cos_Theta_0);
		if(fraction_Fail<1) {
			phi_Min = qRadiansToDegrees(acos(fraction_Fail));
		}
	}

	// one point left and one point right from each peak
	double hw_Factor = 3;
	vector<double> q_Points;
	for(size_t i=0; i<q_Peak.size(); i++)
	{
		q_Points.push_back(q_Peak[i] - hw_Factor*hw_Peak[i]);
		q_Points.push_back(q_Peak[i] + hw_Factor*hw_Peak[i]);
	}

	// make phi from q
	vector<double> phi_Interpoints = {phi_Min};
	for(size_t i=0; i<q_Points.size(); i++)
	{
		if(q_Points[i]>0)
		{
			double fraction = (pow(cos_Theta,2) + pow(cos_Theta_0,2) - pow(q_Points[i]/measurement.k_Value,2))/(2*cos_Theta*cos_Theta_0);
			double phi = qRadiansToDegrees(acos(fraction));
			if(phi>phi_Min) phi_Interpoints.push_back(phi);
		}
	}
	// restrict phi by phi_Max
	for(int i=phi_Interpoints.size()-1; i>=0; i--)
	{
		if(phi_Interpoints[i]>=phi_Max) phi_Interpoints.erase(phi_Interpoints.begin()+i);
		if(phi_Interpoints[i]< phi_Min) phi_Interpoints.erase(phi_Interpoints.begin()+i);
	}
	phi_Interpoints.push_back(phi_Max);
	std::sort(phi_Interpoints.begin(), phi_Interpoints.end());

	// integration
	auto f = [&](double phi){return function->function(phi, function->params);};
	double result = 0;
	tanh_sinh<double> integrator;
	double integrator_Tolerance = 1E-1;
	if(disorder_Model) integrator_Tolerance = 1E-3;
//	if(q_Max*max_R > 10  && q_Max*max_R <= 50) integrator_Tolerance_1 = 1E-3;
//	if(q_Max*max_R > 50 && q_Max*max_R <= 200) integrator_Tolerance_1 = 1E-4;
//	if(q_Max*max_R > 200					 ) integrator_Tolerance_1 = 1E-5;
//	double integrator_Tolerance_2 = 2E-2;
//	double integrator_Tolerance_3 = 4E-2;

	for(size_t i=0; i<phi_Interpoints.size()-1; i++)
	{
		if(phi_Interpoints[i]<phi_Interpoints[i+1])
		{
			if(measurement.measurement_Type != measurement_Types[Offset_Scan] || measurement.detector_Theta_Offset>0.01)
			{
					result +=				 integrator.integrate(f, phi_Interpoints[i], phi_Interpoints[i+1],    integrator_Tolerance);
			} else
			{
					result += gauss_kronrod<double,121>::integrate(f, phi_Interpoints[i], phi_Interpoints[i+1], 0, integrator_Tolerance);
			}
			/// before first peak at low theta
//			if(i==0 && phi_Interpoints[i+1]<6) {
//					result +=				 integrator.integrate(f, phi_Interpoints[i], phi_Interpoints[i+1],    integrator_Tolerance);
//					result += gauss_kronrod<double,61>::integrate(f, phi_Interpoints[i], phi_Interpoints[i+1], 0, integrator_Tolerance);
//			} else {
				/// first peak at low theta
//				if(i==1 && phi_Interpoints[i+1]<6) {
//					result +=				 integrator.integrate(f, phi_Interpoints[i], phi_Interpoints[i+1],    integrator_Tolerance);
//					result += gauss_kronrod<double,31>::integrate(f, phi_Interpoints[i], phi_Interpoints[i+1], 0, integrator_Tolerance);
				/// other parts
//				} else {
//					result +=				 integrator.integrate(f, phi_Interpoints[i], phi_Interpoints[i+1],    integrator_Tolerance);
//					result += gauss_kronrod<double,31>::integrate(f, phi_Interpoints[i], phi_Interpoints[i+1], 0, integrator_Tolerance);
//				}
//			}
		}
	}
	return 2*qDegreesToRadians(result);
}

double Unwrapped_Reflection::roughness_Azimuthal_Integration(gsl_function* function, double cos_Theta_0, int point_Index)
{
	double cos_Theta = measurement.detector_Theta_Cos_Vec[point_Index];
	double delta = abs(cos_Theta-cos_Theta_0);
	double phi_Min = 0, phi_Max = max_Phi_Azimuthal_Integration;

	// finite slit length: set phi_Max
	if(measurement.detector_1D.finite_Slit)
	{
		phi_Max = qRadiansToDegrees(Global_Variables::get_Phi_Max_From_Finite_Slit(measurement, cos_Theta));
	}
	double result = 0;
	/// ----------------------------------------------------------------------------------------------------------------------

	// partition
	vector<double> phi_Inter_1 = {phi_Min, 0.002, 0.01, 0.1, 1, 5, 10, 20};
	vector<double> phi_Inter_2 = {phi_Min, 0.05,  0.3,       2, 7,     20};

	// add peak points
	if(multilayer->imperfections_Model.add_Gauss_Peak && !set_PSD_to_1)
	{
		double max_Peak_Frequency =     substrate.roughness_Model.peak_Frequency.value + 2*substrate.roughness_Model.peak_Frequency_Width.value;
		double min_Peak_Frequency = max(substrate.roughness_Model.peak_Frequency.value - 2*substrate.roughness_Model.peak_Frequency_Width.value, 0.);

		double cos_Phi_Max_Peak = (cos_Theta_0*cos_Theta_0 + measurement.detector_Theta_Cos2_Vec[point_Index] - pow(measurement.lambda_Value*max_Peak_Frequency,2))/
								  (2*cos_Theta_0*cos_Theta);
		double cos_Phi_Min_Peak = (cos_Theta_0*cos_Theta_0 + measurement.detector_Theta_Cos2_Vec[point_Index] - pow(measurement.lambda_Value*min_Peak_Frequency,2))/
								  (2*cos_Theta_0*cos_Theta);

		cos_Phi_Max_Peak = min(cos_Phi_Max_Peak, 1.);
		cos_Phi_Min_Peak = min(cos_Phi_Min_Peak, 1.);

		cos_Phi_Max_Peak = max(cos_Phi_Max_Peak, 0.);
		cos_Phi_Min_Peak = max(cos_Phi_Min_Peak, 0.);

		double phi_Max_Peak = qRadiansToDegrees(acos(cos_Phi_Max_Peak));
		double phi_Min_Peak = qRadiansToDegrees(acos(cos_Phi_Min_Peak));

		// add points
		phi_Inter_1.push_back(phi_Min_Peak);
		phi_Inter_1.push_back(phi_Max_Peak);

		phi_Inter_2.push_back(phi_Min_Peak);
		phi_Inter_2.push_back(phi_Max_Peak);
	}
	// add measured points
	if(multilayer->imperfections_Model.add_Measured_PSD_2D && multilayer->imperfections_Model.PSD_2D.is_Loaded() && !set_PSD_to_1)
	{
		double max_Measured_2D_Frequency = multilayer->imperfections_Model.PSD_2D.argument.front();
		double min_Measured_2D_Frequency = multilayer->imperfections_Model.PSD_2D.argument.back();

		double cos_Measured_2D_Max = (cos_Theta_0*cos_Theta_0 + measurement.detector_Theta_Cos2_Vec[point_Index] - pow(measurement.lambda_Value*max_Measured_2D_Frequency,2))/
									 (2*cos_Theta_0*cos_Theta);
		double cos_Measured_2D_Min = (cos_Theta_0*cos_Theta_0 + measurement.detector_Theta_Cos2_Vec[point_Index] - pow(measurement.lambda_Value*min_Measured_2D_Frequency,2))/
									 (2*cos_Theta_0*cos_Theta);

		cos_Measured_2D_Max = min(cos_Measured_2D_Max, 1.);
		cos_Measured_2D_Min = min(cos_Measured_2D_Min, 1.);

		cos_Measured_2D_Max = max(cos_Measured_2D_Max, 0.);
		cos_Measured_2D_Min = max(cos_Measured_2D_Min, 0.);

		double phi_Measured_2D_Max = qRadiansToDegrees(acos(cos_Measured_2D_Max));
		double phi_Measured_2D_Min = qRadiansToDegrees(acos(cos_Measured_2D_Min));

		// add points
		phi_Inter_1.push_back(phi_Measured_2D_Max);
		phi_Inter_1.push_back(phi_Measured_2D_Min);

		phi_Inter_2.push_back(phi_Measured_2D_Max);
		phi_Inter_2.push_back(phi_Measured_2D_Min);
	}

	// choose and sort points
	vector<double> phi_Inter;
	if(delta<1e-4) {
		phi_Inter = phi_Inter_1;
	} else {
		phi_Inter = phi_Inter_2;
	}
	std::sort(phi_Inter.begin(), phi_Inter.end());

	// restrict phi by phi_Max
	for(int i=phi_Inter.size()-1; i>=0; i--)
	{
		if(phi_Inter[i]>=phi_Max) phi_Inter.erase(phi_Inter.begin()+i);
	}
	phi_Inter.push_back(phi_Max);

	// integration
	auto f = [&](double phi){return function->function(phi, function->params);};
	tanh_sinh<double> integrator;
	double integrator_Tolerance = 1E-2;
	for(size_t i=0; i<phi_Inter.size()-1; i++)
	{
		if(phi_Inter[i]<phi_Inter[i+1])
		{
//			result += integrator.integrate(f, phi_Inter[i],	phi_Inter[i+1], integrator_Tolerance); // too slow
			if(i==0) {
				result += integrator.integrate(f, phi_Inter[i],	phi_Inter[i+1], integrator_Tolerance);
//				result += gauss_kronrod<double,15>::integrate(f, phi_Inter[i], phi_Inter[i+1], 0, integrator_Tolerance);
			} else {
				result += gauss_kronrod<double,5>::integrate(f, phi_Inter[i], phi_Inter[i+1], 0, integrator_Tolerance);
			}
		}
	}

	/// previous realization, less flexible
//	{
//		auto f = [&](double phi){return function->function(phi, function->params);};
//		double error;
//		if(delta<1e-4)
//		{
//			double phi_Inter_1 = 0.002, phi_Inter_2 = 0.01, phi_Inter_3 = 0.1, phi_Inter_4 = 1, phi_Inter_5 = 5, phi_Inter_6 = 20;
//			result += gauss_kronrod<double,15>::integrate(f, phi_Min,     phi_Inter_1, 0, 1e-7, &error);
//			result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_1, phi_Inter_2, 0, 1e-7, &error);
//			result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_2, phi_Inter_3, 0, 1e-7, &error);
//			result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_3, phi_Inter_4, 0, 1e-7, &error);
////			if(mouse_wheel_spinbox_structure_table)
//			{
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_4, phi_Inter_5, 0, 1e-7, &error);
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_5, phi_Inter_6, 0, 1e-7, &error);
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_6, phi_Max,	   0, 1e-7, &error);
//			}
//		} else
//		if(delta<5e-4)
//		{
//			double phi_Inter_1 = 0.05, phi_Inter_2 = 0.3, phi_Inter_3 = 1, phi_Inter_4 = 7, phi_Inter_5 = 20;
//			result += gauss_kronrod<double,11>::integrate(f, phi_Min,     phi_Inter_1, 0, 1e-7, &error);
//			result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_1, phi_Inter_2, 0, 1e-7, &error);
//			result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_2, phi_Inter_3, 0, 1e-7, &error);
////			if(mouse_wheel_spinbox_structure_table)
//			{
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_3, phi_Inter_4, 0, 1e-7, &error);
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_4, phi_Inter_5, 0, 1e-7, &error);
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_5, phi_Max,	   0, 1e-7, &error);
//			}
//		} else
//		{
//			double phi_Inter_1 = 0.05, phi_Inter_2 = 0.2, phi_Inter_3 = 1, phi_Inter_4 = 10, phi_Inter_5 = 20;
//			result += gauss_kronrod<double, 5>::integrate(f, phi_Min,     phi_Inter_1, 0, 1e-7, &error);
//			result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_1, phi_Inter_2, 0, 1e-7, &error);
//			result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_2, phi_Inter_3, 0, 1e-7, &error);
////			if(mouse_wheel_spinbox_structure_table)
//			{
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_3, phi_Inter_4, 0, 1e-7, &error);
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_4, phi_Inter_5, 0, 1e-7, &error);
//				result += gauss_kronrod<double, 5>::integrate(f, phi_Inter_5, phi_Max,	   0, 1e-7, &error);
//			}
//		}
//	}
	/// ----------------------------------------------------------------------------------------------------------------------

	// slower
//	double abserr, temp, result, epsabs = 1e0, epsrel = 1e0;
//	gsl_integration_glfixed_table* t = gsl_integration_glfixed_table_alloc(3);
//	size_t neval;
//	if(delta<1e-4)
//	{
//		double phi_Inter_1 = 0.01, phi_Inter_2 = 0.05, phi_Inter_3 = 0.2, phi_Inter_4 = max_Phi_Azimuthal_Integration;
//		gsl_integration_qng(function, phi_Min,     phi_Inter_1, epsabs, epsrel, &result, &abserr, &neval); temp=result;
//		gsl_integration_qng(function, phi_Inter_1, phi_Inter_2, epsabs, epsrel, &result, &abserr, &neval); temp+=result;
//		gsl_integration_qng(function, phi_Inter_2, phi_Inter_3, epsabs, epsrel, &result, &abserr, &neval); temp+=result;
//		gsl_integration_qng(function, phi_Inter_3, phi_Inter_4, epsabs, epsrel, &result, &abserr, &neval); temp+=result;

//		result = temp + gsl_integration_glfixed(function, phi_Inter_4, phi_Max, t);
//	} else
//	if(delta<5e-4)
//	{
//		double phi_Inter_1 = 0.5, phi_Inter_2 = max_Phi_Azimuthal_Integration;
//		gsl_integration_qng(function, phi_Min,     phi_Inter_1, epsabs, epsrel, &result, &abserr, &neval); temp=result;
//		gsl_integration_qng(function, phi_Inter_1, phi_Inter_2, epsabs, epsrel, &result, &abserr, &neval); temp+=result;

//		result = temp + gsl_integration_glfixed(function, phi_Inter_2, phi_Max, t);
//	} else
//	{
//		double phi_Inter_1 = 4, phi_Inter_2 = max_Phi_Azimuthal_Integration;
//		gsl_integration_qng(function, phi_Min, phi_Inter_1, epsabs, epsrel, &result, &abserr, &neval); temp=result;

//		temp   = temp + gsl_integration_glfixed(function, phi_Inter_1, phi_Inter_2, t);
//		result = temp + gsl_integration_glfixed(function, phi_Inter_2, phi_Max,     t);
//	}
//	gsl_integration_glfixed_table_free(t);

	/// ----------------------------------------------------------------------------------------------------------------------

	// the slowest
//	double abserr, result, epsabs = 1e0, epsrel = 1e0;
//	gsl_integration_workspace* w = gsl_integration_workspace_alloc(100);
//	gsl_integration_qag(function, phi_Min, phi_Max, epsabs, epsrel, w->limit, GSL_INTEG_GAUSS61, w, &result, &abserr);
//	gsl_integration_workspace_free(w);

	return 2*qDegreesToRadians(result);
}

void Unwrapped_Reflection::fill_Specular_Values(int thread_Index, int point_Index)
{
	// reflectance
	if(	spec_Scat_mode == SPECULAR_MODE &&
	   (unwrapped_Structure->calc_Functions.check_Reflectance ||
		unwrapped_Structure->calc_Functions.check_Absorptance ||
		unwrapped_Structure->calc_Functions.check_Scattering  ||
		unwrapped_Structure->calc_Functions.check_GISAS) )
	{
		complex<double> r_s = r_Local_s[thread_Index][0];
		complex<double> r_p = r_Local_p[thread_Index][0];

		calculated_Values.Phi_R_s[point_Index] = qRadiansToDegrees(arg(r_s));
		calculated_Values.Phi_R_p[point_Index] = qRadiansToDegrees(arg(r_p));
		calculated_Values.R_s	 [point_Index] = norm(r_s)*specular_Debye_Waller_Weak_Factor_R_Final[thread_Index];
		calculated_Values.R_p	 [point_Index] = norm(r_p)*specular_Debye_Waller_Weak_Factor_R_Final[thread_Index];
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
	if(	measurement.measurement_Type == measurement_Types[Specular_Scan] &&
	   (unwrapped_Structure->calc_Functions.check_Transmittance ||
		unwrapped_Structure->calc_Functions.check_Absorptance) )
	{
		calc_Environmental_Factor(thread_Index, point_Index);

		complex<double> t_s = t_Local_s[thread_Index][0];
		complex<double> t_p = t_Local_p[thread_Index][0];

		calculated_Values.Phi_T_s[point_Index] = qRadiansToDegrees(arg(t_s));
		calculated_Values.Phi_T_p[point_Index] = qRadiansToDegrees(arg(t_p));
		calculated_Values.T_s	 [point_Index] = norm(t_s)*environment_Factor_s[thread_Index];
		calculated_Values.T_p	 [point_Index] = norm(t_p)*environment_Factor_p[thread_Index];
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
	if(	measurement.measurement_Type == measurement_Types[Specular_Scan] &&
		unwrapped_Structure->calc_Functions.check_Absorptance )
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
	if(measurement.measurement_Type == measurement_Types[Specular_Scan] &&
	   (unwrapped_Structure->calc_Functions.check_Field ||
		unwrapped_Structure->calc_Functions.check_Joule) )
	{
		// calculated_Values.field_Intensity	already calculated
		// calculated_Values.absorption_Map		already calculated
	}

	// scattering
	if(	spec_Scat_mode == SCATTERED_MODE &&
		unwrapped_Structure->calc_Functions.check_Scattering )
	{
		if(multilayer->imperfections_Model.use_Roughness || multilayer->imperfections_Model.use_Particles)
		{
			// calculated_Values.S_s	[point_Index] already calculated
			// calculated_Values.S_p	[point_Index] already calculated
			calculated_Values.S			[point_Index] = s_Weight * calculated_Values.S_s[point_Index] + p_Weight * calculated_Values.S_p[point_Index];
		} else
		{
			calculated_Values.S			[point_Index] = 0;
		}
		calculated_Values.S_Instrumental[point_Index] = calculated_Values.S[point_Index];
	}

	// GISAS
	if(	spec_Scat_mode == SCATTERED_MODE &&
		unwrapped_Structure->calc_Functions.check_GISAS)
	{
		if(multilayer->imperfections_Model.use_Roughness || multilayer->imperfections_Model.use_Particles)
		{
			// interpolate the other half
			if(short_Phi_Points!=phi_Points)
			{
				for(size_t phi_Index = 0; phi_Index<short_Phi_Points; phi_Index++)
				{
					GISAS_Slice[thread_Index][phi_Index] = calculated_Values.GISAS_Map       [measurement.start_Phi_Index+phi_Index][point_Index];
					phi_Slice  [thread_Index][phi_Index] = measurement.detector_Phi_Angle_Vec[measurement.start_Phi_Index+phi_Index];
				}
				gsl_spline_init(spline_Vec[thread_Index], phi_Slice[thread_Index].data(), GISAS_Slice[thread_Index].data(), GISAS_Slice[thread_Index].size());
				if(measurement.start_Phi_Index!=0)
				{
					for(size_t phi_Index = 1; phi_Index<measurement.start_Phi_Index; phi_Index++)
					{
						calculated_Values.GISAS_Map[phi_Index][point_Index] = gsl_spline_eval(spline_Vec[thread_Index], -measurement.detector_Phi_Angle_Vec[phi_Index], acc_Vec[thread_Index]);
					}
					size_t phi_Index = 0;
					if(abs(measurement.detector_Phi_Angle_Vec.back()+measurement.detector_Phi_Angle_Vec.front())<=1e-9)
					{
						calculated_Values.GISAS_Map[phi_Index][point_Index] = calculated_Values.GISAS_Map[phi_Points-1][point_Index];
					} else
					{
						calculated_Values.GISAS_Map[phi_Index][point_Index] = gsl_spline_eval(spline_Vec[thread_Index], -measurement.detector_Phi_Angle_Vec[phi_Index], acc_Vec[thread_Index]);
					}
				} else
				if(measurement.end_Phi_Number!=phi_Points)
				{
					for(size_t phi_Index = measurement.end_Phi_Number; phi_Index<phi_Points-1; phi_Index++)
					{
						calculated_Values.GISAS_Map[phi_Index][point_Index] = gsl_spline_eval(spline_Vec[thread_Index], -measurement.detector_Phi_Angle_Vec[phi_Index], acc_Vec[thread_Index]);
					}
					size_t phi_Index = phi_Points-1;
					if(abs(measurement.detector_Phi_Angle_Vec.back()+measurement.detector_Phi_Angle_Vec.front())<=1e-9)
					{
						calculated_Values.GISAS_Map[phi_Index][point_Index] = calculated_Values.GISAS_Map[0][point_Index];
					} else
					{
						calculated_Values.GISAS_Map[phi_Index][point_Index] = gsl_spline_eval(spline_Vec[thread_Index], -measurement.detector_Phi_Angle_Vec[phi_Index], acc_Vec[thread_Index]);
					}
				}
			}
		} else
		{
			for(size_t phi_Index = 0; phi_Index<phi_Points; phi_Index++)
			{
				calculated_Values.GISAS_Map[phi_Index][point_Index] = 0;
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
		fill_Specular_Values		  (thread_Index, point_Index);
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
}
