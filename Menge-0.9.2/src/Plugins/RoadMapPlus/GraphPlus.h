/*
 Menge Crowd Simulation Framework

 Copyright and trademark 2012-17 University of North Carolina at Chapel Hill

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
 or
    LICENSE.txt in the root of the Menge repository.

 Any questions or comments should be sent to the authors menge@cs.unc.edu

 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 *	@file		Graph.h
 *	@brief		The definition of a graph for performing graph searches
 *				and path planning.
 */

#ifndef __GRAPHPLUS_H__
#define	__GRAPHPLUS_H__

#include "MengeCore/Core.h"
#include "MengeCore/mengeCommon.h"
#include "MengeCore/resources/GraphVertex.h"
#include "MengeCore/resources/Resource.h"
#include "MengeCore/resources/GraphEdge.h"
#include "MengeCore/resources/RoadMapPath.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQuery.h"
#include "MengeCore/Math/Vector2.h"

#include "MinHeapPlus.h"


namespace ROAD {
	/*!
	 *	@brief		A roadmap graph and the infrastructure for performing graph
	 *				searches.  NOTE: This implementation assumes that the graph 
	 *				doesn't change.
	 */
	class Graph : public Menge::Resource {
	public:
		/*!
		 *	@brief		Constructor.
		 *
		 *	@param		fileName		The name of the file which contains the vector field
		 *								definition.
		 */
		Graph( const std::string & fileName );

	protected:
		/*!
		 *	@brief		Destructor.
		 */
		~Graph();

	public:
		/*!
		 *	@brief		Returns a unique resource label to be used to identify
		 *				different resource *types* which use the same underlying
		 *				file data.
		 */
		virtual const std::string & getLabel() const { return LABEL; }

		/*!
		 *	@brief		Clears the graph -- such that it has no vertices and no edges.
		 */
		void clear();

		/*!
		 *	@brief		Parses a graph definition and returns a pointer to it.
		 *
		 *	This function works in conjunction with the ResourceManager.  That is why it
		 *	returns a pointer, not to a Graph, but to a Resource.  The ResourceManager
		 *	uses it to load and instantiate Graph instances.
		 *
		 *	@param		fileName		The path to the file containing the VectorField
		 *								definition.
		 *	@returns	A pointer to the new Graph (if the file is valid), NULL if
		 *				invalid.
		 */
		static Resource * load( const std::string & fileName );
		
		/*!
		 *	@brief		Compute path
		 *
		 *	@param		agent		The agent for whom to compute the path.
		 *	@param		goal		The agent's goal.
		 *	@returns	A pointer to a RoadMapPath.  If there is an error,
		 *				the poitner will be NULL.
		 */
		Menge::RoadMapPath * getPath( const Menge::Agents::BaseAgent * agent, const Menge::BFSM::Goal * goal );

		/*!
		 *	@brief		Return the number of vertices in the graph.
		 *
		 *	@returns	The number of vertices in the graph.
		 */
		inline size_t getVertexCount() const { return _vCount; }

		/*!
		 *	@brief		Returns a const pointer to the ith vertex.
		 *
		 *	The validitiy of the index is only checked in debug mode with
		 *	an assertion.
		 *
		 *	@param		i		The index of the desired pointer.
		 *	@returns	A const pointer to the ith graph vertex.
		 */
		const Menge::GraphVertex * getVertex( size_t i ) const;

		/*!
		 *	@brief		The unique label for this data type to be used with 
		 *				resource management.
		 */
		static const std::string LABEL;

	protected:

		/*!
		 *	@brief		Find the closest visible graph vertex to the given
		 *				point.
		 *
		 *	@param		point		The point to connect to the graph.
		 *	@param		radius		The radius of the agent testing.
		 *	@returns	The index of the closest node.
		 */
		size_t getClosestVertex( const Menge::Vector2 & point, float radius );

		/*!
		 *	@brief		Computes the shortest path from start to end vertices.
		 *
		 *	This function instantiates a new path, but the caller is responsible
		 *	for deleting it.
		 *
		 *	@param		startID		The index of the start vertex.
		 *	@param		endID		The index of the end vertex.
		 *	@returns	A pointer to a new RoadMapPath.
		 */
		Menge::RoadMapPath * getPath( size_t startID, size_t endID );

		/*!
		 *	@brief		Compute's "h" for the A* algorithm.  H is the estimate of the
		 *				cost of a node to a goal point.  In this case, simply Euclidian
		 *				distance.
		 *
		 *	@param		v			The vertex to estimate the cost.
		 *	@param		goal		The goal point.
		 *	@returns	The h-value.
		 */
		inline float computeH( size_t v, const Menge::Vector2 & goal ) {
			return abs( _vertices[v].getPosition() - goal );
		}

		/*!
		 *	@brief		The number of vertices.
		 */
		size_t _vCount;

		/*!
		 *	@brief		An array containing all vertices.
		 */
		Menge::GraphVertex *	_vertices;

		/*!
		 *	@brief		Initializes the heap memory based on current
		 *				graph state.
		 */
		 void initHeapMemory();

		/*!
		 *	@brief		The size of a block of data used for COST in
		 *				the A* algorithm  (3N, N = number of nodes)
		 */
		size_t DATA_SIZE;

		/*!
		 *	@brief		The size of a block of data used for STATE in
		 *				the A* algorithm  (2N, N = number of nodes)
		 */
		size_t STATE_SIZE;

		/*!
		 *	@brief		The full set of data to serve as the heap
		 *				There are N entries in a single heap
		 *				and one heap per thread.
		 */
		unsigned int * _HEAP;	

		/*!
		 *	@brief		The full set of data for reconstructing the path.
		 *				For any given entry i, the value at i is the index of
		 *				the node from which i was reached. There is one block
		 *				per thread.
		 */
		unsigned int * _PATH;

		/*!
		 *	@brief		The block of data for tracking the f, g, and h data for nodes.
		 *				(This is where DATA_SIZE = 3N comes from.)  One block for
		 *				each active thread.
		 *				The first N values in a block are the f values, the next N are the g
		 *				values, and the last set of N values are the h values.
		 */
		float * _DATA;	

		/*!
		 *	@brief		Block of data for reportin node state (if its in a heap or if its
		 *				no longer used for calculation.  First N booleans are "in heap", second
		 *				N are "finished".  One block of 2N booleans per thread.
		 */
		bool *	_STATE;
	};

	/*!
	 *	@brief		The definition of the managed pointer for Graph data
	 */
	typedef Menge::ResourcePtr< Graph > GraphPtr;

	/*!
	 *	@brief		        Loads the graph of the given name
	 *
	 *	@param	 fileName   The name of the file containing the graph definition.
	 *	@returns	        The GraphPtr containing the data.
	 *	@throws		        A  ResourceException if the data is unable to be instantiated.
	 */
	GraphPtr loadGraph( const std::string & fileName ) throw ( Menge::ResourceException );
}	// namespace Menge

#endif	// __GRAPH_H__
