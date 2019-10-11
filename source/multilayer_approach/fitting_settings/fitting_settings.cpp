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
	max_Evaluations=2000;
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

		// Particle_Swarm_Optimization
		S_PSO = 148.0;
		omega_PSO = -0.046644;
		phi_p_PSO = 2.882152;
		phi_g_PSO = 1.857463;

		// Forever_Accumulating_Evolution
		S_FAE = 100.0;
		lambda_g_FAE = 1.486496;
		lambda_x_FAE = -3.949617;

		// Many_Optimizing_Liaisons
		S_MOL = 153.0;
		omega_MOL = -0.289623;
		phi_g_MOL = 1.494742;

		// Layered_and_Interleaved_CoEvolution
		gamma_2_LICE = 0.991083;
		N_LICE = 25.0;
		gamma_LICE = 5.633202;
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
	<< fitting_Settings->max_Evaluations << fitting_Settings->max_Eval_Factor << fitting_Settings->max_Eval_Check
	// additional
	<< fitting_Settings->r_Factor_HC << fitting_Settings->D_HC
	<< fitting_Settings->r_Factor_SA << fitting_Settings->alpha_SA << fitting_Settings->beta_SA << fitting_Settings->T_SA
	<< fitting_Settings->gamma_LUS
	<< fitting_Settings->NP_DE << fitting_Settings->CR_DE << fitting_Settings->F_DE
	<< fitting_Settings->NP_DES << fitting_Settings->CR_DES << fitting_Settings->F_DES
	<< fitting_Settings->NP_DETP << fitting_Settings->CR_1_DETP << fitting_Settings->CR_2_DETP << fitting_Settings->F_1_DETP << fitting_Settings->F_2_DETP
	<< fitting_Settings->NP_JDE << fitting_Settings->F_Init_JDE << fitting_Settings->F_l_JDE << fitting_Settings->F_u_JDE << fitting_Settings->Tau_F_JDE
								<< fitting_Settings->CR_Init_JDE << fitting_Settings->CR_l_JDE << fitting_Settings->CR_u_JDE << fitting_Settings->Tau_CR_JDE
	<< fitting_Settings->NP_ELG
	<< fitting_Settings->NP_MYG << fitting_Settings->F_MYG
	<< fitting_Settings->S_PSO << fitting_Settings->omega_PSO << fitting_Settings->phi_p_PSO << fitting_Settings->phi_g_PSO
	<< fitting_Settings->S_FAE << fitting_Settings->lambda_g_FAE << fitting_Settings->lambda_x_FAE
	<< fitting_Settings->S_MOL << fitting_Settings->omega_MOL << fitting_Settings->phi_g_MOL
	<< fitting_Settings->gamma_2_LICE << fitting_Settings->N_LICE << fitting_Settings->gamma_LICE;
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
	stream >> fitting_Settings->max_Evaluations >> fitting_Settings->max_Eval_Factor >> fitting_Settings->max_Eval_Check;
	// additional
	if(Global_Variables::check_Loaded_Version(1,9,4))	// since 1.9.4
	{
		stream
		>> fitting_Settings->r_Factor_HC >> fitting_Settings->D_HC
		>> fitting_Settings->r_Factor_SA >> fitting_Settings->alpha_SA >> fitting_Settings->beta_SA >> fitting_Settings->T_SA
		>> fitting_Settings->gamma_LUS
		>> fitting_Settings->NP_DE >> fitting_Settings->CR_DE >> fitting_Settings->F_DE
		>> fitting_Settings->NP_DES >> fitting_Settings->CR_DES >> fitting_Settings->F_DES
		>> fitting_Settings->NP_DETP >> fitting_Settings->CR_1_DETP >> fitting_Settings->CR_2_DETP >> fitting_Settings->F_1_DETP >> fitting_Settings->F_2_DETP
		>> fitting_Settings->NP_JDE >> fitting_Settings->F_Init_JDE >> fitting_Settings->F_l_JDE >> fitting_Settings->F_u_JDE >> fitting_Settings->Tau_F_JDE
									>> fitting_Settings->CR_Init_JDE >> fitting_Settings->CR_l_JDE >> fitting_Settings->CR_u_JDE >> fitting_Settings->Tau_CR_JDE
		>> fitting_Settings->NP_ELG
		>> fitting_Settings->NP_MYG >> fitting_Settings->F_MYG
		>> fitting_Settings->S_PSO >> fitting_Settings->omega_PSO >> fitting_Settings->phi_p_PSO >> fitting_Settings->phi_g_PSO
		>> fitting_Settings->S_FAE >> fitting_Settings->lambda_g_FAE >> fitting_Settings->lambda_x_FAE
		>> fitting_Settings->S_MOL >> fitting_Settings->omega_MOL >> fitting_Settings->phi_g_MOL
		>> fitting_Settings->gamma_2_LICE >> fitting_Settings->N_LICE >> fitting_Settings->gamma_LICE;

	}

	return stream;
}
