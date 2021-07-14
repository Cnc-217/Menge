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

#include "MengeCore/BFSM/Transitions/CondLeaderGoal.h"

#include "MengeCore/Core.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/Goals/GoalAgent.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Runtime/Logger.h"

namespace Menge {

	namespace BFSM {

		///////////////////////////////////////////////////////////////////////////
		//                   Implementation of GoalCondition
		///////////////////////////////////////////////////////////////////////////

		LeaderGoalCondition::LeaderGoalCondition(): Condition(),_distSq(0.f) {
		}

		///////////////////////////////////////////////////////////////////////////

		LeaderGoalCondition::LeaderGoalCondition( const LeaderGoalCondition& cond ) : Condition(cond) {
			_distSq = cond._distSq;
		}

		///////////////////////////////////////////////////////////////////////////

		bool LeaderGoalCondition::conditionMet( Agents::BaseAgent * agent, const Goal * goal ) {
			//agent������panicer����normalagent��goalһ����agentgoal
			
			AgentGoal* agentGoal = (AgentGoal*)goal;//agentGoal
			size_t currenState = Menge::ACTIVE_FSM->getAgentStateID(agent);
			float distSq = 0;
			const Goal* leadersGoal = 0x0;
			if (Menge::ACTIVE_FSM->getNode(currenState)->getID()==1) {//Follow_leader
				//��ǰagent״̬Ϊfollowleader��leader��goalһ����goalset0�У�leaderΪ������
				leadersGoal = Menge::ACTIVE_FSM->getNode("Evacuation_lead")->getGoal(agentGoal->_leaderID);
				distSq = leadersGoal->squaredDistance(agent->_pos);
				return distSq <= _distSq;
			}
			else {//����Ǹ���panicer,��ô�ж�һ��panic��������߻����ڸ���leader
				//panicer���ѡ��̶�Ŀ������			
				Agents::BaseAgent* panicAgent = Menge::SIMULATOR->getAgent(agentGoal->_leaderID);//�õ�panic��agent
				size_t panicState = Menge::ACTIVE_FSM->getAgentStateID(panicAgent);
				if (Menge::ACTIVE_FSM->getNode(panicState)->getID()==3) {//panic��stateΪEvacuation_random
					const Goal* panicsGoal = Menge::ACTIVE_FSM->getNode("Evacuation_random")->getGoal(panicAgent->_id);//ͨ��panic��agentid�õ�������goal
					distSq = panicsGoal->squaredDistance(agent->_pos);
					return distSq <= _distSq;
				} 
				else if (Menge::ACTIVE_FSM->getNode(panicState)->getID() == 1) {//panic����������,��������̶�goal�ƶ���panic��stateΪFollow_leader
					AgentGoal* panicsGoal = (AgentGoal*)Menge::ACTIVE_FSM->getNode("Follow_leader")->getGoal(panicAgent->_id);//ͨ��panic��agent�õ�������goal
					leadersGoal = Menge::ACTIVE_FSM->getNode("Evacuation_lead")->getGoal(panicsGoal->_leaderID);//�õ�leader��goal
					distSq = leadersGoal->squaredDistance(agent->_pos);
					return distSq <= _distSq;
				}
				else if (Menge::ACTIVE_FSM->getNode(panicState)->getID() == 4) {//panic�Ѿ����ˣ�stop״̬
					distSq = panicAgent->_pos.distance(agent->_pos);
					return distSq <= _distSq;

				}
				
			}
			
			/*
			float distSq = 0;
			distSq = goal->squaredDistance(agent->_pos);
			return distSq <= _distSq*3;*/

			return false;
		}

		///////////////////////////////////////////////////////////////////////////

		Condition * LeaderGoalCondition::copy() {
			return new LeaderGoalCondition( *this );
		}

		///////////////////////////////////////////////////////////////////////////
		//                   Implementation of GoalCondFactory
		/////////////////////////////////////////////////////////////////////

		LeaderGoalCondFactory::LeaderGoalCondFactory() : ConditionFactory() {
			_distanceID = _attrSet.addFloatAttribute( "distance", false /*required*/,
													  0.f /*default*/ );
		}

		///////////////////////////////////////////////////////////////////////////

		bool LeaderGoalCondFactory::setFromXML( Condition * condition, TiXmlElement * node,
										  const std::string & behaveFldr ) const {
			LeaderGoalCondition* gCond = dynamic_cast<LeaderGoalCondition* >( condition );
			assert( gCond != 0x0 &&
					"Trying to set the properties of a goal condition on an incompatible object" );

			if ( !ConditionFactory::setFromXML( gCond, node, behaveFldr ) ) return false;

			gCond->setMinDistance( _attrSet.getFloat( _distanceID ) );
			return true;
		}

	}	// namespace BFSM 
}	// namespace Menge
