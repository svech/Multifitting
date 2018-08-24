// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "fitting_swarmops.h"

Fitting_SwarmOps::Fitting_SwarmOps(Fitting* fitting):
	main_Calculation_Module(fitting->main_Calculation_Module),
	calculation_Trees(fitting->main_Calculation_Module->calculation_Trees),
	fitables(fitting->main_Calculation_Module->fitables),
	fitting(fitting),
	params(&fitting->params)
{

}

void Fitting_SwarmOps::callback(Fitting_Params* params, SO_TFitness residual)
{
	// print out current location
	if(params->counter%10 == 0)
	{
		printf("iter %zu :", params->counter);
		for(size_t i=0; i<params->fitables.fit_Parameters.size(); ++i)
		{
			printf("%f ", params->fitables.fit_Parameters[i]->value);
		}
		printf("\t|f|=%g", residual);

		printf("\n\n");
	}
}

SO_TFitness Fitting_SwarmOps::calc_Residual(SO_TElm* x,  void* context, SO_TFitness fitnessLimit)
{
	fitnessLimit;
	Fitting_Params* params = ((struct Fitting_Params*)context);

	// first point is the initial
	if(global_Multilayer_Approach->fitting_Settings->initialize_By_Current_State && params->counter == 0)
	for(size_t i=0; i<params->p; ++i)
	{
		x[i]  = params->fitables.fit_Value_Parametrized[i];
	}

	// fill x
	for(size_t i=0; i<params->p; ++i)
	{
		gsl_vector_set(params->x, i, x[i]);
	}

	// calc f
	Fitting::calc_Residual(params->x, params, params->f);

	// calc residual
	gsl_blas_ddot(params->f, params->f, &params->final_Residual);

	// print state	
	callback(params, params->final_Residual);
	params->counter++;

	return params->final_Residual;
}

bool Fitting_SwarmOps::fit()
{
	RO_RandSeedClock(9385839);
	params->counter = 0;

	/// reading parameters

	// read main parameters
	size_t kMethodId = SO_kMethodDE;	// default value
	const SO_TDim kDim = params->p;
	size_t kNumIterations = global_Multilayer_Approach->fitting_Settings->max_Evaluations;
	if(global_Multilayer_Approach->fitting_Settings->max_Eval_Check)
	{
		kNumIterations = global_Multilayer_Approach->fitting_Settings->max_Eval_Factor*kDim;
	}
	{
		// read method
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Mesh_Iteration])						{	kMethodId = SO_kMethodMESH;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Random_Sampling_Uniform])				{	kMethodId = SO_kMethodRND;		}
//		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Gradient_Descent])					{	kMethodId = SO_kMethodGD;		}
//		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Gradient_Emancipated_Descent])		{	kMethodId = SO_kMethodGED;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Hill_Climber])						{	kMethodId = SO_kMethodHC;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Simulated_Annealing])					{	kMethodId = SO_kMethodSA;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Pattern_Search])						{	kMethodId = SO_kMethodPS;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Local_Unimodal_Sampling])				{	kMethodId = SO_kMethodLUS;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Differential_Evolution])				{	kMethodId = SO_kMethodDE;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Differential_Evolution_Suite])		{	kMethodId = SO_kMethodDESuite;	}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[DE_with_Temporal_Parameters])			{	kMethodId = SO_kMethodDETP;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Jan_Differential_Evolution])			{	kMethodId = SO_kMethodJDE;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Evolution_by_Lingering_Global_Best])	{	kMethodId = SO_kMethodELG;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[More_Yo_yos_Doing_Global_optimization]){	kMethodId = SO_kMethodMYG;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Particle_Swarm_Optimization])			{	kMethodId = SO_kMethodPSO;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Forever_Accumulating_Evolution])		{	kMethodId = SO_kMethodFAE;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Many_Optimizing_Liaisons])			{	kMethodId = SO_kMethodMOL;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Layered_and_Interleaved_CoEvolution])	{	kMethodId = SO_kMethodLICE;		}
	}

	vector<SO_TElm> lowerBound(kDim, 0);
	vector<SO_TElm> upperBound(kDim, 1);

	// calc init residual
	Fitting::calc_Residual(params->x, params, params->f);
	gsl_blas_ddot(params->f, params->f, &params->init_Residual);

	auto start = std::chrono::system_clock::now();
	struct SO_Results res = SO_OptimizePar(
			  SO_kMethodDefaultParameters[kMethodId],			/* Behavioural parameters for optimization method. */
			  kMethodId,										/* Optimization method. */
			  1,												/* Number of optimization runs. */
			  kNumIterations,									/* Number of iterations per run. */
			  nullptr,												/* Additional optimization settings. */
			  calc_Residual,									/* Optimization problem (aka. fitness function). */
			  nullptr,												/* Gradient for optimization problem. */
			  params,											/* Context for optimization problem. */
			  kDim,												/* Dimensionality for optimization problem. */
			  lowerBound.data(),								/* Lower initialization boundary. */
			  upperBound.data(),								/* Upper initialization bounder. */
			  lowerBound.data(),								/* Lower search-space boundary. */
			  upperBound.data(),								/* Upper search-space boundary. */
			  nullptr);											/* Fitness trace filename (null-pointer for no trace). */

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	qInfo() << "\nFit  : "<< elapsed.count()/1000. << " seconds\n";
	qInfo() << "previous_ID =" << previous_ID << endl;

//	printf("Fitness average:  %g\n", res.stat.fitnessAvg);
//	printf("Fitness std.dev.: %g\n", res.stat.fitnessStdDev);
//	printf("Best fitness:	  %g\n", res.best.fitness);
//	printf("\n");

	params->final_Residual = res.best.fitness;
	if(!global_Multilayer_Approach->fitting_Settings->randomized_Start)
	{
		printf("Initial     |f| = %g\n", params->init_Residual);

		// compare with previous solution
		double* final_State_Parametrized;
		if(params->init_Residual<res.best.fitness)
		{
			printf("-Keep previous solution-\n\n");
			final_State_Parametrized = params->fitables.fit_Value_Parametrized.data();
		}  else
		{
			final_State_Parametrized = res.best.x;
		}

		// replace parameters
		for(size_t i=0; i<params->p; ++i)
		{
			// actualize params->x
			gsl_vector_set(params->x, i, final_State_Parametrized[i]);

			// unparametrize
			res.best.x[i] = params->main_Calculation_Module->unparametrize(	final_State_Parametrized[i],
																			params->fitables.fit_Parameters[i]->fit.min,
																			params->fitables.fit_Parameters[i]->fit.max);
//			params->fitables.fit_Parameters[i]->value = res.best.x[i];
		}
	} else
	// if randomized start
	{
		// replace parameters
		for(size_t i=0; i<params->p; ++i)
		{
			// actualize params->x
			gsl_vector_set(params->x, i, res.best.x[i]);

			// unparametrize
			res.best.x[i] = params->main_Calculation_Module->unparametrize(	res.best.x[i],
																			params->fitables.fit_Parameters[i]->fit.min,
																			params->fitables.fit_Parameters[i]->fit.max);
		}
	}
	// replace parameters
	for(size_t i=0; i<params->p; ++i)
	{
		double old_Value = params->fitables.fit_Parameters[i]->value;
		params->fitables.fit_Parameters[i]->value = res.best.x[i];
		double new_Value = params->fitables.fit_Parameters[i]->value;

		Fitting::change_Real_Fitables_and_Dependent(params, old_Value, new_Value, i);
	}


	printf("Final       |f| = %g\n", res.best.fitness);
	printf("Best solution:   ");
	SO_PrintVector(res.best.x, res.best.dim);
	printf("\n\n");

	SO_FreeResults(&res);
	return true;
}
