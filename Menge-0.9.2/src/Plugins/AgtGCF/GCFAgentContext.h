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
 *  @file       GCFAgentContext.h
 *  @brief      A basic context for interacting with and displaying
 *				generalized centrifugal force agent parameters
 */

#ifndef __GCF_AGENT_CONTEXT_H__
#define __GCF_AGENT_CONTEXT_H__

#include "GCFAgent.h"

#include "MengeCore/Agents/Obstacle.h"
#include "
/Runtime/AgentContext/BaseAgentContext.h"

namespace GCF {
	// forward declaration
	class Agent;
	/*!
	 *	@brief		The context for displaying the computational aspects of the
	 *				Generalized Centifugal Force model (see GCF::Agent).
	 */
	class AgentContext : public MengeVis::Runtime::BaseAgentContext {
	public:
		/*!
		 *	@brief		Constructor.
		 */
		AgentContext();

		/*!
		 *	@brief		Destructor
		 */
		virtual ~AgentContext();

		/*!
		 *	@brief		Sets the agent for this context.
		 *
		 *	This method works in conjunction with the VisElementDatabase. When this
		 *	visualization element is triggered, the database will supply the triggering
		 *	element.
		 *
		 *	@param		agent		The agent to interact with.
		 */
		void setElement( const MengeVis::Runtime::VisAgent * agent ) override;

		/*!
		 *	@brief		Returns the name of the context for display.
		 *
		 *	@returns		The name of this context.
		 */
		std::string contextName() const override { return "GCF 2010"; }

		/*!
		 *	@brief		The value used to store this element in the visual element database.
		 *				This string value should correspond to the getStringId method of the
		 *				corresponding simulation element.
		 */
		virtual std::string getElementName() const { return Agent::NAME; }

		/*!
		 *	@brief		Give the context the opportunity to respond to a keyboard
		 *				event.
		 *
		 *	@param		e		The SDL event with the keyboard event data.
		 *	@returns	A ContextResult instance reporting if the event was handled and
		 *				if redrawing is necessary.
		 */
		virtual MengeVis::SceneGraph::ContextResult handleKeyboard( SDL_Event & e );

		/*!
		 *	@brief		Allow the context to update any time-dependent state it might have to
		 *				the given global time.
		 */
		virtual void update(); 

	protected:

		/*!
		 *	@brief		Draw UI elements into the context.
		 *
		 *	@param		vWidth		The width of the viewport (in pixels).
		 *	@param		vHeight		The height of the viewport (in pixels).
		 *	@param		select		Defines if the drawing is being done for selection
		 *							purposes (true) or visualization (false).
		 */
		virtual void drawUIGL( int vWidth, int vHeight, bool select=false );

		/*!
		 *	@brief		Draw context elements into the 3D world.
		 *
		 *	@param		select		Defines if the drawing is being done for selection
		 *							purposes (true) or visualization (false).
		 */
		virtual void draw3DGL( bool select=false );

		/*!
		 *	@brief		Creates a formatted string to be printed in the context
		 *				for a particular agent
		 *
		 *	@param		agent		The agent whose data is to be displayed.
		 *	@returns	A formatted string for display in the context's 2D gui.
		 */
		virtual std::string agentText( const Menge::Agents::BaseAgent * agent ) const;

		/*!
		 *	@brief		Determines if the force vectors are drawn
		 */
		bool		_showForce;

		/*!
		 *	@brief		Determines if the foces are drawn proportionately (false) or 
		 *				purely the direction (true).
		 */
		bool		_onlyForceDir;

		/*!
		 *	@brief		The id of the forceObject to display.
		 *				If zero, all agents, if positive, a single agent
		 *				if negative, an obstacle
		 */
		int		_forceObject;

		/*!
		 *	@brief		A sampling of force responses based on effective distance.
		 */
		float *		_responses;

		/*!
		 *	@brief		Cycles the single entity whose force is being drawn.
		 *
		 *	@param		agt			The agent whose forces are being drawn.
		 *	@param		forward		If true, cycles forward, if false, cycles backwards.
		 *	@returns	True if a redraw is required.
		 */
		bool cycleSingleForce(const Agent * agt,  bool forward );

		/*!
		 *	@brief		Function for drawing the force vectors acting on agt
		 *
		 *	@param		agt			The agent whose forces are displayed.
		 */
		void drawForces( const Agent * agt );

		/*!
		 *	@brief		Draw the force vector from srcAgent acting on agt
		 *
		 *	@param		agt			The agent on which the force is acting
		 *	@param		other		The agent imparting the force
		 *	@param		thresh		The minimum force magnitude required to draw
		 */
		void singleAgentForce( const Agent * agt, const Agent * other, float thresh=0.5f );

		/*!
		 *	@brief		Draw the force vector from an obstacle acting on agt
		 *
		 *	@param		agt			The agent on which the force is acting
		 *	@param		obst		The obstacle imparting the force
		 *	@param		thresh		The minimum force magnitude required to draw
		 */
		void singleObstacleForce( const Agent * agt, const Agents::Obstacle * obst,
								  float thresh=0.5f );

		/*!
		 *	@brief		Draws the given force on the given agent
		 *				It assumes the force color has already been set.
		 *
		 *	@param		agt			A pointer to the agent on which the force is acting
		 *	@param		forceDir	The direction of the force vector to draw
		 *	@param		forceMag	The magnitude of the underlying force.
		 *	@param		label		The label to apply to the force.
		 */
		void drawForce( const Agent * agt, const Menge::Math::Vector2 & forceDir, float forceMag, 
			            const std::string & label );

		/*!
		 *	@brief		Initializes the responses curve
		 *
		 *	@param		Agent to use to build the response curve from.
		 */
		void initResponses(const Agent * agt);
	};
}	// namespace GCF
#endif	 // __GCF_AGENT_CONTEXT_H__
