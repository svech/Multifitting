#include "fitting_swarmops.h"

jmp_buf buffer_SO;

Fitting_SwarmOps::Fitting_SwarmOps(Fitting* fitting):
	main_Calculation_Module(fitting->main_Calculation_Module),
	calculation_Trees(fitting->main_Calculation_Module->calculation_Trees),
	fitting(fitting),
	params(&fitting->params)
{

}

void Fitting_SwarmOps::callback(Fitting_Params* params, SO_TFitness residual)
{
	// print out current location
	if(params->counter%10 == 0 || params->counter == params->num_Iter-1)
	{
		printf("iter %zu :", params->counter);
		for(size_t i=0; i<params->fitables.param_Pointers.size(); ++i)
		{
			printf("%f ", params->fitables.param_Pointers[i]->value);
		}

		if(params->maximize)
		{
			printf("\t|f|=%g", params->max_Integral-residual);
		} else
		{
			printf("\t|f|=%g", residual);
		}

		printf("\n\n");
	}
}

SO_TFitness Fitting_SwarmOps::calc_Residual(const SO_TElm* x,  void* context, SO_TFitness fitnessLimit)
{
	if(global_Multilayer_Approach->fitting_Settings->abort)
	{
		longjmp(buffer_SO, 2018); // not zero! zero means repeating in infinite loop!
	}
	Q_UNUSED(fitnessLimit);
	Fitting_Params* params = ((struct Fitting_Params*)context);

    SO_TElm* var_x = const_cast<SO_TElm*>(x);

	// first point is the initial
    if(params->counter == 0 && global_Multilayer_Approach->fitting_Settings->initialize_By_Current_State) {
        for(size_t i=0; i<params->p; ++i)
            var_x[i] = params->fitables.values_Parametrized[i];
    }

	// fill x
	for(size_t i=0; i<params->p; ++i)
	{
        gsl_vector_set(params->x, i, var_x[i]);
	}

	// calc residual
	Fitting::calc_Residual(params->x, params, params->f);
	gsl_blas_ddot(params->f, params->f, &params->final_Residual);
	// duplicate SO functionality for aborting
	if(params->final_Residual < params->my_Res.best.fitness )
	{
		params->my_Res.best.fitness = params->final_Residual;
		for(size_t i=0; i<params->p; ++i)
		{
            params->my_Res.best.x[i] = var_x[i];
		}
	}

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
    params->num_Iter = kNumIterations;
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


    // init my_Res for aborting
    params->my_Res.best.fitness = DBL_MAX;
    params->my_Res.best.dim = params->p;
    params->my_Res.best.x = new SO_TElm[params->my_Res.best.dim];

    SO_Results res;

    auto start = std::chrono::system_clock::now();
    int repeat = setjmp(buffer_SO);
    if(repeat == 0)
    {
        res = SO_OptimizePar(
              SO_kMethodDefaultParameters[kMethodId],			/* Behavioural parameters for optimization method. */
              kMethodId,										/* Optimization method. */
              1,												/* Number of optimization runs. */
              kNumIterations,									/* Number of iterations per run. */
              nullptr,											/* Additional optimization settings. */
              calc_Residual,									/* Optimization problem (aka. fitness function). */
              nullptr,											/* Gradient for optimization problem. */
              params,											/* Context for optimization problem. */
              kDim,												/* Dimensionality for optimization problem. */
              lowerBound.data(),								/* Lower initialization boundary. */
              upperBound.data(),								/* Upper initialization bounder. */
              lowerBound.data(),								/* Lower search-space boundary. */
              upperBound.data(),								/* Upper search-space boundary. */
              nullptr);											/* Fitness trace filename (null-pointer for no trace). */
    } else
    // if aborted
    {
        res = params->my_Res;
    }

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
        if(params->maximize)
        {
            printf("Initial     |f| = %g\n", params->max_Integral-params->init_Residual);
        } else
        {
            printf("Initial     |f| = %g\n", params->init_Residual);
        }

        // compare with previous solution
        double* final_State_Parametrized;
        if(params->init_Residual<res.best.fitness)
        {
            printf("-Keep previous solution-\n\n");
            final_State_Parametrized = params->fitables.values_Parametrized.data();
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
                                                                            params->fitables.param_Pointers[i]->fit.min,
                                                                            params->fitables.param_Pointers[i]->fit.max);
            params->fitables.param_Pointers[i]->value = res.best.x[i];
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
                                                                            params->fitables.param_Pointers[i]->fit.min,
                                                                            params->fitables.param_Pointers[i]->fit.max);
        }
    }
    // replace parameters
    for(size_t i=0; i<params->p; ++i)
    {
        double old_Value = params->fitables.param_Pointers[i]->value;
        params->fitables.param_Pointers[i]->value = res.best.x[i];
        double new_Value = params->fitables.param_Pointers[i]->value;

        Fitting::change_Real_Fitables_and_Dependent(params, old_Value, new_Value, i, FITTING);
    }

    if(params->maximize)
    {
        printf("Final       |f| = %g\n", params->max_Integral-res.best.fitness);
    } else
    {
        printf("Final       |f| = %g\n", res.best.fitness);
    }
    printf("Best solution:   ");
    SO_PrintVector(res.best.x, res.best.dim);
    printf("\n\n");

    if(repeat == 0) SO_FreeResults(&res);

	return true;
}

