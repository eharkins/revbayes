#include "Model.h"

#include <map>
#include <ostream>
#include <string>

#include "DagNode.h"
#include "RbException.h"
#include "StringUtilities.h"

using namespace RevBayesCore;


/**
 * Constructor from a single DAG node.
 * The model graph is extracted by obtaining all DAG nodes connected to the provide source node.
 * The entire model graph is copied and a map between the pointers to the original DAG nodes and
 * the copied DAG nodes is created for convenient access.
 *
 * \param[in]    source    The DAG node from which the model graph is extracted.
 */
Model::Model(const DagNode *source) : Parallelizable()
{
    
    // add this node to the source nodes and build model graph
    addSourceNode( source );
    
}


/**
 * Constructor from a set of DAG nodes.
 * The model graph is extracted by obtaining all DAG nodes that are connected to either of the provide source nodes.
 * The entire model graph is copied and a map between the pointers to the original DAG nodes and
 * the copied DAG nodes is created for convenient access.
 *
 * \param[in]    s    The set of DAG nodes from which the model graph is extracted.
 */
Model::Model(const std::set<const DagNode*> &s) : Parallelizable(),
    sources() 
{
    
    // iterate over all sources
    for (std::set<const DagNode*>::const_iterator it = s.begin(); it != s.end(); ++it) 
    {
        // add this node and build model graph
        addSourceNode( *it );
    }
    
}


/**
 * Copy constructor. We instantiate the model from the previously stored source nodes. 
 *
 * \param[in]    m    The model object to copy.
 */
Model::Model(const Model &m) : Parallelizable(m),
    sources()
{
    
    // iterate over all sources
    for (std::vector<const DagNode*>::const_iterator it = m.sources.begin(); it != m.sources.end(); ++it)
    {
        // add this node and build model graph
        addSourceNode( *it );
    }
    
}

/**
 * Destructor.
 * We have created new copied of the DAG nodes so we need to delete these here again.
 */
Model::~Model( void ) 
{
    
    // delete each DAG node from the copied model graph.
    for (std::vector<DagNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it) 
    {
        DagNode *the_node = *it;
        if ( the_node->decrementReferenceCount() == 0 )
        {
            delete the_node;
        }
    }
    
    while ( sources.empty() == false )
    {
        std::vector<const DagNode*>::iterator it = sources.begin();
        const DagNode *the_node = *it;
        sources.erase( it );
        
        if ( the_node->decrementReferenceCount() == 0)
        {
            delete the_node;
        }
        
    }
    
}


/**
 * Assignment operator.
 * We have stored previously all the source nodes so we can simply construct the model graph by extracting
 * the DAG again from these source nodes. Note that we are acutally making a copy of the original graph again
 * which may cause that we get a different model graph (e.g. if the graph has change in the meanwhile).
 * The reason behind this approach is that we keep a valid map from the pointers of the original DAG nodes to
 * the pointers to the copied DAG nodes.
 *
 * \todo Check that this copy constructor is good or if we should use a different mechanism for the map between the
 * original DAG nodes and the copied DAG nodes.
 *
 * \param[in]    x    The model object to copy.
 */
Model& Model::operator=(const Model &x) 
{
    Parallelizable::operator=(x);
    
    if ( this != &x )
    {
        // first remove all DAG nodes
        for (std::vector<DagNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it) 
        {
            DagNode *the_node = *it;
            if ( the_node->decrementReferenceCount() == 0 )
            {
                delete the_node;
            }
        }
        
        // empty the source nodes
        while ( sources.empty() == false )
        {
            std::vector<const DagNode*>::iterator it = sources.begin();
            const DagNode *the_node = *it;
            sources.erase( it );
            
            if ( the_node->decrementReferenceCount() == 0)
            {
                delete the_node;
            }
        }
        
        // iterate over all sources
        for (std::vector<const DagNode*>::const_iterator it = x.sources.begin(); it != x.sources.end(); ++it)
        {
            // add this node and build model graph
            addSourceNode( *it );
        }
    }
    
    return *this;
}



/**
 * Add a new source node.
 * We extract the model graph from the source node by calling cloneDAG on it.
 * cloneDAG clones the entire connected graph containing the given node.
 * Then we insert the copied source node in our set of source nodes so that we can use it
 * later to construct the model graph again in the copy constructor.
 * At the same time we fill the nodes map between the pointers of the nodes to the original DAG
 * and the the pointer to the cloned DAG nodes and fill also the vector of DAG nodes contained 
 * in this model.
 * 
 *
 * \param[in]    sourceNode    The new source node.
 */
void Model::addSourceNode(const DagNode *sourceNode) 
{
    
    // check that the source node is a valid pointer
    if (sourceNode == NULL)
    {
        throw RbException("Cannot instantiate a model with a NULL DAG node.");
    }
    
    
    // copy the entire graph connected to the source node
    // only if the node is not contained already in the nodesMap will it be copied.
    std::map<std::string, const DagNode* > names;
    sourceNode->cloneDAG(nodesMap, names);
    
    
    // add the source node to our set of sources
    DagNode *theNewSource = nodesMap[sourceNode];
    theNewSource->incrementReferenceCount();
    sources.push_back( theNewSource );
    
    
    // we don't really know which nodes are new in our nodes map.
    // therefore we empty the nodes map and fill it again.
    for (std::vector<DagNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it) 
    {
        
        DagNode *the_node = *it;
        the_node->decrementReferenceCount();
        
    }
    nodes.clear();
    
    // insert new nodes into direct access vector
    std::vector< DagNode* >::iterator i = nodesMap.begin();
    
    while ( i != nodesMap.end() )
    {
        // get the copied node
        DagNode* theNewNode = (*i);
        
        // increment the reference count to the new node
        theNewNode->incrementReferenceCount();
            
        // insert in direct access vector
        nodes.push_back( theNewNode );

        // increment the iterator;
        ++i;
    }
    
}

/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'B'.
 *
 * \return A new copy of myself 
 */
Model* Model::clone( void ) const 
{
    
    return new Model( *this );
}


/**
 * Constant getter function for the vector of DAG nodes of the model.
 *
 * \return Vector of DAG nodes constituting to this model.
 */
const std::vector<DagNode *>& Model::getDagNodes( void ) const 
{
    
    return nodes;
}


/**
 * Non-constant getter function for the vector of DAG nodes of the model.
 *
 * \return Vector of DAG nodes constituting to this model.
 */
std::vector<DagNode *>& Model::getDagNodes( void ) 
{
    
    return nodes;
}


/**
 * Constant getter function for the map between the pointers of the original DAG nodels
 * to the copied DAG nodes of the model.
 *
 * \return Map between pointers from original to copied DAG nodes.
 */
const DagNodeMap& Model::getNodesMap( void ) const
{
    
    return nodesMap;
}



std::vector<DagNode*> Model::getOrderedStochasticNodes( void )
{
    
    std::vector<DagNode *> ordered_nodes;
    std::set< const DagNode *> visited;
    getOrderedStochasticNodes(nodes[0], ordered_nodes, visited );
    
    return ordered_nodes;
}

/**
 * Creates a vector of stochastic nodes, starting from the source nodes to the sink nodes
 */
void Model::getOrderedStochasticNodes(const DagNode* dagNode,  std::vector<DagNode*>& orderedStochasticNodes, std::set<const DagNode*>& visitedNodes)
{
    
    if (visitedNodes.find(dagNode) != visitedNodes.end())
    {
        //The node has been visited before
        //we do nothing
        return;
    }
    
    // add myself here for safety reasons
    visitedNodes.insert( dagNode );
    
    if ( dagNode->isConstant() == false )
    {
        // First I have to visit my parents
        std::vector<const DagNode *> parents = dagNode->getParents() ;
        std::vector<const DagNode *>::const_iterator it;
        for ( it=parents.begin() ; it != parents.end(); it++ )
        {
            getOrderedStochasticNodes(*it, orderedStochasticNodes, visitedNodes);
        }
        
    }

    // Then I can add myself to the nodes visited, and to the ordered vector of stochastic nodes
    if ( dagNode->isStochastic() ) //if the node is stochastic
    {
        orderedStochasticNodes.push_back( const_cast<DagNode*>( dagNode ) );
    }

    // Finally I will visit my children
    std::vector<DagNode*> children = dagNode->getChildren() ;
    std::vector<DagNode*>::iterator it;
    for ( it = children.begin() ; it != children.end(); it++ )
    {
        getOrderedStochasticNodes(*it, orderedStochasticNodes, visitedNodes);
    }
    
}


/**
 * Set the active PID of this specific model object.
 */
void Model::setActivePIDSpecialized(size_t a, size_t n)
{
    
    // delegate the call to each DAG node
    for (std::vector<DagNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        
        DagNode *the_node = *it;
        the_node->setActivePID(a,n);
        
    }
    
}


std::ostream& RevBayesCore::operator<<(std::ostream& o, const Model& m)
{
    
    const std::vector<RevBayesCore::DagNode*>& the_nodes = m.getDagNodes();
    std::vector<RevBayesCore::DagNode*>::const_iterator it;
    
    o << std::endl;
    std::stringstream s;
    
    // compute the number of nodes by only counting nodes that are not hidden
    size_t num_nodes = 0;
    for ( it=the_nodes.begin(); it!=the_nodes.end(); ++it )
    {
        
        if ( (*it)->isHidden() == false )
        {
            ++num_nodes;
        }
        
    }
    
    s << "Model with " << num_nodes << " nodes";
    o << s.str() << std::endl;
    for ( size_t i = 0; i < s.str().size(); ++i )
        o << "=";
    o << std::endl << std::endl;
    
    for ( it=the_nodes.begin(); it!=the_nodes.end(); ++it )
    {
        RevBayesCore::DagNode *the_node = *it;
        // skip hidden nodes
        if ( the_node->isHidden() == true )
        {
            continue;
        }
        
        if ( the_node->getName() != "" )
        {
            o << the_node->getName() <<  " :" << std::endl;
        }
        else
        {
            o << "<" << the_node << "> :" << std::endl;
        }
        
        o << "_value        = ";
        std::ostringstream o1;
        the_node->printValue( o1, ", ", true );
        o << StringUtilities::oneLiner( o1.str(), 54 ) << std::endl;
        
        the_node->printStructureInfo( o, false );
        
        o << std::endl;
    }
    
    return o;
}
