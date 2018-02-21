#include "fitting_swarmops.h"

Fitting_SwarmOps::Fitting_SwarmOps(Fitting* fitting):
	multilayer_Approach(fitting->multilayer_Approach),
	main_Calculation_Module(fitting->main_Calculation_Module),
	calculation_Trees(fitting->main_Calculation_Module->calculation_Trees),
	fitables(fitting->main_Calculation_Module->fitables),
	fitting(fitting),
	params(&fitting->params)
{
	qInfo() << "Fitting_SwarmOps : f.size =" << params->f->size;
	qInfo() << "Fitting_SwarmOps : x.size =" << params->x->size;
}

void Fitting_SwarmOps::callback(Fitting_Params* params, SO_TFitness residual)
{
	// print out current location
	printf("iter %zu :", params->counter++);
	for(size_t i=0; i<params->fitables.fit_Parameters.size(); ++i)
	{
		printf("%f ", params->fitables.fit_Parameters[i]->value);
	}
	printf("\t|f|=%g", residual);
	printf("\n\n");
}


SO_TFitness Fitting_SwarmOps::calc_Residual(const SO_TElm* x,  void* context, const SO_TFitness fitnessLimit)
{
	Fitting_Params* params = ((struct Fitting_Params*)context);

//	// initial position at start (before or after first calculation?)
//	if(params->first_Init)
//	{
//		if(params->counter == 0)
//		{
//			for(size_t i=0; i<params->p; ++i)
//			{
//				x[i] = gsl_vector_get(params->x, i);
//			}
//		}
//	}

	// fill x
	for(size_t i=0; i<params->p; ++i)
	{
		gsl_vector_set(params->x, i, x[i]);
	}

	// calc f
	Fitting::calc_Residual(params->x, params, params->f);

	// calc residual
	SO_TFitness residual=-2018;
	gsl_blas_ddot(params->f, params->f, &residual);

	// print state	
	callback(params, residual);

	return residual;
}

bool Fitting_SwarmOps::fit()
{
	RO_RandSeedClock(9385839);
	params->counter = 0;

	/// reading parameters

	// read main parameters
	size_t kMethodId = SO_kMethodDE;
	const size_t kNumRuns = 1;
	const SO_TDim kDim = params->p;
	const size_t kDimFactor = 250;
	const size_t kNumIterations = 2; // kDimFactor*kDim;
//	const char* kTraceFilename = "FitnessTrace.txt";
	{
		// read method
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Mesh_Iteration])						{	kMethodId = SO_kMethodMESH;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Random_Sampling_Uniform])			{	kMethodId = SO_kMethodRND;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Gradient_Descent])					{	kMethodId = SO_kMethodGD;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Gradient_Emancipated_Descent])		{	kMethodId = SO_kMethodGED;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Hill_Climber])						{	kMethodId = SO_kMethodHC;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Simulated_Annealing])				{	kMethodId = SO_kMethodSA;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Pattern_Search])						{	kMethodId = SO_kMethodPS;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Local_Unimodal_Sampling])			{	kMethodId = SO_kMethodLUS;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Differential_Evolution])				{	kMethodId = SO_kMethodDE;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Differential_Evolution_Suite])		{	kMethodId = SO_kMethodDESuite;	}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[DE_with_Temporal_Parameters])		{	kMethodId = SO_kMethodDETP;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Jan_Differential_Evolution])			{	kMethodId = SO_kMethodJDE;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Evolution_by_Lingering_Global_Best])	{	kMethodId = SO_kMethodELG;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[More_Yo_yos_Doing_Global_optimization]){	kMethodId = SO_kMethodMYG;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Particle_Swarm_Optimization])		{	kMethodId = SO_kMethodPSO;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Forever_Accumulating_Evolution])		{	kMethodId = SO_kMethodFAE;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Many_Optimizing_Liaisons])			{	kMethodId = SO_kMethodMOL;		}
		if(multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Layered_and_Interleaved_CoEvolution]){	kMethodId = SO_kMethodLICE;		}
	}

	SO_TElm lowerBound = 0;
	SO_TElm upperBound = 1;

	// calc init residual
	Fitting::calc_Residual(params->x, params, params->f);
	double residual=-2018;
	gsl_blas_ddot(params->f, params->f, &residual);

	auto start = std::chrono::system_clock::now();
	struct SO_Results res = SO_OptimizePar(
			  SO_kMethodDefaultParameters[kMethodId],			/* Behavioural parameters for optimization method. */
			  kMethodId,										/* Optimization method. */
			  kNumRuns,											/* Number of optimization runs. */
			  kNumIterations,									/* Number of iterations per run. */
			  NULL,												/* Additional optimization settings. */
			  calc_Residual,									/* Optimization problem (aka. fitness function). */
			  NULL,												/* Gradient for optimization problem. */
			  params,											/* Context for optimization problem. */
			  kDim,												/* Dimensionality for optimization problem. */
			  &lowerBound,										/* Lower initialization boundary. */
			  &upperBound,										/* Upper initialization bounder. */
			  &lowerBound,										/* Lower search-space boundary. */
			  &upperBound,										/* Upper search-space boundary. */
			  NULL/*kTraceFilename*/);							/* Fitness trace filename (null-pointer for no trace). */

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	qInfo() << "\nFit  : "<< elapsed.count()/1000. << " seconds\n";
	qInfo() << "previous_ID =" << previous_ID << endl;

	printf("Fitness average:  %g\n", res.stat.fitnessAvg);
	printf("Fitness std.dev.: %g\n", res.stat.fitnessStdDev);
	printf("Best fitness:	  %g\n", res.best.fitness);
	printf("\n");
	printf("Initial     |f| = %g\n", residual);
	printf("Final       |f| = %g\n", res.best.fitness);
	printf("\n");

	// compare with previous solution
	double* final_State_Parametrized;
	if(residual<res.best.fitness)
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
		res.best.x[i] = params->main_Calculation_Module->unparametrize(	final_State_Parametrized[i],
																		params->fitables.fit_Parameters[i]->fit.min,
																		params->fitables.fit_Parameters[i]->fit.max);
		params->fitables.fit_Parameters[i]->value = res.best.x[i];
	}

	printf("Best solution:   ");
	SO_PrintVector(res.best.x, res.best.dim);
	printf("\n");

	SO_FreeResults(&res);
	return true;
}

