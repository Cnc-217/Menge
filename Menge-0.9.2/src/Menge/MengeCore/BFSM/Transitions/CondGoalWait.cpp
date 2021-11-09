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
#include "MengeCore/Core.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/Agents/SimulatorInterface.h"

#define EATTIME 20
#define BUYTIME 40
#define PLAYTIME 80
#define NOISETIME 10
#define IOTIME 10

using namespace Menge::Olympic;
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
		//agent��test conditionMet,Ȼ��onenter��Ȼ���޸�currnode
		void GoalWaitCondition::onEnter(Agents::BaseAgent* agent) {
			if (_triggerTimes.find(agent->_id) != _triggerTimes.end()) { //�������
				_lock.lockWrite();
				_triggerTimes[agent->_id] = 0;//��ֵ��ʱ��Ҫ��������ʼ��Ϊ0
				_statusAgents[agent->_id] = 0;//��ʼ״̬Ϊ0
				_reachedAgents[agent->_id] = false;//��ʼ��Ϊfalse
				_lock.releaseWrite();
			}
			else { //��������ڣ�
				_triggerTimes.insert(std::map< size_t, float >::value_type(agent->_id, 0));
				_reachedAgents.insert(std::map< size_t, float >::value_type(agent->_id, false));
				_statusAgents[agent->_id] = 0;//��ʼ״̬Ϊ0
				//insert��kv���Ѵ��ڵ�����£����Ե�ǰinsert���
			}
		}

		///////////////////////////////////////////////////////////////////////////

		bool GoalWaitCondition::conditionMet(Agents::BaseAgent* agent, const Goal* goal)
		{
			float distSq = goal->squaredDistance(agent->_pos);
			int agentId = agent->_id;
			bool reached = (distSq <= _distSq);
			if (reached) {
				_lock.lockWrite();
				_reachedAgents[agentId] = true;
				_lock.releaseWrite();
			}
			else
				return false;
			//����һЩ�ֲ�����
			float serviceTime;
			int goalId = goal->getID();
			int serviceMax = shopInfo[goalId].serviceMax;
			int blockMax = shopInfo[goalId].blockMax;
			int shopType = shopInfo[goalId].type;
			switch (shopType)//�˴������ٸĸ���ϸ��ʱ��
			{
			case 0:serviceTime = EATTIME; break;//��
			case 1:serviceTime = BUYTIME; break;//��
			case 2:serviceTime = PLAYTIME; break;//��
			case 3:serviceTime = NOISETIME; break;//���
			case 4:serviceTime = IOTIME; break;//�����
			default:break;
			}
			switch (_statusAgents[agentId])//  0δ����  1����  2���ܷ���  3����  
			{
				/*  ת��  0->1  */
			case 0: {
				if (_reachedAgents[agentId])
				{
					if (shopType == 4)//��Գ����  �����������  ���ȴ�ֱ��ѡ����һ��goal
					{
						if (agent->_shopGone.size() == 5)
							agent->_shopGone.pop_front();
						agent->_shopGoneNum = 1;
<<<<<<< HEAD
						agent->_shopGone.push_back(goalId);
=======
						agent->_shopGone2.push_back(goalId);
						
>>>>>>> 72121f7f5cc63a289c2500eb3211bdfc6c0cab9a
						return true;
					}
					_statusAgents[agentId] = 1;
					if (agentId == 1)
						cout << "0-1" << "+" << shopInfo[goalId].serviceMax << endl;
				}
			}; break;
				/*  ת��  1->2  1->3 */
			case 1: {
				 if (shopInfo[goalId].serviceQ.size() < serviceMax && shopInfo[goalId].blockQ.size()==0 )//���������λ&&û�������������Ŷ�
				{
					_statusAgents[agentId] = 2;					//agent->_status = 2;
					_triggerTimes[agentId] = Menge::SIM_TIME + serviceTime;
					_lock.lockWrite();
					shopInfo[goalId].serviceQ.push(agentId);//ID��������β��
					_lock.releaseWrite();
					if (agentId == 1)
					{
						cout << "1-2" << "+" << shopInfo[goalId].blockMax << "+" << shopInfo[goalId].serviceMax << "+ cango 2 :" << verticesCanGo[75] << endl;
						/*if (Menge::SIM_TIME > 1000 && Menge::SIM_TIME < 2000)
						{
							verticesCanGo[75] = 0;
							cout << "time is : " << Menge::SIM_TIME << "+ cango 2 :" << verticesCanGo[75] << endl;
						}*/
					}
						
				}
				else
				{//����������֮ǰ�ȿ��Ƿ�ﵽ����  ����ֱ���뿪
					 if (shopInfo[goalId].blockQ.size() == blockMax)
					 {
						 _statusAgents[agentId] = 0;					//agent->_status = 0;
						 _reachedAgents[agentId] = false;
						 return true;
					 }//����ֱ�ӽ���������
					_statusAgents[agentId] = 3;					//agent->_status = 3;
					//_triggerTimes[agentId] = Menge::SIM_TIME + (shopInfo[goalId].blockQ.size()+ shopInfo[goalId].serviceQ.size())*servicetime;//�˴����޸���Ϊ����������Ŷ�ʱ��Ϊ�������+��������������*����ʱ��
					_lock.lockWrite();
					shopInfo[goalId].blockQ.push(agentId);//ID����������β��
					_lock.releaseWrite();
					if (agentId == 1)
						cout << "1-3" << "+"<< _triggerTimes[agentId] <<endl;
				}
			}; break;

			case 2: {				/*  ת��  2->0 */
				if (Menge::SIM_TIME > _triggerTimes[agentId] && agentId == shopInfo[goalId].serviceQ.front())
				{//�����ͷagentʱ�䵽��&& �����ͷagent���Լ�
					_statusAgents[agentId] = 0;					//agent->_status = 0;
					_reachedAgents[agentId] = false;
					_lock.lockWrite();
					shopInfo[goalId].serviceQ.pop();	//agent���������;
					_lock.releaseWrite();
					int lastShopGone = agent->_shopGone.back();
					if (agent->_shopGone.size() == 5)
						agent->_shopGone.pop_front();
					agent->_shopGone.push_back(goalId);
					if (shopInfo[goalId].goalSet == shopInfo[lastShopGone].goalSet)
						agent->_shopGoneNum++;
					else
						agent->_shopGoneNum = 1;
					if (agentId == 1)
						cout << "2-0" <<"+"<< agent->_shopGone.front() << endl;
					return true;					//������
				};
			}; break;
				/*  ת��  3->2 */
			case 3: {
				if (agentId == 1)
					cout << shopInfo[goalId].serviceQ.size() <<"+"<< serviceMax <<"+" << shopInfo[goalId].blockQ.front() <<"+"<< agentId << endl;
				if ((shopInfo[goalId].serviceQ.size() < serviceMax) && (agentId == shopInfo[goalId].blockQ.front()))
				{//���������λ&& ������ͷagent���Լ�
					_statusAgents[agentId] = 2;					//agent->_status = 2;
					_triggerTimes[agentId] = Menge::SIM_TIME + serviceTime;//�˴������ٸ���ϸ��ʱ��
					_lock.lockWrite();
					shopInfo[goalId].blockQ.pop();			  //agent����������;
					shopInfo[goalId].serviceQ.push(agentId);//agent���������;
					_lock.releaseWrite();
					if (agentId == 1)
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
