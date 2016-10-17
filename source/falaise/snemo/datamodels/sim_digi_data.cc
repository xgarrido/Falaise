/// \file falaise/snemo/datamodels/sim_digi_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/sim_digi_data.h>

namespace snemo {

  namespace datamodel {

    // Serial tag :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_digi_data,
                                                      "snemo::datamodel::sim_digi_data")

    sim_digi_data::sim_digi_data()
    {
      return;
    }

    sim_digi_data::~sim_digi_data()
    {
      return;
    }

    bool sim_digi_data::is_valid() const
    {
      return true;
    }

    void sim_digi_data::reset()
    {
      _trigger_digi_data_.reset();
      _calo_digi_hits_.clear();
      _tracker_digi_hits_.clear();
      _auxiliaries_.clear();
      return;
    }

    const sim_trigger_digi_data & sim_digi_data::get_trigger_digi_data() const
    {
      return _trigger_digi_data_;
    }

    sim_trigger_digi_data & sim_digi_data::grab_trigger_digi_data()
    {
      return _trigger_digi_data_;
    }

    const sim_digi_data::calo_digi_hit_col & sim_digi_data::get_calo_digi_hits() const
    {
      return _calo_digi_hits_;
    }

    sim_digi_data::calo_digi_hit_col & sim_digi_data::grab_calo_digi_hits()
    {
      return _calo_digi_hits_;
    }

    const sim_digi_data::tracker_digi_hit_col & sim_digi_data::get_tracker_digi_hits() const
    {
      return _tracker_digi_hits_;
    }

    sim_digi_data::tracker_digi_hit_col & sim_digi_data::grab_tracker_digi_hits()
    {
      return _tracker_digi_hits_;
    }

    const datatools::properties & sim_digi_data::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    datatools::properties & sim_digi_data::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    void sim_digi_data::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
    {
      if(! title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      // Auxiliary properties:
      {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Auxiliary properties : ";
        if (_auxiliaries_.size() == 0) {
          out_ << "<empty>";
        }
        out_ << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent_;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _auxiliaries_.tree_dump(out_, "", indent_oss.str());
        }
      }

      // Trigger digitized data:
      {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Trigger digitized data : " << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent_;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _trigger_digi_data_.tree_dump(out_, "", indent_oss.str());
        }
      }

      // Calorimeter digitized hits:
      {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Calorimeter digitized hits : ";
        out_ << _calo_digi_hits_.size();
        out_ << std::endl;
      }

      // Tracker digitized hits:
      {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Tracker digitized hits : ";
        out_ << _tracker_digi_hits_.size();
        out_ << std::endl;
      }

      // Validity:
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
      out_ << "Valid: " << is_valid() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo