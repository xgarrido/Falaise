/// \file falaise/datamodels/sim_digi_data.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_IPP
#define FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_IPP

// Ourselves:
#include <falaise/snemo/datamodels/sim_digi_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.ipp>
#include <bayeux/datatools/properties.ipp>

// This project:
#include <falaise/snemo/datamodels/sim_digi_event_data.ipp>

namespace snemo {

namespace datamodel {

template <class Archive>
void sim_digi_data::serialize(Archive& ar, const unsigned int /* version */) {
  ar& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar& boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
  ar& boost::serialization::make_nvp("digi_events", _digi_events_);
  return;
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
