#include "fitting_settings.h"
#include "gsl/gsl_machine.h"

Fitting_Settings::Fitting_Settings(QWidget* parent) :
	QWidget(parent)
{
	// common
	current_Method = SO_Methods[Differential_Evolution];
	num_Runs = 1;
	randomized_Start = false;

	/// for GSL TRS
	// main
	max_Iter = 20;
	x_Tolerance = 1.0e-8;
	g_Tolerance = 1.0e-8;
	f_Tolerance = 1.0e-8;

	// additional
	current_Scale = GSL_Scales[More];
	current_Solver = GSL_Solvers[QR_decomposition];
	current_Fdtype = GSL_Fdtype[Forward];
	factor_Up = 3;
	factor_Down = 2;
	avmax = 0.75;
	h_df = GSL_SQRT_DBL_EPSILON;
	h_fvv = 0.02;

	/// for SwarmOps
	// main
	initialize_By_Current_State = true;
	max_Evaluations=1000;
	max_Eval_Factor=250;
	max_Eval_Check=false;

	// additional

		// Hill_Climber
		r_Factor_HC = 0.01;
		D_HC = 10.0;

		// Simulated_Annealing
		r_Factor_SA = 0.01;
		alpha_SA = 0.3;
		beta_SA = 0.01;
		T_SA = 40000;

		// Local_Unimodal_Sampling
		gamma_LUS = 3.0;

		// Differential_Evolution
		NP_DE = 172.0;
		CR_DE = 0.965609;
		F_DE = 0.361520;

		// Differential_Evolution_Suite (Best/1/Bin/Simple)
		NP_DES = 157.0;
		CR_DES = 0.976920;
		F_DES = 0.334942;

		// DE_with_Temporal_Parameters
		NP_DETP = 9.0;
		CR_1_DETP = 0.040135;
		CR_2_DETP = 0.576005;
		F_1_DETP = 0.955493;
		F_2_DETP = 0.320264;

		// Jan_Differential_Evolution
		NP_JDE = 18.0;
		F_Init_JDE = 1.393273;
		F_l_JDE = 0.319121;
		F_u_JDE = 0.933712;
		Tau_F_JDE = 0.619482;
		CR_Init_JDE = 0.777215;
		CR_l_JDE = 0.889368;
		CR_u_JDE = 0.160088;
		Tau_CR_JDE = 0.846782;

		// Evolution_by_Lingering_Global_Best
		NP_ELG = 143.0;

		// More_Yo_yos_Doing_Global_optimization
		NP_MYG = 300;
		F_MYG = 1.627797;

}

// serialization
QDataStream& operator <<( QDataStream& stream, const Fitting_Settings* fitting_Settings )
{
	return stream << fitting_Settings->current_Method << fitting_Settings->num_Runs << fitting_Settings->randomized_Start

	/// GSL TRS
	// main
	<< fitting_Settings->max_Iter << fitting_Settings->x_Tolerance << fitting_Settings->g_Tolerance << fitting_Settings->f_Tolerance
	// additional
	<< fitting_Settings->current_Scale << fitting_Settings->current_Solver << fitting_Settings->current_Fdtype
	<< fitting_Settings->factor_Up << fitting_Settings->factor_Down << fitting_Settings->avmax << fitting_Settings->h_df << fitting_Settings->h_fvv

	/// SwarmOps
	// main
	<< fitting_Settings->initialize_By_Current_State	// since 1.7.1
	<< fitting_Settings->max_Evaluations << fitting_Settings->max_Eval_Factor << fitting_Settings->max_Eval_Check ;
}
QDataStream& operator >>( QDataStream& stream,		  Fitting_Settings* fitting_Settings )
{
	stream >> fitting_Settings->current_Method >> fitting_Settings->num_Runs >> fitting_Settings->randomized_Start;

	/// GSL TRS
	// main
	stream >> fitting_Settings->max_Iter >> fitting_Settings->x_Tolerance >> fitting_Settings->g_Tolerance >> fitting_Settings->f_Tolerance
	// additional
		   >> fitting_Settings->current_Scale >> fitting_Settings->current_Solver >> fitting_Settings->current_Fdtype
		   >> fitting_Settings->factor_Up >> fitting_Settings->factor_Down >> fitting_Settings->avmax >> fitting_Settings->h_df >> fitting_Settings->h_fvv;

	/// SwarmOps
	// main
	if(Global_Variables::check_Loaded_Version(1,7,1))
	{stream >> fitting_Settings->initialize_By_Current_State; }	// since 1.7.1
	stream >> fitting_Settings->max_Evaluations >> fitting_Settings->max_Eval_Factor >> fitting_Settings->max_Eval_Check ;

	return stream;
}
