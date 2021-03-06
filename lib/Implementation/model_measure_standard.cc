#include <model_measure_standard.h>


using namespace FullPhysics;
using namespace blitz;


ModelMeasureStandard::ModelMeasureStandard(const boost::shared_ptr<ForwardModel>& forward_model, 
        const boost::shared_ptr<Observation>& observation, 
        const boost::shared_ptr<StateVector>& state_vector)
  : fm(forward_model), obs(observation), sv(state_vector),
    meas_units(obs->radiance_all().spectral_range().units())
{}


void ModelMeasureStandard::model_eval()
{
  if(M.size() <= 0)
    radiance_from_fm(true);
}


void ModelMeasureStandard::jacobian_eval()
{
  if(K.size() <= 0)
    radiance_from_fm(false);
}


void ModelMeasureStandard::model_jacobian_eval()
{
  if((K.size() <= 0) or (M.size() <= 0))
    radiance_from_fm(false);
}

int ModelMeasureStandard::expected_parameter_size() const
{
  return sv->observer_claimed_size();
}


void ModelMeasureStandard::radiance_from_fm(bool Skip_jacobian)
{
  assert_parameter_set_correctly();
  sv->update_state(X);

  //  TEMPORARY
  //
  //  There should be no need for this error check block.
  //  The problem is that with our current model code 
  //  measurement may change.
  //
  Array<double, 1> temp_msrmnt(obs->radiance_all().spectral_range().data());
  if(msrmnt.rows() != temp_msrmnt.rows())
    throw Exception("Measurement has changed during the retrieval. :( ");
  double msrmnt_L1_norm = sum(abs(msrmnt));
  if(msrmnt_L1_norm <= 0.0)
    throw Exception("Measurement has no signal (just 0). :( ");
  if( sum(abs(temp_msrmnt-msrmnt))/msrmnt_L1_norm > 0.0000001 )
    throw Exception("Measurement has changed during the retrieval. :( ");

  Spectrum rad_spec = fm->radiance_all(Skip_jacobian);
  SpectralRange rad_mod = rad_spec.spectral_range().convert(meas_units);
  M.reference(rad_mod.data_ad().value());
  assert_model_correct(M);
  M.makeUnique();
  if(!Skip_jacobian) {
    K.reference(rad_mod.data_ad().jacobian());
    assert_jacobian_correct(K);

    //  TEMPORARY
    //
    // Should go away after we end support for 
    // fixed pressure level grid.
    vanishing_params_update();

    K.makeUnique();
  }
}




//  TEMPORARY
//
// Should go away after we end support for 
// fixed pressure level grid.
void ModelMeasureStandard::vanishing_params_update()
{
  if(K.size() <= 0) return;

  //  Even with fixed-pressure-level the following 
  //  code block should not be necessary if the
  //  forward-model class is already setting to zero
  //  the columns of the Jacobian associated with
  //  the unused parameters (if any).
  //
  Array<bool, 1> used(sv->used_flag());
  if(used.rows() != K.cols())
    throw Exception("Columns of Jacobian and elements of used-flag not equal in numbers! :( ");
  for(int i=0; i<used.rows(); i++)
    if(!used(i)) K(Range::all(),i) = 0.0;
}


void ModelMeasureStandard::parameters(const blitz::Array<double, 1>& x)
{
  //  This check is needed because it is not obvious 
  //  whether or not the Forward Model does caching.
  //
  if(!parameters_different(x)) return;
  ModelMeasure::parameters(x);
  sv->update_state(x);
}
