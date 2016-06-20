// hc_rate_analysis.cxx
// Standard libraries :
#include <iostream>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>
#include <bayeux/geomtools/id_mgr.h>
#include <bayeux/geomtools/id_selector.h>

// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>

// Falaise:
#include <falaise/falaise.h>

// Third part : 
// Root : 
#include "TFile.h"
#include "TTree.h"


int column_to_hc_half_zone(const int & column);


int main( int  argc_ , char **argv_  )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  // Parsing arguments
  int iarg = 1;
  bool is_input_file   = false;
  bool is_event_number = false;
  bool is_output_path  = false;
  bool is_display      = false;
  bool is_help         = false;  
  
  std::string input_filename;
  std::string output_path;
  int arg_event_number  = -1;

  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") 
      {
	is_input_file = true;
	input_filename = argv_[++iarg];
      } 

    else if (arg == "-op" || arg == "--output-path")
      {
	is_output_path = true;
	output_path = argv_[++iarg];	
      }
    
    else if (arg == "-n" || arg == "--number")
      {
        is_event_number = true;
	arg_event_number    = atoi(argv_[++iarg]);
      }

    else if (arg == "-d" || arg == "--display")
      {
	is_display = true;
      }  

    else if (arg =="-h" || arg == "--help")
      {
	is_help = true;
      }

    iarg++;
  }

  if (is_help) 
    { 
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
		<< "$ BuildProducts/bin/half_commissioning-hc_rate_analysis [OPTIONS] [ARGUMENTS]" << std::endl << std::endl
		<< "Allowed options: " << std::endl
		<< "-h  [ --help ]           produce help message" << std::endl
		<< "-i  [ --input ]          set an input file" << std::endl
		<< "-op [ --output path ]    set a path where all files are stored" << std::endl
		<< "-d  [ --display ]        display things for debug" << std::endl
		<< "-n  [ --number ]         set the number of events" << std::endl
		<< "Example : " << std::endl << std::endl;
	return 0;
    }

  try {
    // boolean for debugging (display etc)
    bool debug = false;

    if (is_display) debug = true;   

    std::clog << "Program for half commissioning rate analysis" << std::endl;
    
    std::string manager_config_file;
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/4.0/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_geom_manager;
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories"))
      {
	manager_config.erase ("mapping.excluded_categories");
      }
    my_geom_manager.initialize (manager_config);

    std::string pipeline_simulated_data_filename;
    
    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";
    datatools::fetch_path_with_env(input_filename);
    if(is_input_file){
      pipeline_simulated_data_filename = input_filename;
    }else{
      pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Co60-100_row_0_column_0_SD.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    // Number of events :
    int event_number = -1;
    if (is_event_number) event_number = arg_event_number;
    else                 event_number = 10;

    // Vertex in the filename :
    std::string vtx_filename = pipeline_simulated_data_filename;
    std::size_t found = 0;
    found = vtx_filename.find_last_of("/");
    vtx_filename.erase(0, found+1);
    found = vtx_filename.find(".brio");
    vtx_filename.erase(found, vtx_filename.size());
    std::clog << "vertex = " << vtx_filename << std::endl;

    // Found row i and column j :
    std::string temporary_filename = vtx_filename;
    std::size_t found_1 = 0;
    int row = 0;
    int column = 0;
    // Find the row :
    found_1 = temporary_filename.find("row_");
    temporary_filename.erase(0, found_1 + 4);
    found_1 = temporary_filename.find_first_of("_");
    temporary_filename.erase(found_1, temporary_filename.size());
    row = std::stoi(temporary_filename);

    // Find the column :
    temporary_filename = vtx_filename;
    found_1 = temporary_filename.find("column_");
    temporary_filename.erase(0, found_1 + 7);
    found_1 = temporary_filename.find_first_of("_");
    temporary_filename.erase(found_1, temporary_filename.size());
    column = std::stoi(temporary_filename);
    
    // Check if a column count for 2 half zones :
    int process_size = 1;
    bool event_for_two_zones = false;
    if ((column >= 3 && column <= 5) || column == 62 || (column >= 107 && column <= 109))
      {
	process_size = 2;
	event_for_two_zones = true;
      }

    for (int i = 0; i < process_size; i++)
      {   
	// Event reader :
	dpp::input_module reader;
	datatools::properties reader_config;
	reader_config.store ("logging.priority", "debug");
	reader_config.store ("max_record_total", event_number);
	reader_config.store ("files.mode", "single");
	reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
	reader.initialize_standalone (reader_config);
	if (debug) reader.tree_dump(std::clog, "Simulated data reader module");

	// Event record :
	datatools::things ER;
    
        // Output path :
	datatools::fetch_path_with_env(output_path);
	if (is_output_path) output_path = output_path;
	else output_path = "${FALAISE_DIGITIZATION_TESTING_DIR}/output_default/";
	datatools::fetch_path_with_env(output_path);

	// Name of SD output files
	std::string HC_writer_1 = output_path + "HC_writer" + ".brio";

	// Event writer : 
	dpp::output_module writer_1;
	datatools::properties writer_config_1;
	writer_config_1.store ("logging.priority", "debug");
	writer_config_1.store ("files.mode", "single");   
	writer_config_1.store ("files.single.filename", HC_writer_1);
	writer_1.initialize_standalone(writer_config_1); 

	// Output ROOT file : 
	std::string root_filename = output_path + "HC_" + vtx_filename + "_analysis.root";
	datatools::fetch_path_with_env(root_filename);
	TFile* root_file = new TFile(root_filename.c_str(), "RECREATE");
	TTree* hc_analysis_tree = new TTree("HC_analysis_tree", "Half Commissioning analysis tree");
      
	// Internal counters
	int psd_count = 0; // Event counter
	
	// Selection rules depending on half zone for commissioning :

	int hc_half_zone = column_to_hc_half_zone(column) + i;
	std::string hc_main_calo_half_zone_rules  = " ";
	std::string hc_xwall_calo_half_zone_rules = " ";
	std::string hc_geiger_half_zone_rules     = " ";
	int geiger_hc_zone_inf_limit = 0;
	int geiger_hc_zone_sup_limit = 0;
	int main_calo_column = 0;

	main_calo_column = hc_half_zone;
	if (hc_half_zone == 0)
	  {
	    geiger_hc_zone_inf_limit = 0;
	    geiger_hc_zone_sup_limit = 5;
	  }
	else if (hc_half_zone >= 1 && hc_half_zone <= 10)
	  {	   
	    geiger_hc_zone_inf_limit = hc_half_zone * 6 - 3;
	    geiger_hc_zone_sup_limit = geiger_hc_zone_inf_limit + 5;
	  }
	else if (hc_half_zone >= 11 && hc_half_zone <= 18)
	  {
	    geiger_hc_zone_inf_limit = hc_half_zone * 6 - 4;
	    geiger_hc_zone_sup_limit = geiger_hc_zone_inf_limit + 5;
	  }
	else if (hc_half_zone == 19)
	  {		    
	    geiger_hc_zone_inf_limit = 107;
	    geiger_hc_zone_sup_limit = 112;
	  }

	std::clog << "borne inf gg = " << geiger_hc_zone_inf_limit << " borne sup gg = " << geiger_hc_zone_sup_limit << std::endl;
	hc_main_calo_half_zone_rules = "category='calorimeter_block' module={0} side={1} column={" + std::to_string(hc_half_zone) + "} row={*} part={*}";
		
	//hc_xwall_calo_half_zone_rules = "category='calorimeter_block' module={0} side={1} column={8} row={*} part={*}";
	hc_geiger_half_zone_rules = "category='drift_cell_core' module={0} side={1} layer={*} row={"
	  + std::to_string(geiger_hc_zone_inf_limit) + ";" 
	  + std::to_string(geiger_hc_zone_inf_limit + 1) + ";" 
	  + std::to_string(geiger_hc_zone_inf_limit + 2) + ";" 
	  + std::to_string(geiger_hc_zone_inf_limit + 3) + ";" 
	  + std::to_string(geiger_hc_zone_inf_limit + 4) + ";" 
	  + std::to_string(geiger_hc_zone_sup_limit) + "}";
	    
	while (!reader.is_terminated())
	  {
	    if(is_display) std::clog <<  "********************************************************************************" << std::endl;
	    if(is_display) std::clog <<  "****************************** EVENT #" << psd_count << " **************************************" << std::endl;
	    reader.process(ER);
	    
	    // A plain `mctools::simulated_data' object is stored here :
	    if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	      {
		// Access to the "SD" bank with a stored `mctools::simulated_data' :
		const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);

		// If main calo hits :
		if (SD.has_step_hits("calo"))
		  {
		    const size_t number_of_main_calo_hits = SD.get_number_of_step_hits("calo");
		    // Rules have to change depending on row / column input
		    geomtools::id_selector my_hc_main_calo_id_selector(my_geom_manager.get_id_mgr());
		    my_hc_main_calo_id_selector.initialize(hc_main_calo_half_zone_rules);
		    if (is_display) my_hc_main_calo_id_selector.dump(std::clog, "Main calo ID selector: ");

		    mctools::simulated_data::hit_handle_collection_type BSHC = SD.get_step_hits("calo");
		    if (is_display) std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
		    int count = 0;
		    for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
			 i != BSHC.end();
			 i++) 
		      {
			const mctools::base_step_hit & BSH = i->get();
			// extract the corresponding geom ID:
			const geomtools::geom_id & main_calo_gid = BSH.get_geom_id();
			if (my_hc_main_calo_id_selector.match(main_calo_gid))
			  {
			    if (is_display) std::clog << "ID=" << main_calo_gid << " matches the selector rules !" << std::endl;
			    if (is_display) BSH.tree_dump(std::clog, "A Main calo Base Step Hit : ", "INFO : ");
			  }
			else
			  {
			    if (is_display) std::clog << "ID=" << main_calo_gid << " does not match the selector rules !" << std::endl;
			  }
		      }

		  } // end of if has step hits calo

		// If xcalo hits :
		if (SD.has_step_hits("xcalo"))
		  {
		    const size_t number_of_xcalo_hits = SD.get_number_of_step_hits("xcalo");
		
		
		  } // end of if has step hits xcalo
	    
		// If Geiger hits :
		if (SD.has_step_hits("gg"))
		  {
		    const size_t number_of_hits = SD.get_number_of_step_hits("gg");
		    std::clog << "Has steps hits geiger : " << SD.has_step_hits("gg") << std::endl;
		    // Rules have to change depending on row / column input
		    geomtools::id_selector my_hc_geiger_id_selector(my_geom_manager.get_id_mgr());
		    my_hc_geiger_id_selector.initialize(hc_geiger_half_zone_rules);

		    if (is_display) my_hc_geiger_id_selector.dump(std::clog, "Geiger ID selector: ");

		    // New sd bank
		    mctools::simulated_data flaged_sd = SD;
	
		    // We have to flag the gg cells already hit before (maybe take into account the dead time of a GG cell)
		    for (size_t ihit = 0; ihit < number_of_hits; ihit++)
		      {
			mctools::base_step_hit & geiger_hit = flaged_sd.grab_step_hit("gg", ihit);	    
			for (size_t jhit = ihit + 1; jhit < number_of_hits; jhit++)
			  {
			    mctools::base_step_hit & other_geiger_hit = flaged_sd.grab_step_hit("gg", jhit);
			    if (geiger_hit.get_geom_id() == other_geiger_hit.get_geom_id())
			      {
				const double gg_hit_time       = geiger_hit.get_time_start();
				const double other_gg_hit_time = other_geiger_hit.get_time_start();
				if (gg_hit_time > other_gg_hit_time) 
				  {
				    bool geiger_already_hit = true;
				    if (!geiger_hit.get_auxiliaries().has_flag("geiger_already_hit")) geiger_hit.grab_auxiliaries().store("geiger_already_hit", geiger_already_hit);
				  }
				else 
				  {
				    bool geiger_already_hit = true;
				    if (!other_geiger_hit.get_auxiliaries().has_flag("geiger_already_hit")) other_geiger_hit.grab_auxiliaries().store("geiger_already_hit", geiger_already_hit);
				  }
			      } // end of if get_geom_id
			  } // end of jhit
		      } // end of ihit
		    mctools::simulated_data::hit_handle_collection_type BSHC = flaged_sd.get_step_hits("gg");
		    if (is_display) std::clog << "BSCH step hits # = " << BSHC.size() << std::endl;
		    int count = 0;
		    for (mctools::simulated_data::hit_handle_collection_type::const_iterator i = BSHC.begin();
			 i != BSHC.end();
			 i++) 
		      {
			const mctools::base_step_hit & BSH = i->get();
			if (is_display) BSH.tree_dump(std::clog, "A Geiger Base Step Hit : ", "INFO : ");
			if (BSH.get_auxiliaries().has_flag("geiger_already_hit") || BSH.get_auxiliaries().has_flag("other_geiger_already_hit")) {}
			else
			  {
			    // extract the corresponding geom ID:
			    const geomtools::geom_id & geiger_gid = BSH.get_geom_id();

			    if (my_hc_geiger_id_selector.match(geiger_gid))
			      {
				if (is_display) std::clog << "ID=" << geiger_gid << " matches the selector rules !" << std::endl;
			      }
			    else
			      {
				if (is_display) std::clog << "ID=" << geiger_gid << " does not match the selector rules !" << std::endl;
			      }
			
			    std::clog << "geiger gid = " << geiger_gid << std::endl;
			    int hit_id = count;
			    double time_start = BSH.get_time_start();
			    geomtools::vector_3d position_start_vector = BSH.get_position_start();
			    geomtools::vector_3d position_stop_vector  = BSH.get_position_stop();
			    geomtools::vector_3d momentum_start_vector = BSH.get_momentum_start();
			
			    count++;

			  }
		      } // end of for 
		  } // end of if has step hits "gg"

	      } // end of ER
	    ER.clear();
	    psd_count++;
	    if (is_display) std::clog << "DEBUG : psd count " << psd_count << std::endl;
	  } // end of while reader
	
      } // end of for i = process size 
    

    std::clog << "The end." << std::endl;
  } // end of try

  catch (std::exception & error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  }

  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }

  falaise::terminate();
  return error_code;
}


int column_to_hc_half_zone(const int & column)
{
  int hc_zone = -1;
  if (column >= 0 && column <= 5)
    {
      hc_zone = 0;
    }
  
  else if (column >= 6 && column <= 62)
    {
      hc_zone = (column + 3) / 6;
    }
  
  else if (column >= 63 && column <= 106)
    {
      hc_zone = (column + 4) / 6;
    }
  
  else if (column >= 107 && column <= 112)
    {
      hc_zone = 19;
    }
  
  return hc_zone;
}
