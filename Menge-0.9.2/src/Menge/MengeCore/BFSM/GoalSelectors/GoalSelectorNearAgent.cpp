/*

License

Menge
Copyright ?and trademark ?2012-14 University of North Carolina at Chapel Hill. 
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation 
for educational, research, and non-profit purposes, without fee, and without a 
written agreement is hereby granted, provided that the above copyright notice, 
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North 
Carolina at Chapel Hill. The software program and documentation are supplied "as is," 
without any accompanying services from the University of North Carolina at Chapel 
Hill or the authors. The University of North Carolina at Chapel Hill and the 
authors do not warrant that the operation of the program will be uninterrupted 
or error-free. The end-user understands that the program was developed for research 
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS 
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS 
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE 
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY 
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY 
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND 
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS 
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include "MengeCore/BFSM/GoalSelectors/GoalSelectorNearAgent.h"

#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Agents/BaseAgent.h"

#include <cassert>
#include <MengeCore/Core.h>
#include <MengeCore/Agents/SpatialQueries/SpatialQueryStructs.h>

namespace Menge {

	namespace BFSM {



		/////////////////////////////////////////////////////////////////////
		//                   Implementation of NearAgentGoalSelector
		/////////////////////////////////////////////////////////////////////
		
		Goal * NearAgentGoalSelector::getGoal( const Agents::BaseAgent * agent ) const {
			if (Menge::PROJECTNAME == EVACUATION) {
				State* currentState = Menge::ACTIVE_FSM->getCurrentState(agent);
				//����stateΪFollow_leader
				if (_goalSetID==1) {  //����Follow_leader��Ҫ�ڵ�ǰgoalset���ҵ��Ǹ���agent�ڽ���leader
					std::vector<Menge::Agents::NearAgent> nearAgents = agent->_nearAgents;
					vector<Menge::Agents::NearAgent>::iterator nearAgent;
					for (nearAgent = nearAgents.begin(); nearAgent != nearAgents.end(); ++nearAgent) { //����������agent
						if ((*nearAgent).agent->_class == 0) { //��leader������Ӧ��agentGoal����
							return _goalSet->getGoalByID((*nearAgent).agent->_id);
						}
					}
				}
				//����stateΪFollow_panic
				else if (_goalSetID == 2) {//����Follow_panic��Ҫ�ڵ�ǰgoalset���ҵ��Ǹ���agent�ڽ���leader
					std::vector<Menge::Agents::NearAgent> nearAgents = agent->_nearAgents;
					vector<Menge::Agents::NearAgent>::iterator nearAgent;
					for (nearAgent = nearAgents.begin(); nearAgent != nearAgents.end(); ++nearAgent) { //����������agent
						if ((*nearAgent).agent->_class == 1) { //��panicer������Ӧ��agentGoal����
							return _goalSet->getGoalByID((*nearAgent).agent->_id);
						}
					}
				}
			}
			else return _goalSet->getRandomGoal();
		}

		/////////////////////////////////////////////////////////////////////

		

	}	// namespace BFSM
}	// namespace Menge
