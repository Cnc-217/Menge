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

#include "MengeCore/BFSM/Transitions/CondGoalWait.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/Runtime/Logger.h"
#include "MengeCore/BFSM/Transitions/myQueue.h"
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/Agents/SimulatorInterface.h"

namespace Menge {

	namespace BFSM {

		///////////////////////////////////////////////////////////////////////////
		//                   Implementation of GoalWaitCondition
		///////////////////////////////////////////////////////////////////////////

		GoalWaitCondition::GoalWaitCondition(): Condition(),_distSq(0.f) {
		}

		///////////////////////////////////////////////////////////////////////////

		GoalWaitCondition::GoalWaitCondition( const GoalWaitCondition & cond ) : Condition(cond) {
			_distSq = cond._distSq;
			/*for (int i = 0; i < SIMULATOR->getNumAgents(); i++) {
				_lock.lockWrite();
				_triggerTimes.insert(std::map< size_t, float >::value_type(i, 0));
				_reachedAgents.insert(std::map< size_t, float >::value_type(i, false));
				_lock.releaseWrite();
			}*/

		}
		///////////////////////////////////////////////////////////////////////////
		//agent先test conditionMet,然后onenter，然后修改currnode
		void GoalWaitCondition::onEnter(Agents::BaseAgent* agent) {
			if (_triggerTimes.find(agent->_id) != _triggerTimes.end()) { //如果存在
				_lock.lockWrite();
				_triggerTimes[agent->_id] = 0;//赋值的时候要用锁，初始化为0
				_statusAgents[agent->_id] = 0;//初始状态为0
				_reachedAgents[agent->_id] = false;//初始化为false
				_lock.releaseWrite();
			}
			else { //如果不存在：
				_triggerTimes.insert(std::map< size_t, float >::value_type(agent->_id, 0));
				_reachedAgents.insert(std::map< size_t, float >::value_type(agent->_id, false));
				_statusAgents[agent->_id] = 0;//初始状态为0
				//insert在kv对已存在的情况下，忽略当前insert语句
			}
			
		}
		///////////////////////////////////////////////////////////////////////////店铺初始化  该放哪？


		///////////////////////////////////////////////////////////////////////////

		bool GoalWaitCondition::conditionMet(Agents::BaseAgent* agent, const Goal* goal)
		{
			float distSq = goal->squaredDistance(agent->_pos);
			float servicetime;
			switch (Menge::Olympic::shoptype[goal->getID()].type)//此处可以再改详细的时间  以后可以改成宏定义
			{
			case 0:servicetime = 20; break;//吃
			case 1:servicetime = 40; break;//买
			case 2:servicetime = 80; break;//玩
			default:break;
			}
			bool reached = (distSq <= _distSq);
			if (reached) {
				_lock.lockWrite();
				_reachedAgents[agent->_id] = true;
				_lock.releaseWrite();
			}
			else
				return false;
			switch (_statusAgents[agent->_id])//  0未到达  1到达  2接受服务  3阻塞  
			{
				/*  转移  0->1  */
			case 0: {
				if (_reachedAgents[agent->_id])
				{
					_lock.lockWrite();
					_statusAgents[agent->_id] = 1;
					_lock.releaseWrite();
					if (agent->_id == 1)
						cout << "0-1" << endl;
				}
			}; break;
				/*  转移  1->2  1->3 */
			case 1: {
				if (goal->getID() == 34 || goal->getID() == 35)
				{
					_lock.lockWrite();
					_statusAgents[agent->_id] = 0;					//agent->_status = 0;
					_reachedAgents[agent->_id] = false;
					_lock.releaseWrite();
					if (agent->_id == 1)
						cout << "1-0" << endl;//针对入口和出口  如果是出入口就直接返回进行下一个目标的选择
					return true;
				}
				else if (Menge::Olympic::shoptype[goal->getID()].serviceQ.size() < Menge::Olympic::shoptype[goal->getID()].maximum)//服务队列有位
				{
					
					_lock.lockWrite();
					_statusAgents[agent->_id] = 2;					//agent->_status = 2;
					_triggerTimes[agent->_id] = Menge::SIM_TIME + servicetime;
					Menge::Olympic::shoptype[goal->getID()].serviceQ.push(agent->_id);//ID插入服务队尾；
					_lock.releaseWrite();
					if (agent->_id == 1)
						cout << "1-2" << "+" << _statusAgents[agent->_id] << endl;
					agent->_prefSpeed = 0.1;
				}
				else
				{
					_lock.lockWrite();
					_statusAgents[agent->_id] = 3;					//agent->_status = 3;
					_triggerTimes[agent->_id] = Menge::SIM_TIME + (Menge::Olympic::shoptype[goal->getID()].blockQ.size()+ Menge::Olympic::shoptype[goal->getID()].serviceQ.size())*servicetime;//此处需修改因为理论上最长的排队时间为服务队列+阻塞队列总人数*服务时间
					
					Menge::Olympic::shoptype[goal->getID()].blockQ.push(agent->_id);//ID插入阻塞队尾；
					_lock.releaseWrite();
					if (agent->_id == 1)
						cout << "1-3" << "+"<< _triggerTimes[agent->_id] <<endl;
					agent->_prefSpeed = 0.1;
				}
			}; break;

			case 2: {				/*  转移  2->0 */
				if (Menge::SIM_TIME > _triggerTimes[agent->_id] && agent->_id == Menge::Olympic::shoptype[goal->getID()].serviceQ.front())
				{//服务队头agent时间到了&& 服务队头agent是自己
					_lock.lockWrite();
					_statusAgents[agent->_id] = 0;					//agent->_status = 0;
					_reachedAgents[agent->_id] = false;
					Menge::Olympic::shoptype[goal->getID()].serviceQ.pop();	//agent出服务队列;
					_lock.releaseWrite();
					if (agent->_id == 1)
						cout << "2-0" << endl;
					agent->_maxSpeed = 30;
					return true;					//出函数
				};
			}; break;
				/*  转移  3->2 */
			case 3: {
				if (agent->_id == 1)
					cout << Menge::Olympic::shoptype[goal->getID()].serviceQ.size() <<"+"<< Menge::Olympic::shoptype[goal->getID()].maximum<<"+" << Menge::Olympic::shoptype[goal->getID()].blockQ.front() <<"+"<< agent->_id << endl;
					if ((Menge::Olympic::shoptype[goal->getID()].serviceQ.size() < Menge::Olympic::shoptype[goal->getID()].maximum) && (int(agent->_id) == Menge::Olympic::shoptype[goal->getID()].blockQ.front()))
				{//服务队列有位&& 阻塞队头agent是自己
					_lock.lockWrite();
					_statusAgents[agent->_id] = 2;					//agent->_status = 2;
					_triggerTimes[agent->_id] = Menge::SIM_TIME + servicetime;//此处可以再改详细的时间
					Menge::Olympic::shoptype[goal->getID()].blockQ.pop();			  //agent出阻塞队列;
					Menge::Olympic::shoptype[goal->getID()].serviceQ.push(agent->_id);//agent进服务队列;
					_lock.releaseWrite();
					if (agent->_id == 1)
						cout << "3-2" << endl;
				}
			} break;
			}
			return false;

		}

		///////////////////////////////////////////////////////////////////////////

		Condition * GoalWaitCondition::copy() {
			return new GoalWaitCondition( *this );
		}

		///////////////////////////////////////////////////////////////////////////
		//                   Implementation of GoalWaitCondFactory
		/////////////////////////////////////////////////////////////////////

		GoalWaitCondFactory::GoalWaitCondFactory() : ConditionFactory() {
			_distanceID = _attrSet.addFloatAttribute( "distance", false /*required*/,
													  0.f /*default*/ );
		}

		///////////////////////////////////////////////////////////////////////////

		bool GoalWaitCondFactory::setFromXML( Condition * condition, TiXmlElement * node,
										  const std::string & behaveFldr ) const {
			GoalWaitCondition* gCond = dynamic_cast<GoalWaitCondition* >( condition );
			assert( gCond != 0x0 &&
					"Trying to set the properties of a goal condition on an incompatible object" );

			if ( !ConditionFactory::setFromXML( gCond, node, behaveFldr ) ) return false;

			gCond->setMinDistance( _attrSet.getFloat( _distanceID ) );
			return true;
		}

	}	// namespace BFSM 
}	// namespace Menge
