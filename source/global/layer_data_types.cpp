#include "layer_data_types.h"
#include "gsl/gsl_integration.h"

Data::Data(QString item_Type_Passed)
{
	id = Global_Definitions::generate_Id();

	if(item_Type_Passed != "empty")
	{
		if( (item_Type_Passed != item_Type_Measurement) &&
			(item_Type_Passed != item_Type_Ambient) &&
			(item_Type_Passed != item_Type_Layer) &&
			(item_Type_Passed != item_Type_Substrate) &&
			(item_Type_Passed != item_Type_Multilayer) &&
			(item_Type_Passed != item_Type_General_Aperiodic) &&
			(item_Type_Passed != item_Type_Regular_Aperiodic) )
		{
			qInfo() << "\n\nData::Data : wrong item_Type\n\n" << endl;
		} else
		{
			item_Type = item_Type_Passed;
			parent_Item_Type = NOPARENT;
		}
//		qInfo() << "Created:" << item_Type << id << endl;
	} else
	{
//		qInfo() << "Data::Data : empty call" << id << endl;
	}

	// Measurement
	{	
		measurement_Type = no_Measurement_Type;
		argument_Type = no_Argument_Type;

		// wavelength
		{
			wavelength.independent.is_Independent = true;
			wavelength.independent.min = default_min_wavelength;
			wavelength.independent.max = default_max_wavelength;
			wavelength.independent.num_Points = default_num_spectral_points;
			wavelength.value = default_wavelength_value;
			wavelength.indicator.whats_This = whats_This_Wavelength;
			wavelength.indicator.item_Id = id;
		}

		// theta_0 angle
		{
			beam_Theta_0_Angle.value = default_theta_angle_value;
			beam_Theta_0_Angle.independent.is_Independent = true;
			beam_Theta_0_Angle.independent.min = default_min_theta_angle;
			beam_Theta_0_Angle.independent.max = default_max_theta_angle;
			beam_Theta_0_Angle.independent.num_Points = default_num_theta_angular_points;
			beam_Theta_0_Angle.indicator.whats_This = whats_This_Beam_Theta_0_Angle;
			beam_Theta_0_Angle.indicator.item_Id = id;

			beam_Theta_0_Specular_Position = 1;
		}

		// theta angle
		{
			detector_Theta_Angle.value = default_alpha_angle_value;
			detector_Theta_Angle.independent.is_Independent = true;
			detector_Theta_Angle.independent.min = default_min_alpha_angle;
			detector_Theta_Angle.independent.max = default_max_alpha_angle;
			detector_Theta_Angle.independent.num_Points = default_num_alpha_angular_points;
			detector_Theta_Angle.indicator.whats_This = whats_This_Detector_Theta_Angle;
			detector_Theta_Angle.indicator.item_Id = id;

			detector_Theta_Offset = 0.5;
		}

		// phi angle
		{
			detector_Phi_Angle.value = default_phi_angle_value;
			detector_Phi_Angle.independent.is_Independent = true;
			detector_Phi_Angle.independent.min = default_min_phi_angle;
			detector_Phi_Angle.independent.max = default_max_phi_angle;
			detector_Phi_Angle.independent.num_Points = default_num_phi_angular_points;
			detector_Phi_Angle.indicator.whats_This = whats_This_Detector_Phi_Angle;
			detector_Phi_Angle.indicator.item_Id = id;
		}

		// resolution
		spectral_Distribution.FWHM_distribution = 0;
		spectral_Distribution.distribution_Function = distributions[Lorentz];
		spectral_Distribution.number_of_Samples = 5;
		spectral_Distribution.coverage = 2;

		beam_Theta_0_Distribution.FWHM_distribution = 0;
		beam_Theta_0_Distribution.distribution_Function = distributions[Lorentz];
		beam_Theta_0_Distribution.number_of_Samples = 5;
		beam_Theta_0_Distribution.coverage = 2;

		beam_Phi_0_Distribution.FWHM_distribution = 0;
		beam_Phi_0_Distribution.distribution_Function = distributions[Lorentz];
		beam_Phi_0_Distribution.number_of_Samples = 5;
		beam_Phi_0_Distribution.coverage = 2;

		// detector
		detector_1D.detector_Type = detectors[Slit];
		detector_1D.slit_Width = 0.8;
		detector_1D.distance_To_Sample = 320;
		detector_1D.detector_Theta_Resolution.FWHM_distribution = 0;
		detector_1D.detector_Theta_Resolution.distribution_Function = distributions[Lorentz];
		detector_1D.detector_Theta_Resolution.number_of_Samples = 5;
		detector_1D.detector_Theta_Resolution.coverage = 2;

		detector_2D.detector_Type = detectors[Spherical];
		detector_2D.detector_Theta_Resolution.FWHM_distribution = 0;
		detector_2D.detector_Theta_Resolution.distribution_Function = distributions[Lorentz];
		detector_2D.detector_Theta_Resolution.number_of_Samples = 5;
		detector_2D.detector_Theta_Resolution.coverage = 2;
		detector_2D.detector_Phi_Resolution.FWHM_distribution = 0;
		detector_2D.detector_Phi_Resolution.distribution_Function = distributions[Lorentz];
		detector_2D.detector_Phi_Resolution.number_of_Samples = 5;
		detector_2D.detector_Phi_Resolution.coverage = 2;

		// footprint
		beam_Geometry.size = 0.055;
		beam_Geometry.smoothing = 0.5;

		sample_Geometry.size = 20;
		sample_Geometry.x_Position = 0;
		sample_Geometry.z_Position = 0;
		sample_Geometry.curvature = 0;

		// other
		polarization = 1;
		background = 0;
	}

	// Ambient, Layer, Substrate
	{
		// material
		{
			if(item_Type == item_Type_Ambient)		material = ambient_default_material;
			if(item_Type == item_Type_Layer)		material = layer_default_material;
			if(item_Type == item_Type_Substrate)	material = substrate_default_material;

			approved_Material = material;
		}
		// absolute density
		{
			if(item_Type == item_Type_Ambient)		absolute_Density.value =   ambient_default_absolute_density;
			if(item_Type == item_Type_Layer)		absolute_Density.value =     layer_default_absolute_density;
			if(item_Type == item_Type_Substrate)	absolute_Density.value = substrate_default_absolute_density;
			absolute_Density.fit.is_Fitable = false;
			absolute_Density.fit.min = absolute_Density.value*(1-dispersion);
			absolute_Density.fit.max = absolute_Density.value*(1/*+dispersion*/);
			absolute_Density.indicator.whats_This = whats_This_Absolute_Density;
			absolute_Density.indicator.item_Id = id;

			absolute_Density.confidence.calc_Conf_Interval = false;
			absolute_Density.confidence.min = absolute_Density.fit.min;
			absolute_Density.confidence.max = absolute_Density.fit.max;
			absolute_Density.confidence.num_Points = default_num_confidence_points;
		}
		// relative density
		{
			if(item_Type == item_Type_Ambient)		relative_Density.value =   ambient_default_relative_density;
			if(item_Type == item_Type_Layer)		relative_Density.value =     layer_default_relative_density;
			if(item_Type == item_Type_Substrate)	relative_Density.value = substrate_default_relative_density;
			relative_Density.fit.is_Fitable = false;
			relative_Density.fit.min = relative_Density.value*(1-dispersion);
			relative_Density.fit.max = relative_Density.value*(1/*+dispersion*/);
			relative_Density.indicator.whats_This = whats_This_Relative_Density;
			relative_Density.indicator.item_Id = id;

			relative_Density.confidence.calc_Conf_Interval = false;
			relative_Density.confidence.min = relative_Density.fit.min;
			relative_Density.confidence.max = relative_Density.fit.max;
			relative_Density.confidence.num_Points = default_num_confidence_points;
		}
		// permittivity
		{
			if(item_Type == item_Type_Ambient)		permittivity.value =   ambient_default_permittivity;
			if(item_Type == item_Type_Layer)		permittivity.value =     layer_default_permittivity;
			if(item_Type == item_Type_Substrate)	permittivity.value = substrate_default_permittivity;
			permittivity.fit.is_Fitable = false;
			permittivity.fit.min = permittivity.value*(1-dispersion);
			permittivity.fit.max = permittivity.value*(1+dispersion);
			permittivity.indicator.whats_This = whats_This_Permittivity;
			permittivity.indicator.item_Id = id;

			permittivity.confidence.calc_Conf_Interval = false;
			permittivity.confidence.min = permittivity.fit.min;
			permittivity.confidence.max = permittivity.fit.max;
			permittivity.confidence.num_Points = default_num_confidence_points;
		}
		// absorption
		{
			if(item_Type == item_Type_Ambient)		absorption.value =   ambient_default_absorption;
			if(item_Type == item_Type_Layer)		absorption.value =     layer_default_absorption;
			if(item_Type == item_Type_Substrate)	absorption.value = substrate_default_absorption;
			absorption.fit.is_Fitable = false;
			absorption.fit.min = absorption.value*(1-dispersion);
			absorption.fit.max = absorption.value*(1+dispersion);
			absorption.indicator.whats_This = whats_This_Absorption;
			absorption.indicator.item_Id = id;

			absorption.confidence.calc_Conf_Interval = false;
			absorption.confidence.min = absorption.fit.min;
			absorption.confidence.max = absorption.fit.max;
			absorption.confidence.num_Points = default_num_confidence_points;
		}
		// composed material
		{
			if(item_Type == item_Type_Ambient)		composed_Material =   ambient_default_composed;
			if(item_Type == item_Type_Layer)		composed_Material =     layer_default_composed;
			if(item_Type == item_Type_Substrate)	composed_Material = substrate_default_composed;
		}
	}

	// Layer, Substrate
	{
		// sigma diffuse
		{
			if(item_Type == item_Type_Layer)		sigma_Diffuse.value = layer_default_sigma_diffuse;
			if(item_Type == item_Type_Substrate)	sigma_Diffuse.value = substrate_default_sigma_diffuse;
			sigma_Diffuse.fit.is_Fitable = false;
			sigma_Diffuse.fit.min = 1;//sigma_Dispersion_Min*sigma.value;
			sigma_Diffuse.fit.max = max(sigma_Dispersion_Max*sigma_Diffuse.value, 20.);
			sigma_Diffuse.indicator.whats_This = whats_This_Sigma_Diffuse;
			sigma_Diffuse.indicator.item_Id = id;

			sigma_Diffuse.confidence.calc_Conf_Interval = false;
			sigma_Diffuse.confidence.min = sigma_Diffuse.fit.min;
			sigma_Diffuse.confidence.max = sigma_Diffuse.fit.max;
			sigma_Diffuse.confidence.num_Points = default_num_confidence_points;
		}
		// interlayer composition
		{
			interlayer_Composition.clear();
			interlayer_Composition.resize(transition_Layer_Functions_Size);
			for(int i=0; i<interlayer_Composition.size(); ++i)
			{
				if(i==0)	{interlayer_Composition[i].enabled=true; }
				else 		{interlayer_Composition[i].enabled=false;}
				{
					interlayer_Composition[i].interlayer.value=1;
					interlayer_Composition[i].interlayer.fit.is_Fitable = false;
					interlayer_Composition[i].interlayer.fit.min = 0;
					interlayer_Composition[i].interlayer.fit.max = 1;
					interlayer_Composition[i].interlayer.indicator.item_Id = id;

					interlayer_Composition[i].interlayer.confidence.calc_Conf_Interval = false;
					interlayer_Composition[i].interlayer.confidence.min = interlayer_Composition[i].interlayer.fit.min;
					interlayer_Composition[i].interlayer.confidence.max = interlayer_Composition[i].interlayer.fit.max;
					interlayer_Composition[i].interlayer.confidence.num_Points = default_num_confidence_points;
				}
				{
					interlayer_Composition[i].my_Sigma_Diffuse.value=0;
					interlayer_Composition[i].my_Sigma_Diffuse.fit.is_Fitable = false;
					interlayer_Composition[i].my_Sigma_Diffuse.fit.min = sigma_Dispersion_Min*interlayer_Composition[i].my_Sigma_Diffuse.value;
					interlayer_Composition[i].my_Sigma_Diffuse.fit.max = sigma_Dispersion_Max*interlayer_Composition[i].my_Sigma_Diffuse.value;
					interlayer_Composition[i].my_Sigma_Diffuse.indicator.item_Id = id;

					interlayer_Composition[i].my_Sigma_Diffuse.confidence.calc_Conf_Interval = false;
					interlayer_Composition[i].my_Sigma_Diffuse.confidence.min = interlayer_Composition[i].my_Sigma_Diffuse.fit.min;
					interlayer_Composition[i].my_Sigma_Diffuse.confidence.max = interlayer_Composition[i].my_Sigma_Diffuse.fit.max;
					interlayer_Composition[i].my_Sigma_Diffuse.confidence.num_Points = default_num_confidence_points;
				}
			}
		}
		// roughness
		{
			roughness_Model.is_Enabled = false;
			roughness_Model.model = ABC_model;
		}
		// sigma roughness
		{
			if(item_Type == item_Type_Layer)		roughness_Model.sigma.value = layer_default_sigma_roughness;
			if(item_Type == item_Type_Substrate)	roughness_Model.sigma.value = substrate_default_sigma_roughness;
			roughness_Model.sigma.fit.is_Fitable = false;
			roughness_Model.sigma.fit.min = 1;
			roughness_Model.sigma.fit.max = max(sigma_Dispersion_Max*roughness_Model.sigma.value, 20.);
			roughness_Model.sigma.indicator.whats_This = whats_This_Sigma_Roughness;
			roughness_Model.sigma.indicator.item_Id = id;

			roughness_Model.sigma.confidence.calc_Conf_Interval = false;
			roughness_Model.sigma.confidence.min = roughness_Model.sigma.fit.min;
			roughness_Model.sigma.confidence.max = roughness_Model.sigma.fit.max;
			roughness_Model.sigma.confidence.num_Points = default_num_confidence_points;
		}
		// correlation radius
		{
			roughness_Model.cor_radius.value = 10000;
			roughness_Model.cor_radius.fit.is_Fitable = false;
			roughness_Model.cor_radius.fit.min = 100;
			roughness_Model.cor_radius.fit.max = 100000;
			roughness_Model.cor_radius.indicator.whats_This = whats_This_Correlation_Radius;
			roughness_Model.cor_radius.indicator.item_Id = id;

			roughness_Model.cor_radius.confidence.calc_Conf_Interval = false;
			roughness_Model.cor_radius.confidence.min = roughness_Model.cor_radius.fit.min;
			roughness_Model.cor_radius.confidence.max = roughness_Model.cor_radius.fit.max;
			roughness_Model.cor_radius.confidence.num_Points = default_num_confidence_points;
		}
		// fractality / jagednesss
		{
			roughness_Model.fractal_alpha.value = 0.5;
			roughness_Model.fractal_alpha.fit.is_Fitable = false;
			roughness_Model.fractal_alpha.fit.min = 0;
			roughness_Model.fractal_alpha.fit.max = 1;
			roughness_Model.fractal_alpha.indicator.whats_This = whats_This_Fractal_Alpha;
			roughness_Model.fractal_alpha.indicator.item_Id = id;

			roughness_Model.fractal_alpha.confidence.calc_Conf_Interval = false;
			roughness_Model.fractal_alpha.confidence.min = roughness_Model.fractal_alpha.fit.min;
			roughness_Model.fractal_alpha.confidence.max = roughness_Model.fractal_alpha.fit.max;
			roughness_Model.fractal_alpha.confidence.num_Points = default_num_confidence_points;
		}
		// particle volume omega
		{
			roughness_Model.omega.value = 1000;
			roughness_Model.omega.fit.is_Fitable = false;
			roughness_Model.omega.fit.min = 300;
			roughness_Model.omega.fit.max = 20000;
			roughness_Model.omega.indicator.whats_This = whats_This_Linear_PSD_Omega;
			roughness_Model.omega.indicator.item_Id = id;

			roughness_Model.omega.confidence.calc_Conf_Interval = false;
			roughness_Model.omega.confidence.min = roughness_Model.omega.fit.min;
			roughness_Model.omega.confidence.max = roughness_Model.omega.fit.max;
			roughness_Model.omega.confidence.num_Points = default_num_confidence_points;
		}
		// factor in growth exponent (depends on alpha?)
		{
			roughness_Model.mu.value = 20;
			roughness_Model.mu.fit.is_Fitable = false;
			roughness_Model.mu.fit.min = 5;
			roughness_Model.mu.fit.max = 100;
			roughness_Model.mu.indicator.whats_This = whats_This_Linear_PSD_Exponenta_Mu;
			roughness_Model.mu.indicator.item_Id = id;

			roughness_Model.mu.confidence.calc_Conf_Interval = false;
			roughness_Model.mu.confidence.min = roughness_Model.mu.fit.min;
			roughness_Model.mu.confidence.max = roughness_Model.mu.fit.max;
			roughness_Model.mu.confidence.num_Points = default_num_confidence_points;
		}
	}

	// Layer
	{
		// thickness
		{
			thickness.value	= layer_default_thickness;
			thickness.fit.is_Fitable = false;
			thickness.fit.min = thickness.value*(1-dispersion);
			thickness.fit.max = thickness.value*(2+dispersion);
			thickness.indicator.whats_This = whats_This_Thickness;
			thickness.indicator.item_Id = id;

			thickness.confidence.calc_Conf_Interval = false;
			thickness.confidence.min = thickness.fit.min;
			thickness.confidence.max = thickness.fit.max;
			thickness.confidence.num_Points = default_num_confidence_points;
		}
		// thickness drift
		{
			thickness_Drift.is_Drift_Line = false;
			thickness_Drift.is_Drift_Sine = false;
			thickness_Drift.is_Drift_Rand = false;

			thickness_Drift.show_Drift_Line = true;
			thickness_Drift.show_Drift_Rand = true;
			thickness_Drift.show_Drift_Sine = false;
			{
				thickness_Drift.drift_Line_Value.value = 0;
				thickness_Drift.drift_Line_Value.fit.min = -thickness_Drift.drift_Line_Value.value*2;
				thickness_Drift.drift_Line_Value.fit.max =  thickness_Drift.drift_Line_Value.value*2;
				thickness_Drift.drift_Line_Value.indicator.whats_This = whats_This_Thickness_Drift_Line_Value;
				thickness_Drift.drift_Line_Value.indicator.item_Id = id;

				thickness_Drift.drift_Line_Value.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Line_Value.confidence.min = thickness_Drift.drift_Line_Value.fit.min;
				thickness_Drift.drift_Line_Value.confidence.max = thickness_Drift.drift_Line_Value.fit.max;
				thickness_Drift.drift_Line_Value.confidence.num_Points = default_num_confidence_points;
			}
			{
				thickness_Drift.drift_Rand_Rms.value = 0;
				thickness_Drift.drift_Rand_Rms.fit.min = 0;
				thickness_Drift.drift_Rand_Rms.fit.max = thickness_Drift.drift_Rand_Rms.value*2;
				thickness_Drift.drift_Rand_Rms.indicator.whats_This = whats_This_Thickness_Drift_Rand_Rms;
				thickness_Drift.drift_Rand_Rms.indicator.item_Id = id;

				thickness_Drift.drift_Rand_Rms.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Rand_Rms.confidence.min = thickness_Drift.drift_Rand_Rms.fit.min;
				thickness_Drift.drift_Rand_Rms.confidence.max = thickness_Drift.drift_Rand_Rms.fit.max;
				thickness_Drift.drift_Rand_Rms.confidence.num_Points = default_num_confidence_points;
			}
			{
				thickness_Drift.drift_Sine_Amplitude.value = 0;
				thickness_Drift.drift_Sine_Amplitude.fit.min = 0;
				thickness_Drift.drift_Sine_Amplitude.fit.max = thickness_Drift.drift_Sine_Amplitude.value*2;
				thickness_Drift.drift_Sine_Amplitude.indicator.whats_This = whats_This_Thickness_Drift_Sine_Amplitude;
				thickness_Drift.drift_Sine_Amplitude.indicator.item_Id = id;

				thickness_Drift.drift_Sine_Amplitude.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Sine_Amplitude.confidence.min = thickness_Drift.drift_Sine_Amplitude.fit.min;
				thickness_Drift.drift_Sine_Amplitude.confidence.max = thickness_Drift.drift_Sine_Amplitude.fit.max;
				thickness_Drift.drift_Sine_Amplitude.confidence.num_Points = default_num_confidence_points;
			}
			{
				thickness_Drift.drift_Sine_Frequency.value = 0.333333333333333333333;
				thickness_Drift.drift_Sine_Frequency.fit.min = thickness_Drift.drift_Sine_Frequency.value*(1-dispersion);
				thickness_Drift.drift_Sine_Frequency.fit.max = thickness_Drift.drift_Sine_Frequency.value*(1+dispersion);
				thickness_Drift.drift_Sine_Frequency.indicator.whats_This = whats_This_Thickness_Drift_Sine_Frequency;
				thickness_Drift.drift_Sine_Frequency.indicator.item_Id = id;

				thickness_Drift.drift_Sine_Frequency.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Sine_Frequency.confidence.min = thickness_Drift.drift_Sine_Frequency.fit.min;
				thickness_Drift.drift_Sine_Frequency.confidence.max = thickness_Drift.drift_Sine_Frequency.fit.max;
				thickness_Drift.drift_Sine_Frequency.confidence.num_Points = default_num_confidence_points;
			}
			{
				thickness_Drift.drift_Sine_Phase.value = 0;
				thickness_Drift.drift_Sine_Phase.fit.min = 0;
				thickness_Drift.drift_Sine_Phase.fit.max = 1;
				thickness_Drift.drift_Sine_Phase.indicator.whats_This = whats_This_Thickness_Drift_Sine_Phase;
				thickness_Drift.drift_Sine_Phase.indicator.item_Id = id;

				thickness_Drift.drift_Sine_Phase.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Sine_Phase.confidence.min = thickness_Drift.drift_Sine_Phase.fit.min;
				thickness_Drift.drift_Sine_Phase.confidence.max = thickness_Drift.drift_Sine_Phase.fit.max;
				thickness_Drift.drift_Sine_Phase.confidence.num_Points = default_num_confidence_points;
			}
		}
		// sigma drift
		{
			sigma_Diffuse_Drift.is_Drift_Line = false;
			sigma_Diffuse_Drift.is_Drift_Sine = false;
			sigma_Diffuse_Drift.is_Drift_Rand = false;

			sigma_Diffuse_Drift.show_Drift_Line = false;
			sigma_Diffuse_Drift.show_Drift_Rand = false;
			sigma_Diffuse_Drift.show_Drift_Sine = false;
			{
				sigma_Diffuse_Drift.drift_Line_Value.value = 0;
				sigma_Diffuse_Drift.drift_Line_Value.fit.min = -thickness_Drift.drift_Line_Value.value*2;
				sigma_Diffuse_Drift.drift_Line_Value.fit.max =  thickness_Drift.drift_Line_Value.value*2;
				sigma_Diffuse_Drift.drift_Line_Value.indicator.whats_This = whats_This_Sigma_Drift_Line_Value;
				sigma_Diffuse_Drift.drift_Line_Value.indicator.item_Id = id;

				sigma_Diffuse_Drift.drift_Line_Value.confidence.calc_Conf_Interval = false;
				sigma_Diffuse_Drift.drift_Line_Value.confidence.min = sigma_Diffuse_Drift.drift_Line_Value.fit.min;
				sigma_Diffuse_Drift.drift_Line_Value.confidence.max = sigma_Diffuse_Drift.drift_Line_Value.fit.max;
				sigma_Diffuse_Drift.drift_Line_Value.confidence.num_Points = default_num_confidence_points;
			}
			{
				sigma_Diffuse_Drift.drift_Rand_Rms.value = 0;
				sigma_Diffuse_Drift.drift_Rand_Rms.fit.min = 0;
				sigma_Diffuse_Drift.drift_Rand_Rms.fit.max = thickness_Drift.drift_Rand_Rms.value*2;
				sigma_Diffuse_Drift.drift_Rand_Rms.indicator.whats_This = whats_This_Sigma_Drift_Rand_Rms;
				sigma_Diffuse_Drift.drift_Rand_Rms.indicator.item_Id = id;

				sigma_Diffuse_Drift.drift_Rand_Rms.confidence.calc_Conf_Interval = false;
				sigma_Diffuse_Drift.drift_Rand_Rms.confidence.min = sigma_Diffuse_Drift.drift_Rand_Rms.fit.min;
				sigma_Diffuse_Drift.drift_Rand_Rms.confidence.max = sigma_Diffuse_Drift.drift_Rand_Rms.fit.max;
				sigma_Diffuse_Drift.drift_Rand_Rms.confidence.num_Points = default_num_confidence_points;
			}
			{
				sigma_Diffuse_Drift.drift_Sine_Amplitude.value = 0;
				sigma_Diffuse_Drift.drift_Sine_Amplitude.fit.min = 0;
				sigma_Diffuse_Drift.drift_Sine_Amplitude.fit.max = thickness_Drift.drift_Sine_Amplitude.value*2;
				sigma_Diffuse_Drift.drift_Sine_Amplitude.indicator.whats_This = whats_This_Sigma_Drift_Sine_Amplitude;
				sigma_Diffuse_Drift.drift_Sine_Amplitude.indicator.item_Id = id;

				sigma_Diffuse_Drift.drift_Sine_Amplitude.confidence.calc_Conf_Interval = false;
				sigma_Diffuse_Drift.drift_Sine_Amplitude.confidence.min = sigma_Diffuse_Drift.drift_Sine_Amplitude.fit.min;
				sigma_Diffuse_Drift.drift_Sine_Amplitude.confidence.max = sigma_Diffuse_Drift.drift_Sine_Amplitude.fit.max;
				sigma_Diffuse_Drift.drift_Sine_Amplitude.confidence.num_Points = default_num_confidence_points;
			}
			{
				sigma_Diffuse_Drift.drift_Sine_Frequency.value = 0.333333333333333333333;
				sigma_Diffuse_Drift.drift_Sine_Frequency.fit.min = thickness_Drift.drift_Sine_Frequency.value*(1-dispersion);
				sigma_Diffuse_Drift.drift_Sine_Frequency.fit.max = thickness_Drift.drift_Sine_Frequency.value*(1+dispersion);
				sigma_Diffuse_Drift.drift_Sine_Frequency.indicator.whats_This = whats_This_Sigma_Drift_Sine_Frequency;
				sigma_Diffuse_Drift.drift_Sine_Frequency.indicator.item_Id = id;

				sigma_Diffuse_Drift.drift_Sine_Frequency.confidence.calc_Conf_Interval = false;
				sigma_Diffuse_Drift.drift_Sine_Frequency.confidence.min = sigma_Diffuse_Drift.drift_Sine_Frequency.fit.min;
				sigma_Diffuse_Drift.drift_Sine_Frequency.confidence.max = sigma_Diffuse_Drift.drift_Sine_Frequency.fit.max;
				sigma_Diffuse_Drift.drift_Sine_Frequency.confidence.num_Points = default_num_confidence_points;
			}
			{
				sigma_Diffuse_Drift.drift_Sine_Phase.value = 0;
				sigma_Diffuse_Drift.drift_Sine_Phase.fit.min = 0;
				sigma_Diffuse_Drift.drift_Sine_Phase.fit.max = 1;
				sigma_Diffuse_Drift.drift_Sine_Phase.indicator.whats_This = whats_This_Sigma_Drift_Sine_Phase;
				sigma_Diffuse_Drift.drift_Sine_Phase.indicator.item_Id = id;

				sigma_Diffuse_Drift.drift_Sine_Phase.confidence.calc_Conf_Interval = false;
				sigma_Diffuse_Drift.drift_Sine_Phase.confidence.min = sigma_Diffuse_Drift.drift_Sine_Phase.fit.min;
				sigma_Diffuse_Drift.drift_Sine_Phase.confidence.max = sigma_Diffuse_Drift.drift_Sine_Phase.fit.max;
				sigma_Diffuse_Drift.drift_Sine_Phase.confidence.num_Points = default_num_confidence_points;
			}
		}
	}

	// Multilayer
	{
		// num repetition
		{
			num_Repetition.parameter.value = stack_default_number_of_repetition;
			num_Repetition.parameter.coupled.is_Coupled = false;
			num_Repetition.parameter.confidence.calc_Conf_Interval = false;
			num_Repetition.parameter.fit.is_Fitable = false;
			num_Repetition.parameter.fit.min = num_Repetition.parameter.value*(1-dispersion);
			num_Repetition.parameter.fit.max = num_Repetition.parameter.value*(1+dispersion);
			num_Repetition.parameter.indicator.whats_This = whats_This_Num_Repetitions;
			num_Repetition.parameter.indicator.item_Id = id;
		}
		// period
		{
			period.value = stack_default_period;
			period.fit.is_Fitable = false;
			period.fit.min = period.value*(1-dispersion);
			period.fit.max = period.value*(1+dispersion);
			period.indicator.whats_This = whats_This_Period;
			period.indicator.item_Id = id;

			period.confidence.calc_Conf_Interval = false;
			period.confidence.min = period.fit.min;
			period.confidence.max = period.fit.max;
			period.confidence.num_Points = default_num_confidence_points;
		}
		// gamma
		{
			gamma.value	= stack_default_gamma;
			gamma.fit.is_Fitable = false;
			gamma.fit.min = 0;
			gamma.fit.max = 1;
			gamma.indicator.whats_This = whats_This_Gamma;
			gamma.indicator.item_Id = id;

			gamma.confidence.calc_Conf_Interval = false;
			gamma.confidence.min = gamma.fit.min;
			gamma.confidence.max = gamma.fit.max;
			gamma.confidence.num_Points = default_num_confidence_points;
		}
	}
	// Layer, Multilayer, Aperiodic
	{
		step_Value_Change = step_thickness_transfer;
	}
}

void Data::reset_All_IDs()
{
	id = Global_Definitions::generate_Id();
	///---------------------------------------------
	// Measurement
	//---------------------------------------------
		// angle
		beam_Theta_0_Angle		.indicator.id = Global_Definitions::generate_Id(); beam_Theta_0_Angle		.indicator.item_Id = id;	beam_Theta_0_Angle		.coupled.clear_Coupled();
		detector_Theta_Angle	.indicator.id = Global_Definitions::generate_Id(); detector_Theta_Angle		.indicator.item_Id = id;	detector_Theta_Angle	.coupled.clear_Coupled();
		detector_Phi_Angle		.indicator.id = Global_Definitions::generate_Id(); detector_Phi_Angle		.indicator.item_Id = id;	detector_Phi_Angle		.coupled.clear_Coupled();

		// wavelength
		wavelength				.indicator.id = Global_Definitions::generate_Id(); wavelength				.indicator.item_Id = id;	wavelength				.coupled.clear_Coupled();
	///---------------------------------------------
	///---------------------------------------------
	// Ambient, Layer, Substrate
	//---------------------------------------------
		relative_Density.indicator.id = Global_Definitions::generate_Id(); relative_Density	.indicator.item_Id = id;		relative_Density	.coupled.clear_Coupled();
		absolute_Density.indicator.id = Global_Definitions::generate_Id(); absolute_Density	.indicator.item_Id = id;		absolute_Density	.coupled.clear_Coupled();

		permittivity	.indicator.id = Global_Definitions::generate_Id(); permittivity		.indicator.item_Id = id;		permittivity		.coupled.clear_Coupled();
		absorption		.indicator.id = Global_Definitions::generate_Id(); absorption		.indicator.item_Id = id;		absorption			.coupled.clear_Coupled();

		// material composition
		for(auto& stoich : composition)
		{
			stoich.composition.indicator.id = Global_Definitions::generate_Id(); stoich.composition.indicator.item_Id = id;	stoich.composition	.coupled.clear_Coupled();
		}
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Substrate
	//---------------------------------------------
		sigma_Diffuse.indicator.id = Global_Definitions::generate_Id();			sigma_Diffuse.indicator.item_Id = id;			sigma_Diffuse.coupled.clear_Coupled();
		for(Interlayer& inter : interlayer_Composition)
		{
			inter.interlayer.indicator.id = Global_Definitions::generate_Id(); inter.interlayer	.indicator.item_Id = id;	inter.interlayer.coupled.clear_Coupled();
			inter.my_Sigma_Diffuse	.indicator.id = Global_Definitions::generate_Id(); inter.my_Sigma_Diffuse	.indicator.item_Id = id;	inter.my_Sigma_Diffuse.	 coupled.clear_Coupled();
		}

		roughness_Model.sigma			   .indicator.id = Global_Definitions::generate_Id(); roughness_Model.sigma.			  indicator.item_Id = id;	roughness_Model.sigma.				coupled.clear_Coupled();
		roughness_Model.cor_radius		   .indicator.id = Global_Definitions::generate_Id(); roughness_Model.cor_radius.		  indicator.item_Id = id;	roughness_Model.cor_radius.			coupled.clear_Coupled();
		roughness_Model.fractal_alpha	   .indicator.id = Global_Definitions::generate_Id(); roughness_Model.fractal_alpha.	  indicator.item_Id = id;	roughness_Model.fractal_alpha.		coupled.clear_Coupled();
		roughness_Model.omega			   .indicator.id = Global_Definitions::generate_Id(); roughness_Model.omega.			  indicator.item_Id = id;	roughness_Model.omega.				coupled.clear_Coupled();
		roughness_Model.mu				   .indicator.id = Global_Definitions::generate_Id(); roughness_Model.mu.				  indicator.item_Id = id;	roughness_Model.mu.					coupled.clear_Coupled();
	///---------------------------------------------
	///---------------------------------------------
	// Layer
	//---------------------------------------------
		thickness.indicator.id = Global_Definitions::generate_Id(); thickness.indicator.item_Id = id;		thickness.coupled.clear_Coupled();

		// thickness drift
		thickness_Drift.drift_Line_Value	.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Line_Value		.indicator.item_Id = id;	thickness_Drift.drift_Line_Value.		coupled.clear_Coupled();
		thickness_Drift.drift_Rand_Rms		.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Rand_Rms		.indicator.item_Id = id;	thickness_Drift.drift_Rand_Rms.			coupled.clear_Coupled();
		thickness_Drift.drift_Sine_Amplitude.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Sine_Amplitude	.indicator.item_Id = id;	thickness_Drift.drift_Sine_Amplitude.	coupled.clear_Coupled();
		thickness_Drift.drift_Sine_Frequency.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Sine_Frequency	.indicator.item_Id = id;	thickness_Drift.drift_Sine_Frequency.	coupled.clear_Coupled();
		thickness_Drift.drift_Sine_Phase	.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Sine_Phase		.indicator.item_Id = id;	thickness_Drift.drift_Sine_Phase.		coupled.clear_Coupled();

		// sigma drift
		sigma_Diffuse_Drift.drift_Line_Value		.indicator.id = Global_Definitions::generate_Id(); sigma_Diffuse_Drift.drift_Line_Value		.indicator.item_Id = id;		sigma_Diffuse_Drift.drift_Line_Value	.coupled.clear_Coupled();
		sigma_Diffuse_Drift.drift_Rand_Rms			.indicator.id = Global_Definitions::generate_Id(); sigma_Diffuse_Drift.drift_Rand_Rms		.indicator.item_Id = id;		sigma_Diffuse_Drift.drift_Rand_Rms		.coupled.clear_Coupled();
		sigma_Diffuse_Drift.drift_Sine_Amplitude	.indicator.id = Global_Definitions::generate_Id(); sigma_Diffuse_Drift.drift_Sine_Amplitude	.indicator.item_Id = id;		sigma_Diffuse_Drift.drift_Sine_Amplitude.coupled.clear_Coupled();
		sigma_Diffuse_Drift.drift_Sine_Frequency	.indicator.id = Global_Definitions::generate_Id(); sigma_Diffuse_Drift.drift_Sine_Frequency	.indicator.item_Id = id;		sigma_Diffuse_Drift.drift_Sine_Frequency.coupled.clear_Coupled();
		sigma_Diffuse_Drift.drift_Sine_Phase		.indicator.id = Global_Definitions::generate_Id(); sigma_Diffuse_Drift.drift_Sine_Phase		.indicator.item_Id = id;		sigma_Diffuse_Drift.drift_Sine_Phase	.coupled.clear_Coupled();
	///---------------------------------------------
	///---------------------------------------------
	// Multilayer, Aperiodic
	//---------------------------------------------
		num_Repetition.parameter.indicator.id = Global_Definitions::generate_Id(); num_Repetition.parameter	.indicator.item_Id = id;	num_Repetition.parameter.coupled.clear_Coupled();
		period.indicator.id					  = Global_Definitions::generate_Id(); period	.indicator.item_Id				   = id;	period.coupled.clear_Coupled();
		gamma .indicator.id					  = Global_Definitions::generate_Id(); gamma	.indicator.item_Id				   = id;	gamma. coupled.clear_Coupled();
}

struct integration_Params_Beam
{
	double beam_Profile_Spreading;
	double beam_Size;
};

double beam_Func(double z, void* params)
{
	Beam_Geometry* iPB = reinterpret_cast<Beam_Geometry*>(params);

	double aConst = 2.*pow(1.-1./sqrt(2.),1./2);
	if((z>(-iPB->size/aConst) ) &&	(z< (iPB->size/aConst)))
	{
		double output = pow(1.- pow(abs(z)*aConst/iPB->size,2),2);
		if(output!=output) qInfo()<< "Unwrapped_Reflection::beam_Func  :  NaN" << endl;
		return output;
	}
	else
	{
		return 0;
	}
}

void Data::calc_Instrumental_Factor()
{
	// TODO
//	int num_Points = 0;

//	// effect of beam size
//	double error;
//	int key = GSL_INTEG_GAUSS15;
//	const double epsabs = 1e-3;
//	const double epsrel = 1e-3;
//	size_t limit = 1000;
//	gsl_integration_workspace* w = gsl_integration_workspace_alloc (limit);
//	gsl_function F = { &beam_Func, &beam_Geometry };

//	// calculate denominator
//	double denominator=1;
//	gsl_integration_qag(&F,-5*beam_Geometry.size, 5*beam_Geometry.size, epsabs, epsrel, limit, key, w, &denominator, &error);

//	if(active_Parameter_Whats_This == whats_This_Beam_Theta_0_Angle)		{ num_Points = beam_Theta_0_Cos2_Vec.size();	  }
//	if(active_Parameter_Whats_This == whats_This_Wavelength){ num_Points = lambda_Vec.size(); }
//	footprint_Factor_Vec.resize(num_Points, 1);

//	// special cases
//	if( (denominator < DBL_MIN) || (beam_Geometry.size<DBL_EPSILON) )	{return;}

//	// calculate factor
//	if(active_Parameter_Whats_This == whats_This_Beam_Theta_0_Angle)
//	{
//		double sin_Grad, min, max, result;
//		for(int i=0; i<num_Points; ++i)
//		{
//			sin_Grad = sqrt(1-beam_Theta_0_Cos2_Vec[i]);

//			if(sin_Grad > DBL_EPSILON)
//			{
//				min = (sample_Geometry.x_Position-sample_Geometry.size/2.)*sin_Grad;
//				max = (sample_Geometry.x_Position+sample_Geometry.size/2.)*sin_Grad;

//				// if reasonable to integrate
//				if( min>-3*beam_Geometry.size ||
//					max< 3*beam_Geometry.size )
//				{
//					gsl_integration_qag(&F,min,max,epsabs,epsrel,limit,key,w,&result,&error);
//				} else
//				{
//					result = denominator;
//				}
//			} else
//			{
//				result = 0.5*denominator;
//			}
//			// fill
//			footprint_Factor_Vec[i] = result/denominator;
//		}
//	} else
//	if(active_Parameter_Whats_This == whats_This_Wavelength)
//	{
//		double sin_Grad, min, max, result;
//		sin_Grad = sqrt(1-beam_Theta_0_Cos2_Value);

//		if(sin_Grad > DBL_EPSILON)
//		{
//			min = (sample_Geometry.x_Position-sample_Geometry.size/2.)*sin_Grad;
//			max = (sample_Geometry.x_Position+sample_Geometry.size/2.)*sin_Grad;

//			// if reasonable to integrate
//			if( min>-1*beam_Geometry.size ||
//				max< 1*beam_Geometry.size )
//			{
//				gsl_integration_qag(&F,min,max,epsabs,epsrel,limit,key,w,&result,&error);
//			} else
//			{
//				result = denominator;
//			}
//		} else
//		{
//			result = 0.5*denominator;
//		}
//		result /= denominator;

//		// fill
//		for(int i=0; i<num_Points; ++i)
//		{
//			footprint_Factor_Vec[i] = result;
//		}
//	}

//	gsl_integration_workspace_free(w);
}

void Data::calc_Mixed_Resolution()
{
	// TODO
//	if(active_Parameter_Whats_This == whats_This_Beam_Theta_0_Angle)
//	{
//		angular_Resolution_Mixed.resize(beam_Theta_0_Angle_Vec.size());
//		for(int i=0; i<angular_Resolution_Mixed.size(); ++i)
//		{
//			double angle_Temp = beam_Theta_0_Angle_Vec[i]*M_PI/180.; // in radians
//			angular_Resolution_Mixed[i] = sqrt(
//						pow(
//							beam_Theta_0_Distribution.FWHM_distribution // in degrees
//							,2) +
//						pow(
//							2*( angle_Temp - asin((1.-spectral_Distribution.FWHM_distribution/2)*sin(angle_Temp)) ) * 180./M_PI  // back to degrees
//							,2));
//		}
//	}
//	if(active_Parameter_Whats_This == whats_This_Wavelength)
//	{
//		spectral_Resolution_Mixed.resize(lambda_Vec.size());
//		for(int i=0; i<spectral_Resolution_Mixed.size(); ++i)
//		{
//			double angular_Component = 0;
//			if(beam_Theta_0_Angle_Value-beam_Theta_0_Distribution.FWHM_distribution/2>0 && beam_Theta_0_Angle_Value>0)
//			{
//				double angle_Temp = beam_Theta_0_Angle_Value*M_PI/180.; // in radians
//				double angular_Resolution_Temp = beam_Theta_0_Distribution.FWHM_distribution*M_PI/180.; // in radians
//				angular_Component = 2*(1-sin(angle_Temp-angular_Resolution_Temp/2)/sin(angle_Temp));
//			}

//			spectral_Resolution_Mixed[i] = sqrt(
//						pow(
//							spectral_Distribution.FWHM_distribution*lambda_Vec[i]
//							,2) +
//						pow(
//							angular_Component*lambda_Vec[i]
//							,2));
//		}
//	}
}

QString Data::get_Composed_Material()
{
	QString composed;
	for(int i=0; i<composition.size(); ++i)
	{
		composed += composition[i].type;
		if(abs(composition[i].composition.value-1)>DBL_EPSILON) composed += Locale.toString(composition[i].composition.value,line_edit_short_double_format,line_edit_composition_precision);
	}
	return composed;
}

void Data::fill_Potentially_Fitable_Parameters_Vector()
{
	potentially_Fitable_Parameters.clear();

	///---------------------------------------------
	// Measurement
	//---------------------------------------------
	if(item_Type == item_Type_Measurement)
	{
		potentially_Fitable_Parameters.push_back(&wavelength);
		potentially_Fitable_Parameters.push_back(&beam_Theta_0_Angle);
		potentially_Fitable_Parameters.push_back(&detector_Theta_Angle);
		potentially_Fitable_Parameters.push_back(&detector_Phi_Angle);
	}
	///---------------------------------------------
	///---------------------------------------------
	// Ambient, Layer, Substrate
	//---------------------------------------------
	if( item_Type == item_Type_Ambient ||
		item_Type == item_Type_Layer   ||
		item_Type == item_Type_Substrate )
	{
		if(!composed_Material)
			potentially_Fitable_Parameters.push_back(&relative_Density);
		else
			potentially_Fitable_Parameters.push_back(&absolute_Density);

		// tabular material
//		all_Parameters.push_back(&permittivity);
//		all_Parameters.push_back(&absorption);

		// material composition
		if(composition.size()>1 && composed_Material)
		{
			for(Stoichiometry& stoichiometry : composition)
			{
				potentially_Fitable_Parameters.push_back(&stoichiometry.composition);
			}
		}
	}
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Substrate
	//---------------------------------------------
	if( item_Type == item_Type_Layer   ||
		item_Type == item_Type_Substrate )
	{
		int enabled_Counter=0;
		for(Interlayer& interlayer : interlayer_Composition)
		{
			if(interlayer.enabled)
			{
				++enabled_Counter;
				potentially_Fitable_Parameters.push_back(&interlayer.interlayer);

				if(!common_Sigma_Diffuse)
					potentially_Fitable_Parameters.push_back(&interlayer.my_Sigma_Diffuse);
			}
		}

		if(common_Sigma_Diffuse && enabled_Counter>0)
		{	potentially_Fitable_Parameters.push_back(&sigma_Diffuse); }

		if(roughness_Model.is_Enabled)
		{
			potentially_Fitable_Parameters.push_back(&roughness_Model.sigma);
			potentially_Fitable_Parameters.push_back(&roughness_Model.cor_radius);
			potentially_Fitable_Parameters.push_back(&roughness_Model.fractal_alpha);
			potentially_Fitable_Parameters.push_back(&roughness_Model.mu);

			if( roughness_Model.model == linear_Growth_Model )
			{
				potentially_Fitable_Parameters.push_back(&roughness_Model.omega);
			}
		}
	}
	///---------------------------------------------
	///---------------------------------------------
	// Layer
	//---------------------------------------------
	if( item_Type == item_Type_Layer )
	{
		potentially_Fitable_Parameters.push_back(&thickness);

		// thickness drift
		if(thickness_Drift.is_Drift_Line)
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Line_Value);

		if(thickness_Drift.is_Drift_Sine)
		{
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Sine_Amplitude);
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Sine_Frequency);
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Sine_Phase);
		}

		if(thickness_Drift.is_Drift_Rand)
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Rand_Rms);

		// sigma drift
		if(sigma_Diffuse_Drift.is_Drift_Line)
			potentially_Fitable_Parameters.push_back(&sigma_Diffuse_Drift.drift_Line_Value);

		if(sigma_Diffuse_Drift.is_Drift_Sine)
		{
			potentially_Fitable_Parameters.push_back(&sigma_Diffuse_Drift.drift_Sine_Amplitude);
			potentially_Fitable_Parameters.push_back(&sigma_Diffuse_Drift.drift_Sine_Frequency);
			potentially_Fitable_Parameters.push_back(&sigma_Diffuse_Drift.drift_Sine_Phase);
		}

		if(sigma_Diffuse_Drift.is_Drift_Rand)
		{	potentially_Fitable_Parameters.push_back(&sigma_Diffuse_Drift.drift_Rand_Rms); }
	}

	///---------------------------------------------
	///---------------------------------------------
	// Multilayer
	//---------------------------------------------
	if( item_Type == item_Type_Multilayer )
	{
		potentially_Fitable_Parameters.push_back(&num_Repetition.parameter);
		potentially_Fitable_Parameters.push_back(&period);
		potentially_Fitable_Parameters.push_back(&gamma);
	}

	///---------------------------------------------
	///---------------------------------------------
	// Regular Aperiodic
	//---------------------------------------------
	if( item_Type == item_Type_Regular_Aperiodic )
	{
		for(int i=0; i<regular_Components.size(); i++)
		{
			for(int n=0; n<num_Repetition.value(); n++)
			{
				Data& regular_Data = regular_Components[i].components[n];
				if(!regular_Components[i].is_Common_Thickness) {potentially_Fitable_Parameters.push_back(&regular_Data.thickness);}
				if(!regular_Components[i].is_Common_Sigma)	   {potentially_Fitable_Parameters.push_back(&regular_Data.sigma_Diffuse);}
			}
		}
	}
}

void Data::prepare_Layer_For_Regular_Component()
{
	item_Enabled = true;
///---------------------------------------------
///---------------------------------------------
// Ambient, Layer, Substrate
//---------------------------------------------
	make_Free(relative_Density);
	make_Free(absolute_Density);

	// tabular material
	make_Free(permittivity);
	make_Free(absorption);

	for(Stoichiometry& comp : composition)	{
		make_Free(comp.composition);
	}
///---------------------------------------------
///---------------------------------------------
// Layer, Substrate
//---------------------------------------------
	common_Sigma_Diffuse = true;
	make_Free(sigma_Diffuse);

	for(Interlayer& inter : interlayer_Composition)
	{
		make_Free(inter.interlayer);
		make_Free(inter.my_Sigma_Diffuse);
	}

	make_Free(roughness_Model.sigma);
	make_Free(roughness_Model.cor_radius);
	make_Free(roughness_Model.fractal_alpha);
	make_Free(roughness_Model.omega);
	make_Free(roughness_Model.mu);

///---------------------------------------------
///---------------------------------------------
// Layer
//---------------------------------------------
	make_Free(thickness);
	make_Free(thickness);

	make_Free(thickness_Drift.drift_Line_Value);
	make_Free(thickness_Drift.drift_Sine_Amplitude);
	make_Free(thickness_Drift.drift_Sine_Frequency);
	make_Free(thickness_Drift.drift_Sine_Phase);
	make_Free(thickness_Drift.drift_Rand_Rms);

	make_Free(sigma_Diffuse_Drift.drift_Line_Value);
	make_Free(sigma_Diffuse_Drift.drift_Sine_Amplitude);
	make_Free(sigma_Diffuse_Drift.drift_Sine_Frequency);
	make_Free(sigma_Diffuse_Drift.drift_Sine_Phase);
	make_Free(sigma_Diffuse_Drift.drift_Rand_Rms);
///---------------------------------------------
///---------------------------------------------
// Multilayer, Aperiodic
//---------------------------------------------
	make_Free(num_Repetition.parameter);
	make_Free(period);
	make_Free(gamma);
}

void Data::make_Free(Parameter &parameter)
{
	parameter.fit.is_Fitable = false;
	parameter.confidence.calc_Conf_Interval = false;

	// TODO do we need it?
//	parameter.coupled.master.exist = false;
//	parameter.coupled.master.id = 0;
//	parameter.coupled.slaves.clear();
}

// serialization

QDataStream& operator <<( QDataStream& stream, const Regular_Component& regular_Component )
{
	return stream
				<< regular_Component.top_Id << regular_Component.components << regular_Component.min_Max_Values			//
				<< regular_Component.is_Common_Thickness << regular_Component.is_Common_Sigma							//
				<< regular_Component.use_Soft_Restrictions << regular_Component.threshold << regular_Component.Q_factor // since 1.8.11
				<< regular_Component.power; // since 1.8.13
}
QDataStream& operator >>( QDataStream& stream,		 Regular_Component& regular_Component )
{
	if(Global_Variables::check_Loaded_Version(1,8,11))
	{stream >> regular_Component.top_Id >> regular_Component.components >> regular_Component.min_Max_Values			    //
			>> regular_Component.is_Common_Thickness >> regular_Component.is_Common_Sigma							    //
			>> regular_Component.use_Soft_Restrictions >> regular_Component.threshold >> regular_Component.Q_factor;}	// since 1.8.11
	if(Global_Variables::check_Loaded_Version(1,8,13))
	{stream >> regular_Component.power; }				// since 1.8.13

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Data& data )
{
	return stream
				<< data.parent_Item_Type << data.item_Type << data.id
				<< data.item_Enabled		// since 1.7.7
			// Measurement
				<< data.measurement_Type << data.argument_Type
				<< data.wavelength
				<< data.beam_Theta_0_Angle << data.beam_Theta_0_Specular_Position
				<< data.detector_Theta_Angle << data.detector_Theta_Offset
				<< data.detector_Phi_Angle

				<< data.spectral_Distribution << data.beam_Theta_0_Distribution << data.beam_Phi_0_Distribution
				<< data.detector_1D << data.detector_2D

				<< data.beam_Geometry << data.sample_Geometry
				<< data.polarization << data.background

			// Ambient, Layer, Substrate
				<< data.composed_Material << data.material << data.approved_Material << data.absolute_Density << data.relative_Density
				<< data.separate_Optical_Constants << data.permittivity << data.absorption << data.composition
			// Layer, Substrate
				<< data.common_Sigma_Diffuse << data.sigma_Diffuse << data.interlayer_Composition
				<< data.roughness_Model
			// Layer
				<< data.layer_Index << data.has_Parent << data.thickness << data.thickness_Drift << data.sigma_Diffuse_Drift
			// Multilayer
				<< data.first_Layer_Index << data.last_Layer_Index << data.num_Repetition << data.period << data.gamma
			// Layer, Multilayer, Aperiodic
				<< data.step_Value_Change 	// since 1.8.3
				<< data.regular_Components; // since 1.8.11
}
QDataStream& operator >>( QDataStream& stream,		 Data& data )
{
	stream		>> data.parent_Item_Type >> data.item_Type >> data.id;
	if(Global_Variables::check_Loaded_Version(1,7,7))
	{stream >> data.item_Enabled; }	// since 1.7.7

	// Measurement
	if(Global_Variables::check_Loaded_Version(1,11,0)) //since 1.11.0
	{
		stream	>> data.measurement_Type >> data.argument_Type
				>> data.wavelength
				>> data.beam_Theta_0_Angle >> data.beam_Theta_0_Specular_Position
				>> data.detector_Theta_Angle >> data.detector_Theta_Offset
				>> data.detector_Phi_Angle

				>> data.spectral_Distribution >> data.beam_Theta_0_Distribution >> data.beam_Phi_0_Distribution
				>> data.detector_1D >> data.detector_2D

				>> data.beam_Geometry >> data.sample_Geometry
				>> data.polarization >> data.background;
	} else // before 1.11.0
	{
		stream	>> data.beam_Theta_0_Angle;

		// ----------------------------
		QVector<double> beam_Theta_0_Cos2_Vec;	stream >> beam_Theta_0_Cos2_Vec;
		QVector<double> beam_Theta_0_Angle_Vec; stream >> beam_Theta_0_Angle_Vec;
		double beam_Theta_0_Cos2_Value;			stream >> beam_Theta_0_Cos2_Value;
		double beam_Theta_0_Angle_Value;		stream >> beam_Theta_0_Angle_Value;
		// ----------------------------
		Parameter angular_Resolution;	stream >> angular_Resolution; data.beam_Theta_0_Distribution.FWHM_distribution = angular_Resolution.value;
		// ----------------------------
		QString angle_Type; stream >> angle_Type;
		// ----------------------------

		stream  >> data.wavelength;

		// ----------------------------
		QVector<double> k_Vec;		stream >> k_Vec;
		QVector<double> lambda_Vec; stream >> lambda_Vec;
		double k_Value;				stream >> k_Value;
		double lambda_Value;		stream >> lambda_Value;
		// ----------------------------
		Parameter spectral_Resolution;	stream >> spectral_Resolution; data.spectral_Distribution.FWHM_distribution = spectral_Resolution.value;
		// ----------------------------
		Parameter polarization; stream >> polarization; data.polarization = polarization.value;
		// ----------------------------
		Parameter polarization_Sensitivity; stream >> polarization_Sensitivity; // not used
		// ----------------------------
		Parameter background; stream >> background; data.background = background.value;
		// ----------------------------
		Parameter beam_Size;  stream >> beam_Size; data.beam_Geometry.size = beam_Size.value;
		// ----------------------------
		Parameter beam_Profile_Spreading;  stream >> beam_Profile_Spreading; // not used
		// ----------------------------
		Parameter sample_Size;  stream >> sample_Size; data.sample_Geometry.size = sample_Size.value;
		// ----------------------------
		Parameter sample_Shift;  stream >> sample_Shift; data.sample_Geometry.x_Position = sample_Shift.value;
		// ----------------------------

		data.measurement_Type = measurement_Types[Specular_Scan];
		if(data.wavelength.independent.num_Points>1)		 {data.argument_Type = argument_Types[Wavelength_Energy]; } else
		if(data.beam_Theta_0_Angle.independent.num_Points>1) {data.argument_Type = argument_Types[Beam_Grazing_Angle];} else
															 {data.argument_Type = argument_Types[Beam_Grazing_Angle];}
	}
			// Ambient, Layer, Substrate
		stream	>> data.composed_Material >> data.material >> data.approved_Material >> data.absolute_Density >> data.relative_Density
				>> data.separate_Optical_Constants >> data.permittivity >> data.absorption >> data.composition;
			// Layer, Substrate
		if(!Global_Variables::check_Loaded_Version(1,11,0))
		{
			bool use_PSD;
			stream >> use_PSD;
		}
		stream >> data.common_Sigma_Diffuse >> data.sigma_Diffuse >> data.interlayer_Composition;
		if(Global_Variables::check_Loaded_Version(1,11,0))
		{stream >> data.roughness_Model;}

			// Layer
		stream  >> data.layer_Index >> data.has_Parent >> data.thickness >> data.thickness_Drift >> data.sigma_Diffuse_Drift
			// Multilayer
				>> data.first_Layer_Index >> data.last_Layer_Index >> data.num_Repetition >> data.period >> data.gamma;


	// Layer, Multilayer, Aperiodic
	if(Global_Variables::check_Loaded_Version(1,8,3))
	{stream >> data.step_Value_Change; }	// since 1.8.3

	if(Global_Variables::check_Loaded_Version(1,8,7) && !Global_Variables::check_Loaded_Version(1,8,11))
	{	bool use_Soft_Restrictions;
		int threshold;
		double Q_factor;
		stream >> use_Soft_Restrictions >> threshold >> Q_factor; }	// since 1.8.7

	if(Global_Variables::check_Loaded_Version(1,8,11))
	{stream >> data.regular_Components; }	// since 1.8.11

	return stream;
}
