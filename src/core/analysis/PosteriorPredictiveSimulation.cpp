#include "DagNode.h"
#include "HomologousDiscreteCharacterData.h"
#include "PosteriorPredictiveSimulation.h"
#include "StateDependentSpeciationExtinctionProcess.h"
#include "RbFileManager.h"

#include <cmath>
#include <typeinfo>

using namespace RevBayesCore;

PosteriorPredictiveSimulation::PosteriorPredictiveSimulation( const Model &m, const std::string &dir, const RbVector<ModelTrace> &t) : Cloneable(), Parallelizable(),
    model( m ),
    directory( dir ),
    traces( t ),
    condition_on_tips( false )
{
    
}


PosteriorPredictiveSimulation::PosteriorPredictiveSimulation( const Model &m, const std::string &dir, const RbVector<ModelTrace> &t, const std::vector<RevBayesCore::AncestralStateTrace> &ast) : Cloneable(), Parallelizable(),
    model( m ),
    directory( dir ),
    traces( t ),
    ancestral_state_traces( ast ),
    condition_on_tips( true )
{
    
}



PosteriorPredictiveSimulation::~PosteriorPredictiveSimulation()
{
    
}



/** Clone function */
PosteriorPredictiveSimulation* PosteriorPredictiveSimulation::clone() const
{
    
    return new PosteriorPredictiveSimulation(*this);
}


void RevBayesCore::PosteriorPredictiveSimulation::run( int thinning )
{
    
    // some general constant variables
    RbFileManager fm = RbFileManager( directory );
    const std::string path_separator = fm.getPathSeparator();
    
    // this is where we need to implement the posterior predictive simulation
    
    size_t n_samples = traces[0].size();
    size_t n_traces = traces.size();
    
    std::vector<DagNode*> nodes = model.getDagNodes();
    
    size_t sim_pid_start = size_t(floor( (double(pid) / num_processes * n_samples ) ) );
    size_t sim_pid_end   = std::max( int(sim_pid_start), int(floor( (double(pid+1) / num_processes * n_samples ) ) - 1) );
    
    size_t index_sample = sim_pid_start;
    size_t current_pp_sim = sim_pid_start;
    for ( ; index_sample <= sim_pid_end; ++current_pp_sim, index_sample += thinning)
    {
        
        // create a new directory name for this simulation
        std::stringstream s;
        s << directory << path_separator << "posterior_predictive_sim_" << (current_pp_sim + 1);
        std::string sim_directory_name = s.str();
        
        // now for the numerical parameters
        for ( size_t j=0; j<n_traces; ++j )
        {
            std::string parameter_name = traces[j].getParameterName();
            
            // iterate over all DAG nodes (variables)
            for ( std::vector<DagNode*>::iterator it = nodes.begin(); it!=nodes.end(); ++it )
            {
                DagNode *the_node = *it;
                
                if ( the_node->getName() == parameter_name )
                {
                    // set the value for the variable with the i-th sample
                    the_node->setValueFromString( traces[j].objectAt( current_pp_sim ) );
                }
            
            }
        
        }
        
        // build a map for the ancestral state trace labels -> tip indices
        std::map<std::string, size_t> ancestral_state_traces_lookup;
        if (condition_on_tips == true)
        {
            for (size_t z = 0; z < ancestral_state_traces.size(); z++)
            {
                ancestral_state_traces_lookup[ ancestral_state_traces[z].getParameterName() ] = z;
            }
        }

        // next we need to simulate the data and store it
        // iterate over all DAG nodes (variables)
        for ( std::vector<DagNode*>::iterator it = nodes.begin(); it!=nodes.end(); ++it )
        {
            DagNode *the_node = *it;
            
            if ( the_node->isClamped() == true )
            {
                // check if the PP simulation must condition on sampled tip states
                if (condition_on_tips == true && typeid(the_node->getDistribution()) == typeid(StateDependentSpeciationExtinctionProcess))
                {
                    // set the tip states to the values sampled during this iteration
                    AncestralStateTrace* tip_state_trace;
                    StateDependentSpeciationExtinctionProcess* sse = static_cast<StateDependentSpeciationExtinctionProcess*>( &the_node->getDistribution() );
                    std::vector<std::string> tips = sse->getValue().getTipNames();
                    size_t num_states = static_cast<TreeDiscreteCharacterData*>( &sse->getValue() )->getCharacterData().getNumberOfStates();
                    HomologousDiscreteCharacterData<NaturalNumbersState> *tip_data = new HomologousDiscreteCharacterData<NaturalNumbersState>();

                    // read the ancestral state trace
                    for (size_t tip_index = 0; tip_index < tips.size(); ++tip_index)
                    {
                        std::string tip_index_anc_str = StringUtilities::toString(tip_index + 1);
                        std::string tip_index_end_str = "end_" + StringUtilities::toString(tip_index + 1);
                        
                        if (ancestral_state_traces_lookup.find(tip_index_anc_str) != ancestral_state_traces_lookup.end())
                        {
                            size_t idx = ancestral_state_traces_lookup[tip_index_anc_str];
                            tip_state_trace = &ancestral_state_traces[idx];
                        }
                        else if (ancestral_state_traces_lookup.find(tip_index_end_str) != ancestral_state_traces_lookup.end())
                        {
                            size_t idx = ancestral_state_traces_lookup[tip_index_end_str];
                            tip_state_trace = &ancestral_state_traces[idx];
                        }
                        const std::vector<std::string>& tip_state_vector = tip_state_trace->getValues();
                        std::string state_str = tip_state_vector[current_pp_sim];
                       
                        // create a taxon data object for each tip
                        DiscreteTaxonData<NaturalNumbersState> this_tip_data = DiscreteTaxonData<NaturalNumbersState>(tips[tip_index]);
                        NaturalNumbersState state = NaturalNumbersState(0, num_states);
                        state.setState(state_str);
                        this_tip_data.addCharacter(state);
                        tip_data->addTaxonData(this_tip_data);
                    }
                   
                    // finally set the tip data to the sampled values
                    static_cast<TreeDiscreteCharacterData*>( &sse->getValue() )->setCharacterData(tip_data);
                }
                else
                {
                    // redraw new values
                    the_node->redraw();
                }

                // we need to store the new simulated data
                the_node->writeToFile(sim_directory_name);
                
            }
            
        }
        
    } // end for over all samples
    
}
