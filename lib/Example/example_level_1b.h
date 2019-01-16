#ifndef EXAMPLE_L1B_H
#define EXAMPLE_L1B_H

#include "level_1b_sample_coefficient.h"
#include "hdf_file.h"
#include "unit.h"
#include "double_with_unit.h"
#include "example_observation_id.h"

namespace FullPhysics {

/****************************************************************//**
  This is an example L1B reader that reads a HDF formatted file
  that corresponds one-to-one with the expected interface.
*******************************************************************/

class ExampleLevel1b: public Level1bSampleCoefficient {
public:
    ExampleLevel1b(const boost::shared_ptr<HdfFile>& input_file, const std::string& observation_id);
    ExampleLevel1b(const std::string& input_filename, const std::string& observation_id);
    ExampleLevel1b(const boost::shared_ptr<HdfFile>& input_file, ExampleObservationId<std::string> observation_id);
    ExampleLevel1b(const std::string& input_filename, ExampleObservationId<std::string> observation_id);

    virtual ~ExampleLevel1b() {}

    virtual int number_spectrometer() const;

    virtual DoubleWithUnit latitude(int i) const
        { return read_scalar_with_unit(group_name + "/latitude", i, units::deg); }

    virtual DoubleWithUnit longitude(int i) const
        { return read_scalar_with_unit(group_name + "/longitude", i, units::deg); }

    virtual DoubleWithUnit solar_zenith(int i) const
        { return read_scalar_with_unit(group_name + "/solar_zenith", i, units::deg); }

    virtual DoubleWithUnit solar_azimuth(int i) const
        { return read_scalar_with_unit(group_name + "/solar_azimuth", i, units::deg); }

    virtual DoubleWithUnit altitude(int i) const
        { return read_scalar_with_unit(group_name + "/altitude", i, units::deg); }

    virtual DoubleWithUnit sounding_zenith(int i) const
        { return read_scalar_with_unit(group_name + "/observation_zenith", i, units::deg); }

    virtual DoubleWithUnit sounding_azimuth(int i) const
        { return read_scalar_with_unit(group_name + "/observation_azimuth", i, units::deg); }

    virtual blitz::Array<double, 1> stokes_coefficient(int i) const
        { return read_array(group_name + "/stokes_coefficient", i); }

    virtual DoubleWithUnit relative_velocity(int i) const 
        { return read_scalar_with_unit(group_name + "/relative_velocity", i, units::deg); }

    virtual ArrayWithUnit<double, 1> spectral_coefficient(int i) const
        { return read_array_with_unit(group_name + "/spectral_coefficient", i, units::micron); }

    virtual Time time(int i) const
        { return Time::time_pgs(read_scalar(group_name + "/time_tai93", i)); }

    virtual void print(std::ostream& Os) const { Os << "ExampleL1b"; };

    virtual SpectralRange radiance(int Spec_index) const;

    static std::vector<ExampleObservationId<std::string>> obs_list(const std::string& geo_input_filename);

    static std::vector<ExampleObservationId<std::string>> closest_obs_n(const std::string& geo_input_filename, double lat, double lon, int n);

    static double obs_distance(const std::string& geo_input_filename, ExampleObservationId<std::string> obs_id, double lat, double lon);


private:
    DoubleWithUnit read_scalar_with_unit(const std::string& dataset_name, int i, const Unit& default_unit) const;
    double read_scalar(const std::string& dataset_name, int i) const;

    ArrayWithUnit<double, 1> read_array_with_unit(const std::string& dataset_name, int i, const Unit& default_unit) const;
    blitz::Array<double, 1> read_array(const std::string& dataset_name, int i) const;

    static bool compare_obs(ExampleObservationId<std::string> obs1, ExampleObservationId<std::string> obs2, const std::string& geo_input_filename, double lat, double lon);

    boost::shared_ptr<HdfFile> input;
    int data_index;

    const std::string group_name = "Level1b";
};
}
#endif
