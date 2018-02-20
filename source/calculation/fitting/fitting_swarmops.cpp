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

void Fitting_SwarmOps::callback(Fitting_Params* params)
{
	// print out current location
	printf("iter %zu :", params->counter++);
	for(size_t i=0; i<params->fitables.fit_Parameters.size(); ++i)
	{
		printf("%f ", params->fitables.fit_Parameters[i]->value);
	}
	printf("\n\n");
}


SO_TFitness Fitting_SwarmOps::calc_Residual(SO_TElm* x,  void const* context, const SO_TFitness fitnessLimit)
{
	Fitting_Params* params = ((struct Fitting_Params*)context);

//	// initial position at start (before or after first calculation?)
//	if(params->first_Init)
//	{
//		if(params->counter == 0)
//		for(size_t i=0; i<fitables.fit_Parameters.size(); ++i)
//		{
//			x[i] = fitables.fit_Value_Parametrized[i];
//		}
//	}

	// fill x
	for(size_t i=0; i<params->n; ++i)
	{
		gsl_vector_set(params->x, i, x[i]);
	}

	// calc f
	Fitting::calc_Residual(params->x, params, params->f);

	// calc residual
	SO_TFitness residual=-2018;
	gsl_blas_ddot(params->f, params->f, &residual);

	// print state
	callback(params);

	return residual;
}

bool Fitting_SwarmOps::fit()
{
	RO_RandSeedClock(9385839);
	params->counter = 0;

	// optimization settings
	const size_t kMethodId = SO_kMethodDE;
	const size_t kNumRuns = 1;
	const SO_TDim kDim = params->p;
	const size_t kDimFactor = 250;
	const size_t kNumIterations = 2000; // kDimFactor*kDim;
	const char* kTraceFilename = "FitnessTrace.txt";

	SO_TElm lowerBound = 0;
	SO_TElm upperBound = 1;

	// calc init residual
	Fitting::calc_Residual(params->x, params, params->f);	
	double residual=-2018;
	gsl_blas_ddot(params->f, params->f, &residual);

	struct SO_Results res = SO_OptimizePar(
			  SO_kMethodDefaultParameters[kMethodId],			/* Behavioural parameters for optimization method. */
			  kMethodId,										/* Optimization method. */
			  kNumRuns,											/* Number of optimization runs. */
			  kNumIterations,									/* Number of iterations per run. */
			  0,												/* Additional optimization settings. */
			  calc_Residual,									/* Optimization problem (aka. fitness function). */
			  0,												/* Gradient for optimization problem. */
			  params,											/* Context for optimization problem. */
			  kDim,												/* Dimensionality for optimization problem. */
			  fitables.fit_Value_Parametrized.data(),			/* Lower initialization boundary. */
			  fitables.fit_Value_Parametrized.data(),			/* Upper initialization bounder. */
			  &lowerBound,										/* Lower search-space boundary. */
			  &upperBound,										/* Upper search-space boundary. */
			  kTraceFilename);									/* Fitness trace filename (null-pointer for no trace). */

//	printf("Fitness average:  %g\n", res.stat.fitnessAvg);
//	printf("Fitness std.dev.: %g\n", res.stat.fitnessStdDev);
//	printf("Best fitness:	  %g\n", res.best.fitness);
//	printf("Best solution: ");
//		SO_PrintVector(res.best.x, res.best.dim);

//	printf("\nParameters used: ");
//	printf("\n");
//	printf("Initial \t|f| = %f\n",	   residual);
//	printf("Final   \t|f| = %f\n\n\n", res.best.fitness);

//	SO_FreeResults(&res);
	return true;
}

