#ifndef FITTING_SETTINGS_H
#define FITTING_SETTINGS_H

#include "global/layer_data_types.h"

class Fitting_Settings: public QWidget
{
	Q_OBJECT
public:
	explicit Fitting_Settings(QWidget *parent = nullptr);

	// state variables
	bool abort = false;
	bool in_Calculation = false;

	// common
	QString current_Method;
	bool randomized_Start;

	/// for GSL TRS
        // main
        int num_Runs_GSL;
        int max_Iter;
	double x_Tolerance;
	double g_Tolerance;
	double f_Tolerance;

	// additional
		QString current_Scale;
		QString current_Solver;
		QString current_Fdtype;
		double factor_Up;
		double factor_Down;
		double avmax;
		double h_df;
		double h_fvv;

	/// for SwarmOps
	// main
        bool initialize_By_Current_State;
        int num_Runs_SO;
        int max_Evaluations;
	int max_Eval_Factor;
	bool max_Eval_Check;

	// additional

		// Hill_Climber
		double r_Factor_HC;
		double D_HC;

		// Simulated_Annealing
		double r_Factor_SA;
		double alpha_SA;
		double beta_SA;
		double T_SA;

		// Local_Unimodal_Sampling
		double gamma_LUS;

		// Differential_Evolution
		double NP_DE;
		double CR_DE;
		double F_DE;

		// Differential_Evolution_Suite (Best/1/Bin/Simple)
		double NP_DES;
		double CR_DES;
		double F_DES;

		// DE_with_Temporal_Parameters
		double NP_DETP;
		double CR_1_DETP;
		double CR_2_DETP;
		double F_1_DETP;
		double F_2_DETP;

		// Jan_Differential_Evolution
		double NP_JDE;
		double F_Init_JDE;
		double F_l_JDE;
		double F_u_JDE;
		double Tau_F_JDE;
		double CR_Init_JDE;
		double CR_l_JDE;
		double CR_u_JDE;
		double Tau_CR_JDE;

		// Evolution_by_Lingering_Global_Best
		double NP_ELG;

		// More_Yo_yos_Doing_Global_optimization
		double NP_MYG;
		double F_MYG;

		// Particle_Swarm_Optimization
		double S_PSO;
		double omega_PSO;
		double phi_p_PSO;
		double phi_g_PSO;

		// Forever_Accumulating_Evolution
		double S_FAE;
		double lambda_g_FAE;
		double lambda_x_FAE;

		// Many_Optimizing_Liaisons
		double S_MOL;
		double omega_MOL;
		double phi_g_MOL;

		// Layered_and_Interleaved_CoEvolution
		double gamma_2_LICE;
		double N_LICE;
		double gamma_LICE;
};

// serialization
QDataStream& operator <<( QDataStream& stream, const Fitting_Settings* fitting_Settings );
QDataStream& operator >>( QDataStream& stream,		 Fitting_Settings* fitting_Settings );

#endif // FITTING_SETTINGS_H
